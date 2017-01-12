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
			"PositionFunctionDemo",
			[button]() { return button->getExteriorPosition(sa::MenuComponent::TOP | sa::MenuComponent::RIGHT); },
			sa::vec3<float>(0.4f, 0.10f, 0),
			"ButtonBase",
			"^gH^ye^bl^wl^co ^w:)",
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

		addChild(button);
		addChild(demoButton);
		addChild(demoButton2);
	}

	void selectProvince(ProvinceGraph::Province* province)
	{
		unselectProvince();
		if (province)
		{
			activeProvince = province;
			provinceMenu.emplace_back(std::make_shared<ProvinceCommandersTab>(this, *province));
			provinceMenu.emplace_back(std::make_shared<ProvinceRecruitmentTab>(this, *province, game));
		}
	}

	void unselectProvince()
	{
		for(auto& entry : provinceMenu)
			entry->hide();
		activeProvince = nullptr;
	}

private:

	Game& game;
	std::vector<std::shared_ptr<sa::MenuComponent>> provinceMenu;
	ProvinceGraph::Province* activeProvince = nullptr;

	virtual void childComponentCall(const std::string& who, const std::string& what, int = 0) override;

	virtual void draw(std::shared_ptr<sa::Graphics> graphics) const override
	{
		for (auto& provinceItem : provinceMenu)
		{
			provinceItem->visualise(graphics);
		}
	}

	virtual void update(float dt) override
	{
		for (auto& provinceItem : provinceMenu)
		{
			provinceItem->tick(dt);
		}

		// delete obsolete menus
		for (size_t i = 0; i < provinceMenu.size(); ++i)
		{
			auto& menuItem = provinceMenu[i];
			if (!menuItem->hasFocus() && !menuItem->inScreen())
			{
				provinceMenu[i] = provinceMenu.back();
				provinceMenu.pop_back();
			}
		}
	}
};
