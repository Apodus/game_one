
#pragma once

#include "menu/MenuComponent.hpp"
#include "menu/MenuButton.hpp"
#include "graphics/color.hpp"
#include "input/userio.hpp"

#include "localization/localization.hpp"

#include <memory>

namespace sa {

class MainMenu : public MenuComponent {

	void addMenuButton(const std::string& name, const std::string& texture, const std::string& text) {
		addChild(
			std::make_shared<MenuButton>(
				this,
				name,
				vec3<float>(-0.50f, 0.4f - m_children.size() * 0.15f, 0),
				vec3<float>(0.75f, 0.10f, 1),
				texture,
				text,
				Color::WHITE,
				Color::GOLDEN
			)
		);
	}

public:
	MainMenu(MenuComponent* parent, const std::string& name, const vec3<float>& position, const vec3<float>& scale) : MenuComponent(parent, name, position, scale) {
		addMenuButton("NewCampaign", "ButtonBase", sa::localization::get("NewCampaign"));
		addMenuButton("LoadCampaign", "ButtonBase", sa::localization::get("LoadCampaign"));
		addMenuButton("Options", "ButtonBase", sa::localization::get("Options"));
		addMenuButton("Exit", "ButtonBase", sa::localization::get("Exit"));
	}

private:
	virtual void childComponentCall(const std::string& who, const std::string& what, int value = 0) override {
		if(who == "Exit") {
			if(what == "click") {
				m_pWindow->requestExit();
			}
		}
		else if(who == "NewCampaign") {
			if(what == "click") {
				hide();
				callParent("showMenu", 1);
			}
		}
		else if(who == "LoadCampaign") {
			if(what == "click") {
				hide();
				callParent("showMenu", 2);
			}
		}
		else if(who == "Options") {
			if(what == "click") {
				hide();
				callParent("showMenu", 3);
			}
		}
	}

	virtual void draw(std::shared_ptr<Graphics> renderer) const override {
	}

	virtual void update(float dt) override {
	}
};

}