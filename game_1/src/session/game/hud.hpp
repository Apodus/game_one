#pragma once

#include "menu/MenuComponent.hpp"
#include "menu/MenuButton.hpp"
#include "menu/MenuSlider.hpp"
#include "graphics/color.hpp"
#include "input/userio.hpp"

#include "localization/localization.hpp"

#include <memory>

class Hud : public sa::MenuComponent {
public:
	Hud(
		MenuComponent* parent,
		const std::string& name,
		const sa::vec3<float>& position,
		const sa::vec3<float>& scale
	) : MenuComponent(parent, name, position, scale)
	{
	}

private:

	std::vector<std::shared_ptr<MenuComponent>> provinceMenu;

	virtual void childComponentCall(const std::string& who, const std::string& what, int = 0) override
	{
		/*
		if (who == "Back")
		{
			if (what == "click")
			{
				hide();
				callParent("showMenu", 0);
			}
		}
		*/
	}

	virtual void draw(std::shared_ptr<sa::Graphics> graphics) const override
	{
	}

	virtual void update(float dt) override
	{
	}
};
