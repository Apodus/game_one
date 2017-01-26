#pragma once

#include "menu/MenuComponent.hpp"
#include "menu/MenuFrame.hpp"
#include "session/game/worldmap/ProvinceGraph.hpp"

#include <vector>
#include <string>

class TroopsTab;
class Game;

class ProvinceCommandersTab : public sa::MenuComponent
{
public:
	struct CommanderIcon : public sa::MenuComponent
	{
		CommanderIcon(sa::MenuComponent* parent, BattleCommander& commander);
		virtual void childComponentCall(const std::string& who, const std::string& what, int = 0) {}
		virtual void draw(std::shared_ptr<sa::Graphics> graphics) const override;
		virtual void update(float dt) override;

		BattleCommander& m_commander;

		float rot = 0; // in case want to make some rotation effect lol
		float targetAlpha = 1.0f;
		
		std::string m_actionName;
		std::string m_icon;
		sa::vec4<float> m_color;

		size_t id;
		std::string name;
		std::string className;

		bool selected = false;
	};

	ProvinceCommandersTab(sa::MenuComponent* parent, Game& game, ProvinceGraph::Province& province, size_t localPlayer);
	virtual ~ProvinceCommandersTab();
	
	void emptyOrder();
	void orderToProvince(ProvinceGraph::Province* province);
	
	virtual void childComponentCall(const std::string& who, const std::string& what, int = 0);
	virtual void draw(std::shared_ptr<sa::Graphics> graphics) const override;
	virtual void hide() override;
	virtual void update(float dt) override;

	std::vector<uint32_t> selectedCommanders() const;
	bool isSelected(uint32_t id) const;
	void select(uint32_t id);
	void unselectAll();

	sa::MenuFrameBackground bg;
	std::vector<std::shared_ptr<CommanderIcon>> icons;
	std::shared_ptr<TroopsTab> troopManagement;
};