
#pragma once

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

#include "menu/menuroot.hpp"
#include "math/2d/polygonModifier.hpp"

#include "messagesystem/TypedMessage.hpp"
#include "graphics/window/messages.hpp"

#include "session/game/game.hpp"

#include <memory>

namespace Session {
	namespace Application {
		class Session {
			struct CameraAspectRatioUpdater : public TypedMessage<AspectRatioChanged> {
				CameraAspectRatioUpdater(std::shared_ptr<sa::Camera> pCamera) : m_pCamera(pCamera) {}
				std::shared_ptr<sa::Camera> m_pCamera;

				virtual void handle(const AspectRatioChanged& msg) override {
					m_pCamera->setProjection(0.02f, 200.f, msg.value);
				}
			};

		public:
			Session();
			bool tick();

		private:
			std::shared_ptr<sa::Camera> pCamera;
			std::shared_ptr<sa::Shaders> pShaders;
			std::shared_ptr<sa::TextRenderer> pTextRenderer;
			std::shared_ptr<sa::MeshRenderer> pRenderer;
			std::shared_ptr<sa::Graphics> pGraphics;
			std::shared_ptr<sa::Window> window;

			FPS_Manager fps_logic;
			FPS_Manager fps_graphic;

			std::unique_ptr<Game> game;

			std::shared_ptr<sa::UserIO> userIO;
			std::shared_ptr<sa::MenuRoot> menuRoot;
		};
	}
}