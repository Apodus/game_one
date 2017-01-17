#pragma once

#include "menu/MenuComponent.hpp"
#include "menu/MenuButton.hpp"
#include "menu/MenuSlider.hpp"
#include "graphics/color.hpp"
#include "input/userio.hpp"

#include "worldmap/ProvinceGraph.hpp"

#include "localization/localization.hpp"

#include "session/game/menu/RecruitmentTab.hpp"
#include "session/game/menu/CommanderTab.hpp"
#include "session/game/menu/ResourceTab.hpp"

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
		auto button = std::make_shared<sa::MenuButton>(
			this,
			"RunBattleTest",
			sa::vec3<float>(0.99f, -0.99f, 0),
			sa::vec3<float>(0.4f, 0.10f, 0),
			"ButtonBase",
			"Battle Test",
			sa::MenuComponent::BOTTOM | sa::MenuComponent::RIGHT
		);

		auto demoButton = std::make_shared<sa::MenuButton>(
			this,
			"EndTurn",
			[button]() { return button->getExteriorPosition(sa::MenuComponent::TOP | sa::MenuComponent::RIGHT); },
			sa::vec3<float>(0.4f, 0.10f, 0),
			"ButtonBase",
			"End Turn",
			sa::MenuComponent::BOTTOM | sa::MenuComponent::RIGHT
		);

		auto demoButton2 = std::make_shared<sa::MenuButton>(
			this,
			"PositionFunctionDemo",
			[demoButton]() { return demoButton->getExteriorPosition(sa::MenuComponent::TOP | sa::MenuComponent::RIGHT); },
			sa::vec3<float>(0.4f, 0.10f, 0),
			"ButtonBase",
			"^gH^ye^bl^wl^co ^w:)",
			sa::MenuComponent::BOTTOM | sa::MenuComponent::RIGHT
		);

		resourceTab = std::make_shared<ResourceTab>(
			this,
			game
		);
		addChild(resourceTab);

		addChild(button);
		addChild(demoButton);
		addChild(demoButton2);
	}

	std::vector<uint32_t> getSelectedCommanders() const;
	bool isCommanderSelected(uint32_t id) const;

	void selectProvince(ProvinceGraph::Province* province);
	void orderToProvince(ProvinceGraph::Province* province);
	void unselectProvince();

	const ProvinceGraph::Province* getActiveProvince() const {
		return activeProvince;
	}

	void newTurn()
	{
		resourceTab->startOfNewTurn();
	}

private:

	Game& game;
	size_t m_localPlayer = 0; // TODO: select player index based on what?
	std::shared_ptr<ProvinceCommandersTab> commandersTab;
	std::shared_ptr<ProvinceRecruitmentTab> recruitmentTab;
	std::shared_ptr<ResourceTab> resourceTab;

	std::vector<std::shared_ptr<sa::MenuComponent>> provinceMenu;
	ProvinceGraph::Province* activeProvince = nullptr;

	virtual void childComponentCall(const std::string& who, const std::string& what, int = 0) override;
	virtual void draw(std::shared_ptr<sa::Graphics> graphics) const override;
	virtual void update(float dt) override;
};
