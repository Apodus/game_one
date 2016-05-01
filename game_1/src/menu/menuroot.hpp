
#pragma once

#include "script/Scripter.hpp"
#include "graphics/window/window.hpp"
#include "input/userio.hpp"
#include "util/vec3.hpp"

#include "menu/mainmenu.hpp"
#include "menu/optionsmenu.hpp"
#include "menu/MenuLogicalFrame.hpp"
#include "menu/gameline.hpp"

#include <memory>

namespace sa {
class MenuRoot : public sa::MenuLogicalFrame {
	Scripter scripter;

	sa::vec4<float> getNextColor() {
		static int count = 0;
		std::vector<sa::vec4<float>> colors;
		colors.push_back(Color::WHITE);
		colors.push_back(Color::GREEN);
		colors.push_back(Color::RED);
		colors.push_back(Color::BLUE);
		colors.push_back(Color::ORANGE);
		colors.push_back(Color::MAGENTA);
		return colors[++count % colors.size()];
	}

public:
	MenuRoot(std::shared_ptr<sa::Window> window, std::shared_ptr<sa::UserIO> userio, std::shared_ptr<sa::Graphics> pGraphics, const std::string& name, const sa::vec3<float>& position, const sa::vec3<float>& scale)
        : sa::MenuLogicalFrame(window, userio, name, position, scale),
		  scripter("../scripts/")
    {
	}

	virtual void childComponentCall(const std::string& who, const std::string& what, int value = 0) override {

	}

	virtual void update(float dt) override {
	
	}

private:
	std::shared_ptr<sa::Graphics> m_pGraphics; // needed for fonts. fonts are required to check how big lines will be.
	
	float bottom;
	float maxY;
};
}