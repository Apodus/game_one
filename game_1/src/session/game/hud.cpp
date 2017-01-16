
#include "session/game/hud.hpp"
#include "session/game/game.hpp"

void Hud::childComponentCall(const std::string& who, const std::string& what, int value)
{
	if (who == "RunBattleTest")
	{
		if (what == "click")
		{
			// hide();
			// callParent("showMenu", 0);
			game.toggleBattle();
		}
	}

	if (who == "RecruitmentTab")
	{
		if (value == 1)
		{
			// user requested a recruitment action
			const TroopReference* troopRef = game.troopReference(what);
			activeProvince->addRecruitOrder(troopRef);
		}
		else if(value == 2)
		{
			const TroopReference* troopRef = game.troopReference(what);
			activeProvince->removeRecruitOrder(troopRef);
		}
		else
		{
			ASSERT(false, "value not handled %d", value);
		}
	}

	if (who == "EndTurn")
	{
		// TODO: Instead of actually processing the turn, should serialize the state
		// of my troops and send to server.
		
		this->unselectProvince();
		game.processTurn();
	}
}


void Hud::draw(std::shared_ptr<sa::Graphics> graphics) const
{
	for (auto& provinceItem : provinceMenu)
	{
		provinceItem->visualise(graphics);
	}
}

void Hud::update(float dt)
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




void Hud::selectProvince(ProvinceGraph::Province* province)
{
	unselectProvince();
	if (province)
	{
		activeProvince = province;
		commandersTab = std::make_shared<ProvinceCommandersTab>(this, *province);
		recruitmentTab = std::make_shared<ProvinceRecruitmentTab>(
			this,
			*province,
			game,
			resourceTab->faction()
		);
		provinceMenu.emplace_back(commandersTab);
		provinceMenu.emplace_back(recruitmentTab);
	}
}

void Hud::orderToProvince(ProvinceGraph::Province* province)
{
	if (province && activeProvince)
	{
		if (province == activeProvince)
		{
			commandersTab->emptyOrder();
		}
		else
		{
			commandersTab->orderToProvince(province);
		}
	}
}

void Hud::unselectProvince()
{
	commandersTab.reset();
	recruitmentTab.reset();
	for (auto& entry : provinceMenu)
		entry->hide();
	activeProvince = nullptr;
}