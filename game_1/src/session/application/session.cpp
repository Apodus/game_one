
#include "session.hpp"
#include "graphics/opengl/opengl.hpp"


Session::Application::Session::Session() : fps_logic(Timer::time_now(), 60), fps_graphic(Timer::time_now(), 60), pCamera(new sa::Camera()), pShaders(new sa::Shaders()) {
	window = std::make_shared<sa::Window>();
	window->createWindow(1280, 720);

	userIO = std::make_shared<sa::UserIO>(window);

	OpenGL gl;
	sa::TextureHandler::getSingleton().createTextures("../textures/textures.dat");

	pCamera->setProjection(0.02f, 200.f, window->getAspectRatio());

	pTextRenderer = std::make_shared<sa::TextRenderer>(pShaders, pCamera);
	pRenderer = std::make_shared<sa::MeshRenderer>(pShaders, pCamera);
	pGraphics = std::make_shared<sa::Graphics>(pShaders, pRenderer, pTextRenderer);

	menuRoot = std::make_shared<sa::MenuRoot>(window, userIO, "MenuRoot", sa::vec3<float>(), sa::vec3<float>(1, 1, 1));
	game = std::make_unique<Game>(menuRoot, userIO);
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

bool Session::Application::Session::tick() {

	long long timeNow = Timer::time_now();

	game->preTick();
	if (fps_logic.canTick(timeNow))
	{
		float dt = fps_logic.tick(timeNow);
		
		// TODO: GAME TICK
		menuRoot->tick(dt);
		game->update(userIO, window->getAspectRatio());
		game->tick(timeNow);
		userIO->update();
	}

	if (fps_graphic.canTick(timeNow))
	{
		float dt_sinceLastGraphicsFrame = fps_graphic.tick(timeNow);
		float time_alpha = fps_logic.timeSince(Timer::time_now()) / fps_logic.frameLength();

		pRenderer->clearScreen();
		pCamera->setProjection(0.02f, 200.0f, window->getAspectRatio());
		pCamera->setPosition(sa::vec3<float>(0, 0, 20));
		pRenderer->cameraToGPU();

		// todo: make use of time_alpha?
		menuRoot->visualise(pGraphics);
		game->draw(pGraphics);

		window->swap_buffers();
	}

	game->postTick();
	window->pollEvents();
	Timer::sleep(1);

	return !window->shouldClose();
}