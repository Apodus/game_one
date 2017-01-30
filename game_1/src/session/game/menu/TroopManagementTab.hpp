
#pragma once

#include "menu/MenuComponent.hpp"
#include "menu/MenuFrame.hpp"
#include "session/game/Faction.hpp"
#include "session/game/worldmap/ProvinceGraph.hpp"

class Game;
class ProvinceCommandersTab;

struct CommanderOrSquad
{
	uint32_t commanderIndex = 0;
	int32_t squadIndex = 0;
	float distanceSquared = 10000;
};

class TroopsTab : public sa::MenuComponent
{
	sa::vec3<float> startPosToUI(sa::vec2<int> posInt) const;
	sa::vec2<int> uiPosToStart(sa::vec3<float> posUI) const;

	inline sa::vec3<float> uiPosOf(sa::vec2<int> posInt) const { return startPosToUI(posInt) + battleAreaPos(); }
	inline sa::vec2<int> wrapToBattleArea(sa::vec2<int> v)
	{
		if (v.x < -100) v.x = -100;
		if (v.x > +100) v.x = 100;
		if (v.y < -100) v.y = -100;
		if (v.y > +100) v.y = +100;
		return v;
	}

	inline sa::vec3<float> battleAreaPos() const { return getPosition() + getScale() * sa::vec3<float>(0, 0.45f * 0.5f, 0); }
	inline sa::vec3<float> battleAreaScale() const { return getScale() * sa::vec3<float>(0.975f, 0.50f, 1) * 0.5f; }


public:
	TroopsTab(sa::MenuComponent* parent, ProvinceGraph::Province& province, ProvinceCommandersTab& commandersTab);

	virtual void childComponentCall(const std::string& who, const std::string& what, int = 0)
	{}
	virtual void update(float dt) override;
	virtual void draw(std::shared_ptr<sa::Graphics> graphics) const override;
	virtual void hide() override;
	bool troopTabEnabled() const;
	void toggle();

	Faction& faction()
	{
		return m_faction;
	}

	CommanderOrSquad findNearest(float x, float y) const;

	Faction m_faction;
	sa::MenuFrameBackground bg;

	ProvinceCommandersTab& m_commandersTab;
	ProvinceGraph::Province& m_province;

	bool m_dragActive = false;
	sa::vec3<float> mousePos;

	std::vector<sa::vec2<int>> commanderOffsets;

	float m_alpha = 0;
	float m_targetAlpha = 0;
};