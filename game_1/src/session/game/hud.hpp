#pragma once

#include "menu/MenuComponent.hpp"
#include "menu/MenuButton.hpp"
#include "menu/MenuSlider.hpp"
#include "graphics/color.hpp"
#include "input/userio.hpp"

#include "localization/localization.hpp"

#include <memory>

class Game;

class Hud : public sa::MenuComponent {
public:
	Hud(
		Game& game,
		MenuComponent* parent,
		const std::string& name,
		const sa::vec3<float>& position,
		const sa::vec3<float>& scale
	) : MenuComponent(parent, name, position, scale)
		, game(game)
	{
		addChild(
			std::make_shared<sa::MenuButton>(
				this,
				"RunBattleTest",
				sa::vec3<float>(0.8f, 0, 0),
				sa::vec3<float>(0.2f, 0.075f, 0),
				"ButtonBase",
				"Battle Test"
			)
		);
	}
private:

	Game& game;
	std::vector<std::shared_ptr<sa::MenuComponent>> provinceMenu;

	virtual void childComponentCall(const std::string& who, const std::string& what, int = 0) override;

	virtual void draw(std::shared_ptr<sa::Graphics> graphics) const override
	{
	}

	virtual void update(float dt) override
	{
	}
};
