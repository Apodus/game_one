
#include "wave.hpp"
#include "signal.hpp"

#include "constants.hpp"
#include "sampler.hpp"
#include "samplers/instrument.hpp"
#include "samplers/constant.hpp"

#include "note.hpp"

#include "thread/TaskManager.hpp"
#include "util/attributes.hpp"
#include "graphics/opengl/opengl.hpp"

#include <iostream>
#include <string>
#include <fstream>
#include <cstdint>
#include <vector>
#include <algorithm>

#include <memory>

constexpr size_t windowSize = 2048;
constexpr size_t jumpSize = 1024;



inline float mix(float v) {
	return std::tanh(v);
}

float g_aspectRatio = 1.0f;


#include "graphics/window/window.hpp"
#include "input/userio.hpp"
#include "util/fps_manager.hpp"
#include "util/timer.hpp"
#include "graphics/texture/texturehandler.hpp"
#include "graphics/camera/camera.hpp"
#include "graphics/shader/shaders.hpp"
#include "graphics/renderer/textrenderer.hpp"
#include "graphics/renderer/meshrenderer.hpp"
#include "graphics/graphics.hpp"
#include "math/2d/polygonModifier.hpp"
#include "messagesystem/TypedMessage.hpp"
#include "graphics/window/messages.hpp"
#include "graphics/opengl/opengl.hpp"

#include "graphics/graphics.hpp"
#include "menu/MenuLogicalFrame.hpp"
#include "graphics/color.hpp"

#include "math/2d/mesh.hpp"
#include "math/2d/polygonTesselator.hpp"
#include "math/matrix/matrix4.hpp"
#include "menu/MenuFrame.hpp"

class Menu : public sa::MenuLogicalFrame {
public:
	Menu(std::shared_ptr<sa::Window> window,
		std::shared_ptr<sa::UserIO> userio,
		const std::string& name,
		const sa::vec3<float>& position,
		const sa::vec3<float>& scale
	) : sa::MenuLogicalFrame(window, userio, name, position, scale)
	{}

private:
	std::shared_ptr<sa::Camera> pCamera;
	std::shared_ptr<sa::Shaders> pShaders;
	std::shared_ptr<sa::TextRenderer> pTextRenderer;
	std::shared_ptr<sa::MeshRenderer> pRenderer;
	std::shared_ptr<sa::Graphics> pGraphics;
	std::shared_ptr<sa::Window> window;
};

class SignalLine : public sa::MenuLogicalFrame {
public:
	SignalLine(MenuComponent* parent, std::vector<int> signal, sa::vec3<float> pos)
		: sa::MenuLogicalFrame(parent, "SignalLine", [pos]() {return pos;}, []() { return sa::vec3<float>(2.0f, 0.1f, 0.0f); })
		, m_bg(this, "bg", "Frame")
		, m_signal(std::move(signal))
	{}

	void draw(std::shared_ptr<sa::Graphics> graphics) const
	{
		float y_offset = getPosition().y;
		float y_scale = getScale().y * 0.9f;
		for (size_t i = 3; i < m_signal.size(); i += 2) {
			float x1 = 20 * float(i - 3) / float(m_signal.size()) - 0.75f;
			float y1 = float(m_signal[i - 3]) / float(1 << 15);

			float x2 = 20 * float(i) / float(m_signal.size()) - 0.75f;
			float y2 = float(m_signal[i]) / float(1 << 15);

			x1 *= x_scale;
			x2 *= x_scale;

			x1 += x_pos;
			x2 += x_pos;

			y1 *= y_scale;
			y2 *= y_scale;

			y1 += y_offset;
			y2 += y_offset;

			if (x2 < -1 || x1 > +1)
				continue;

			graphics->m_pRenderer->drawLine({ x1, y1, 0 }, { x2, y2, 0 }, 0.003f, Color::GREEN);
		}
		
		m_bg.visualise(graphics);
	}

	void update(float dt)
	{
		m_bg.tick(dt);

		if (isMouseOver()) {
			if (m_pUserIO->isKeyDown(GLFW_KEY_LEFT)) {
				x_pos_target -= 0.2f;
			}
			if (m_pUserIO->isKeyDown(GLFW_KEY_RIGHT)) {
				x_pos_target += 0.2f;
			}
			if (m_pUserIO->isKeyDown(GLFW_KEY_UP)) {
				x_scale_target *= 1.04f;
			}
			if (m_pUserIO->isKeyDown(GLFW_KEY_DOWN)) {
				x_scale_target *= 0.96f;
			}
		}

		x_pos += (x_pos_target - x_pos) * 0.04f;
		x_scale += (x_scale_target - x_scale) * 0.04f;
	}

private:
	sa::MenuFrameBackground m_bg;
	std::vector<int> m_signal;

	float x_pos = 0;
	float x_pos_target = 0;
	float x_scale = 1.0f;
	float x_scale_target = 1.0f;
};

class Session {
public:
	Session() : fps_logic(Timer::time_now(), 60), fps_graphic(Timer::time_now(), 60), pCamera(new sa::Camera()), pShaders(new sa::Shaders()) {
		window = std::make_shared<sa::Window>();
		window->createWindow(1280, 720, "synth");

		userIO = std::make_shared<sa::UserIO>(window);

		OpenGL gl;
		sa::TextureHandler::getSingleton().createTextures("../textures/textures.dat");

		pCamera->setProjection(0.02f, 200.f, window->getAspectRatio());
		pCamera->setPosition({0, 0, +1});

		pTextRenderer = std::make_shared<sa::TextRenderer>(pShaders, pCamera);
		pRenderer = std::make_shared<sa::MeshRenderer>(pShaders, pCamera);
		pGraphics = std::make_shared<sa::Graphics>(pShaders, pRenderer, pTextRenderer);

		menuRoot = std::make_shared<Menu>(window, userIO, "MenuRoot", sa::vec3<float>(), sa::vec3<float>(1, 1.0f / window->getAspectRatio(), 1));
		
		/*
		{
		std::shared_ptr<sa::Shader> shader01 = pShaders->loadShader("blurHorizontal", "../shaders/blur.vs.glsl", "../shaders/blur_horizontalpass.fs.glsl");
		std::shared_ptr<sa::Shader> shader02 = pShaders->loadShader("blurVertical", "../shaders/blur.vs.glsl", "../shaders/blur_verticalpass.fs.glsl");
		std::shared_ptr<sa::Shader> shader03 = pShaders->loadShader("deferredAmbientLight", "../shaders/screenspace.vs.glsl", "../shaders/deferred_ambientlight.fs.glsl");
		std::shared_ptr<sa::Shader> shader04 = pShaders->loadShader("deferredLevel", "../shaders/deferred_level.vs.glsl", "../shaders/deferred_level.fs.glsl");
		std::shared_ptr<sa::Shader> shader08 = pShaders->loadShader("deferredUnit", "../shaders/unit.vs.glsl", "../shaders/unit.fs.glsl");
		std::shared_ptr<sa::Shader> shader05 = pShaders->loadShader("deferredLights", "../shaders/screenspace.vs.glsl", "../shaders/deferred_lights.fs.glsl");
		std::shared_ptr<sa::Shader> shader06 = pShaders->loadShader("skybox", "../shaders/skybox.vs.glsl", "../shaders/skybox.fs.glsl");
		std::shared_ptr<sa::Shader> shader07 = pShaders->loadShader("ssao", "../shaders/ssao_simple.vs.glsl", "../shaders/ssao_simple.fs.glsl");
		}
		*/
	}

	void setPositionsOfSignalLines() {
		if (m_signalLines.empty())
			return;

		m_signalLines[0]->setTargetPosition([this]() {
			// return menuRoot->getExteriorPosition(sa::MenuComponent::TOP) -
			return sa::vec3<float>(0, 0.5f * g_aspectRatio, 0) -
				sa::vec3<float>(0, m_signalLines[0]->getScale().y * 0.5f * g_aspectRatio, 0);
		});
		
		for (size_t i = 1; i < m_signalLines.size(); ++i) {
			m_signalLines[i]->setTargetPosition([this, i]() {
				return m_signalLines[i - 1]->getExteriorPosition(sa::MenuComponent::BOTTOM) -
					sa::vec3<float>(0, m_signalLines[i]->getScale().y * 0.5f * g_aspectRatio, 0);
			});
		}
	}

	void push(std::vector<int> signal) {
		sa::vec3<float> pos{0, 0, 0};
		auto signalLine = std::make_shared<SignalLine>(menuRoot.get(), std::move(signal), pos);
		m_signalLines.emplace_back(signalLine);
		menuRoot->addChild(signalLine);

		setPositionsOfSignalLines();
	}

	bool tick() {
		g_aspectRatio = window->getAspectRatio();

		long long timeNow = Timer::time_now();
		std::this_thread::sleep_for(std::chrono::milliseconds(16));
		
		userIO->update();
		auto timeSince = fps_logic.timeSince(timeNow);
		
		menuRoot->setTargetScale(sa::vec3<float>(1.8f, 1.8f / g_aspectRatio, 0));
		menuRoot->tick(0.016f);

		// visual tick
		if (true) {
			pRenderer->clearScreen();
			pRenderer->setCamera(pCamera);
			pRenderer->cameraToGPU();
			menuRoot->visualise(pGraphics);

			pRenderer->drawLine({ 0.2f, 0.0f, 0.0f }, menuRoot->getExteriorPosition(sa::MenuComponent::BOTTOM), 0.05f, Color::BROWN);
			pRenderer->drawLine({ 0.2f, 0.0f, 0.0f }, menuRoot->getExteriorPosition(sa::MenuComponent::TOP), 0.05f, Color::BROWN);
			pRenderer->drawLine({ 0.2f, 0.0f, 0.0f }, menuRoot->getExteriorPosition(sa::MenuComponent::LEFT), 0.05f, Color::BROWN);
			pRenderer->drawLine({ 0.2f, 0.0f, 0.0f }, menuRoot->getExteriorPosition(sa::MenuComponent::RIGHT), 0.05f, Color::BROWN);

			window->swap_buffers();
		}

		window->pollEvents();
		return !window->shouldClose();
	}

private:
	std::shared_ptr<sa::Camera> pCamera;
	std::shared_ptr<sa::Shaders> pShaders;
	std::shared_ptr<sa::TextRenderer> pTextRenderer;
	std::shared_ptr<sa::MeshRenderer> pRenderer;
	std::shared_ptr<sa::Graphics> pGraphics;
	std::shared_ptr<sa::Window> window;

	std::vector<std::shared_ptr<SignalLine>> m_signalLines;

	FPS_Manager fps_logic;
	FPS_Manager fps_graphic;

	std::shared_ptr<sa::UserIO> userIO;
	std::shared_ptr<Menu> menuRoot;
};




int main(int argc, char* argv[]) {
	{
		Instrument piano("piano", 44100, 200, 200);
		piano.harmonic(1).amplitude(1.0f);
		piano.harmonic(2).amplitude(.6f);
		piano.harmonic(3).amplitude(.6f);
		piano.initialize();

		Instrument violin("violin", 44100, 200, 200);
		violin.harmonic(1).amplitude(1.0f).modulatePhase(
			Oscillator().frequency(5).amplitude(1.0f * pi)
		);
		violin.harmonic(3).amplitude(0.6f).modulateAmp(
			Oscillator().amplitude(0.2f).volumeOffset(Constant(1.0f)).frequency(5)
		);
		violin.harmonic(5).amplitude(0.6f).modulateAmp(
			Oscillator().amplitude(0.2f).volumeOffset(Constant(1.0f)).frequency(5)
		);
		violin.harmonic(2).amplitude(0);
		violin.harmonic(4).amplitude(0);
		violin.initialize();


		Instrument bass("bass", 44100, 200, 200);

		bass.harmonic(1).amplitude(0);
		bass.harmonic(2).amplitude(1.0f).modulatePhase(
			Oscillator().frequency(5).amplitude(pi / 2.0f)
		);
		bass.harmonic(3).amplitude(0);
		bass.harmonic(4).amplitude(1.5f).modulateAmp(
			Oscillator().amplitude(0.06f).volumeOffset(Constant(1.0f)).frequency(5)
		).modulateFreq(Oscillator().frequency(6).amplitude(pi / 4.0f));
		bass.harmonic(5).amplitude(0);
		bass.harmonic(6).amplitude(1.2f).modulateAmp(
			Oscillator().amplitude(0.06f).volumeOffset(Constant(1.0f)).frequency(4)
		).modulatePhase(Oscillator().frequency(3).amplitude(pi / 4.0f));
		bass.harmonic(7).amplitude(0);
		bass.harmonic(8).amplitude(1.2f);
		bass.initialize();

		Note note1(bass, 110, 6000, 1000);
		Note note2(piano, 220, 6000, 2000);
		Note note3(violin, 440, 6000, 3000);

		std::vector<int> bass_out;
		std::vector<int> piano_out;
		std::vector<int> violin_out;
		bass_out.resize(4 * 44100, 0);
		piano_out.resize(4 * 44100, 0);
		violin_out.resize(4 * 44100, 0);

		note1.renderOn(bass_out, 0);
		note2.renderOn(piano_out, 0);
		note3.renderOn(violin_out, 0);

		auto clamp_to_range = [](std::vector<int>& out) {
			std::transform(out.begin(), out.end(), out.begin(), [](int v) { return static_cast<int>(32000 * mix(v / 32000.0f)); });
		};

		clamp_to_range(bass_out);
		clamp_to_range(piano_out);
		clamp_to_range(violin_out);

		// auto outWave = wave::saveMonoSignalAsWavPCM(out, 44100);
		// std::ofstream genOut("gen.wav", std::ios::binary);
		// genOut.write(reinterpret_cast<char*>(outWave.data()), outWave.size());

		Session session;
		session.push(bass_out);
		session.push(piano_out);
		session.push(violin_out);

		while (session.tick()) {}

		return 0;
	}
	/*
	{
		std::ifstream in("in/" + input, std::ios::binary);
		Signal source;
		source.fromTimeDomain(std::move(wave::loadWavPCMAsMonoSignal(std::move(in)).sampleRate(44100).samples()), windowSize, jumpSize);

		source.chunk(source.size() / 2).visualize();

		auto signal = source.render(jumpSize);
		auto outWave = wave::saveMonoSignalAsWavPCM(signal, 44100);
		std::ofstream genOut("gen.wav", std::ios::binary);
		genOut.write(reinterpret_cast<char*>(outWave.data()), outWave.size());

		std::cin.get();

		return 0;
	}

	std::vector<std::vector<int>> sampleSets;

	{
		std::ifstream in("in/" + input, std::ios::binary);
		sampleSets.emplace_back(wave::loadWavPCMAsMonoSignal(in).sampleRate(44100).samples());
	}

	// trying things out.
	for (auto&& sample : sampleSets) {
		Signal source;
		source.fromTimeDomain(sample, windowSize, jumpSize);

		{
			auto generatedSignal = source.render(jumpSize);
			{
				auto fileContents = wave::saveMonoSignalAsWavPCM(generatedSignal, 44100);
				std::ofstream out("out/passthrough.wav", std::ios::binary);
				out.write(reinterpret_cast<const char*>(fileContents.data()), fileContents.size());
			}
		}

		{
			Signal s = source;

			for (int i = 0;i < 2; ++i)
				s.apply([](Signal::Chunk& chunk) { chunk.shiftLeft(); });
			auto generatedSignal = s.render(jumpSize);

			{
				auto fileContents = wave::saveMonoSignalAsWavPCM(generatedSignal, 44100);
				std::ofstream out("out/shiftLeft.wav", std::ios::binary);
				out.write(reinterpret_cast<const char*>(fileContents.data()), fileContents.size());
			}
		}

		{
			Signal s = source;
			for (int i = 0;i < 2; ++i)
				s.apply([](Signal::Chunk& chunk) { chunk.shiftRight(); });
			auto generatedSignal = s.render(jumpSize);

			{
				auto fileContents = wave::saveMonoSignalAsWavPCM(generatedSignal, 44100);
				std::ofstream out("out/shiftRight.wav", std::ios::binary);
				out.write(reinterpret_cast<const char*>(fileContents.data()), fileContents.size());
			}
		}

		{
			Signal s = source;
			for (int i = 0;i < 2; ++i)
				s.apply([](Signal::Chunk& chunk) { chunk.scatter(); });
			auto generatedSignal = s.render(jumpSize);

			{
				auto fileContents = wave::saveMonoSignalAsWavPCM(generatedSignal, 44100);
				std::ofstream out("out/scatter.wav", std::ios::binary);
				out.write(reinterpret_cast<const char*>(fileContents.data()), fileContents.size());
			}
		}

		{
			NFrequencySignal s;
			s.fromTimeDomain(sample, windowSize, jumpSize);
			auto generatedSignal = s.render(jumpSize);

			std::ofstream maxOnlySave("out/max_freq.txt");
			s.apply([&](std::pair<uint32_t, float>& chunk) {
				maxOnlySave << chunk.first << " ";
			});

			{
				auto fileContents = wave::saveMonoSignalAsWavPCM(generatedSignal, 44100);
				std::ofstream out("out/filterPreserve1.wav", std::ios::binary);
				out.write(reinterpret_cast<const char*>(fileContents.data()), fileContents.size());
			}
		}

		{
			Signal s = source;
			s.apply([](Signal::Chunk& chunk) { chunk.filterLows(2); });
			auto generatedSignal = s.render(jumpSize);

			{
				auto fileContents = wave::saveMonoSignalAsWavPCM(generatedSignal, 44100);
				std::ofstream out("out/filterPreserve2.wav", std::ios::binary);
				out.write(reinterpret_cast<const char*>(fileContents.data()), fileContents.size());
			}
		}

		{
			Signal s = source;
			s.apply([](Signal::Chunk& chunk) { chunk.filterLows(3); });
			auto generatedSignal = s.render(jumpSize);

			{
				auto fileContents = wave::saveMonoSignalAsWavPCM(generatedSignal, 44100);
				std::ofstream out("out/filterPreserve3.wav", std::ios::binary);
				out.write(reinterpret_cast<const char*>(fileContents.data()), fileContents.size());
			}
		}

		{
			Signal s = source;
			auto generatedSignal = s.render(jumpSize * 3 / 2);

			{
				auto fileContents = wave::saveMonoSignalAsWavPCM(generatedSignal, 44100);
				std::ofstream out("out/widen150p.wav", std::ios::binary);
				out.write(reinterpret_cast<const char*>(fileContents.data()), fileContents.size());
			}
		}
	}

	std::cin.get();
	return 0;
	*/
}