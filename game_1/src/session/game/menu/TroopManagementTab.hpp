
#pragma once

#include "menu/MenuComponent.hpp"
#include "menu/MenuFrame.hpp"
#include "session/game/Faction.hpp"

class Game;
class ProvinceCommandersTab;

class TroopsTab : public sa::MenuComponent
{
public:
	TroopsTab(sa::MenuComponent* parent, Game& game, ProvinceCommandersTab& commandersTab);

	virtual void childComponentCall(const std::string& who, const std::string& what, int = 0)
	{}
	virtual void update(float dt) override;
	virtual void draw(std::shared_ptr<sa::Graphics> graphics) const override;

	bool troopTabEnabled() const;
	void toggle();

	Faction& faction()
	{
		return m_faction;
	}

	Faction m_faction;
	sa::MenuFrameBackground bg;

	ProvinceCommandersTab& commandersTab;
	Game& m_game;

	float alpha;
	float targetAlpha;
};