#pragma once

#include "util/vec3.hpp"
#include "menu/MenuComponent.hpp"
#include "menu/MenuFrame.hpp"
#include "session/game/Faction.hpp"

class Game;

struct ResourceTab : public sa::MenuComponent
{
	ResourceTab(sa::MenuComponent* parent, Game& game)
		: sa::MenuComponent(
			parent,
			"ResourceTab",
			[this, parent]() {
				return sa::vec3<float>(0, 1, 0);
			},
			sa::vec3<float>(1.0f, 0.1f, 0)
		)
		, bg(this, "bg", "Frame")
		, m_game(game)
	{
		positionAlign = TOP;
		setPositionUpdateType(true);
		bg.update(0);
	}

	virtual void childComponentCall(const std::string& who, const std::string& what, int = 0) {}
	virtual void update(float dt) override;
	virtual void draw(std::shared_ptr<sa::Graphics> graphics) const override;

	void startOfNewTurn();
	Faction& faction() { return m_faction; }

	size_t m_localPlayer = 0;
	Faction m_faction;

	sa::MenuFrameBackground bg;
	Game& m_game;
};