
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

class Session {
public:
	Session() : fps_logic(Timer::time_now(), 60), fps_graphic(Timer::time_now(), 60), pCamera(new sa::Camera()), pShaders(new sa::Shaders()) {
		window = std::make_shared<sa::Window>();
		window->createWindow(1280, 720);

		userIO = std::make_shared<sa::UserIO>(window);

		OpenGL gl;
		sa::TextureHandler::getSingleton().createTextures("../textures/textures.dat");

		pCamera->setProjection(0.02f, 200.f, window->getAspectRatio());
		pCamera->setPosition({0, 0, +1});

		pTextRenderer = std::make_shared<sa::TextRenderer>(pShaders, pCamera);
		pRenderer = std::make_shared<sa::MeshRenderer>(pShaders, pCamera);
		pGraphics = std::make_shared<sa::Graphics>(pShaders, pRenderer, pTextRenderer);

		menuRoot = std::make_shared<Menu>(window, userIO, "MenuRoot", sa::vec3<float>(), sa::vec3<float>(1, 1, 1));
		
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

	bool tick() {
		long long timeNow = Timer::time_now();
		std::this_thread::sleep_for(std::chrono::milliseconds(16));
		
		userIO->update();
		auto timeSince = fps_logic.timeSince(timeNow);
		
		// visual tick
		if (true) {
			pRenderer->clearScreen();
			pRenderer->setCamera(pCamera);
			pRenderer->cameraToGPU();
			pRenderer->drawLine({ timeSince * 0.01f, timeSince * 0.01f, 0 }, { -timeSince * 0.01f, -timeSince * 0.01f, 0 }, 0.005f, Color::GREEN);
			window->swap_buffers();
		}

		return !window->shouldClose();
	}

private:
	std::shared_ptr<sa::Camera> pCamera;
	std::shared_ptr<sa::Shaders> pShaders;
	std::shared_ptr<sa::TextRenderer> pTextRenderer;
	std::shared_ptr<sa::MeshRenderer> pRenderer;
	std::shared_ptr<sa::Graphics> pGraphics;
	std::shared_ptr<sa::Window> window;

	FPS_Manager fps_logic;
	FPS_Manager fps_graphic;

	std::shared_ptr<sa::UserIO> userIO;
	std::shared_ptr<Menu> menuRoot;
};




int main(int argc, char* argv[]) {
	
	// first steps:
	// start UI session.
	// generate content for UI.
	// display UI
	// have button to play current buffer

	Session session;
	
	while (session.tick()) {
	}
	
	std::string input;
	if (argc <= 1) {
		std::cout << "Input wave file name: ";
		std::cin >> input;
		std::cin.get();
	}
	else {
		input = argv[1];
		std::cout << "Input wave file name: " << input << std::endl;
	}

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
			Oscillator().amplitude(0.2f).volumeOffset(Constant(1.0f))
		);
		violin.harmonic(5).amplitude(0.6f).modulateAmp(
			Oscillator().amplitude(0.2f).volumeOffset(Constant(1.0f))
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
			Oscillator().amplitude(0.06f).volumeOffset(Constant(1.0f))
		).modulateFreq(Oscillator().frequency(6).amplitude(pi / 4.0f));
		bass.harmonic(5).amplitude(0);
		bass.harmonic(6).amplitude(1.2f).modulateAmp(
			Oscillator().amplitude(0.06f).volumeOffset(Constant(1.0f))
		).modulatePhase(Oscillator().frequency(3).amplitude(pi / 4.0f));
		bass.harmonic(7).amplitude(0);
		bass.harmonic(8).amplitude(1.2f);
		bass.initialize();

		Note note1(bass, 110, 6000, 1000);
		Note note2(piano, 220, 6000, 2000);
		Note note3(violin, 440, 6000, 3000);

		std::vector<int> out;
		out.resize(10 * 44100, 0);

		note1.renderOn(out, 0);
		note2.renderOn(out, 64500);
		note3.renderOn(out, 200000);

		std::transform(out.begin(), out.end(), out.begin(), [](int v) { return static_cast<int>(32000 * mix(v / 32000.0f)); });

		auto outWave = wave::saveMonoSignalAsWavPCM(out, 44100);
		std::ofstream genOut("gen.wav", std::ios::binary);
		genOut.write(reinterpret_cast<char*>(outWave.data()), outWave.size());

		std::cin.get();
		return 0;
	}

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
}