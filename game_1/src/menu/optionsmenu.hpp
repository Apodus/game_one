
#pragma once

#include "menu/MenuComponent.hpp"
#include "menu/MenuButton.hpp"
#include "menu/MenuSlider.hpp"
#include "graphics/color.hpp"
#include "input/userio.hpp"

#include "localization/localization.hpp"

#include <memory>

namespace sa {

class OptionsMenu : public MenuComponent {
	/*
	void addMenuButton(const std::string& name, const std::string& texture, const std::string& text) {
		addChild(
			std::shared_ptr<MenuComponent>(new MenuButton(
				this,
				name,
				vec3<float>(-0.50f, 0.4f - m_children.size() * 0.15f, 0),
				vec3<float>(0.75f, 0.10f, 1),
				texture,
				text,
				Color::WHITE,
				Color::GOLDEN
			))
		);
	}

	void addMenuSlider(const std::string& name, const std::string& texture) {
		addChild(
			std::shared_ptr<MenuComponent>(new MenuSlideBarVertical(
				this,
				name,
				vec3<float>(-0.50f, 0.4f - m_children.size() * 0.15f, 0),
				vec3<float>(0.75f, 0.10f, 1),
				texture,
				texture,
				[](float){}
			))
		);
	}
*/
public:
	OptionsMenu(MenuComponent* parent, const std::string& name, const vec3<float>& position, const vec3<float>& scale) : MenuComponent(parent, name, position, scale) {
		// addMenuSlider("TestSlider", "ButtonBase");
		// addMenuButton("Back", "ButtonBase", sa::localization::get("Back"));
	}

private:
	virtual void childComponentCall(const std::string& who, const std::string& what, int = 0) override {
		if(who == "Back") {
			if(what == "click") {
				hide();
				callParent("showMenu", 0);
			}
		}
	}

	virtual void draw(std::shared_ptr<Graphics>) const override {
	}

	virtual void update(float) override {
	}
};

}
