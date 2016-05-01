
#pragma once

#include <memory>

#include "graphics/shader/shaders.hpp"
#include "graphics/renderer/meshrenderer.hpp"
#include "graphics/renderer/textrenderer.hpp"
#include "graphics/text/fontdata/consolamono.hpp"
#include "graphics/text/fontdata/lenka.hpp"
#include "graphics/text/font.hpp"

namespace sa {

class Graphics {
public:
	std::shared_ptr<sa::Shaders> m_pShaders;
	std::shared_ptr<sa::MeshRenderer> m_pRenderer;
	std::shared_ptr<sa::TextRenderer> m_pTextRenderer;

	Font m_fontLenka;
	Font m_fontConsola;

	Graphics(std::shared_ptr<sa::Shaders> pShaders, std::shared_ptr<sa::MeshRenderer> pRenderer, std::shared_ptr<sa::TextRenderer> pTextRenderer) : m_fontLenka(Fonts::setFontLenka()), m_fontConsola(Fonts::setFontConsolaMono()) {
		m_pShaders = pShaders;
		m_pRenderer = pRenderer;
		m_pTextRenderer = pTextRenderer;
	}
};

}
