
#pragma once

#include "session/game/menu/CommanderTab.hpp"
#include "session/game/Faction.hpp"

class Game;

struct TroopsTab : public sa::MenuComponent
{
	TroopsTab(sa::MenuComponent* parent, Game& game, ProvinceCommandersTab& commandersTab)
		: sa::MenuComponent(
			parent,
			"TroopsTab",
			[this, parent]() {
				return sa::vec3<float>(0, 1, 0);
			},
			sa::vec3<float>(1.0f, 0.1f, 0)
		)
		, bg(this, "bg", "Frame")
		, commandersTab(commandersTab)
		, m_game(game)
	{
		positionAlign = TOP;
		setPositionUpdateType(true);
		bg.update(0);
	}

	virtual void childComponentCall(const std::string& who, const std::string& what, int = 0)
	{}
	virtual void update(float dt) override;
	virtual void draw(std::shared_ptr<sa::Graphics> graphics) const override;

	Faction& faction()
	{
		return m_faction;
	}

	Faction m_faction;
	sa::MenuFrameBackground bg;

	ProvinceCommandersTab& commandersTab;
	Game& m_game;
};