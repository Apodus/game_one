
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
		bool locallyOwned = province->m_owner == m_localPlayer;
		bool unitsPresent = std::any_of(province->units.begin(), province->units.end(), [this](const Troop& troop) {
			return troop.owner == m_localPlayer;
		});
		bool commandersPresent = std::any_of(province->commanders.begin(), province->commanders.end(), [this](const BattleCommander& commander) {
			return commander.owner == m_localPlayer;
		});

		// only show menus if province is owned by player.
		if (locallyOwned) {
			activeProvince = province;
			recruitmentTab = std::make_shared<ProvinceRecruitmentTab>(
				this,
				*province,
				game,
				resourceTab->faction()
			);
			provinceMenu.emplace_back(recruitmentTab);
		}
		
		if (commandersPresent) {
			// if province is hostile but we have people there (spies?),
			// then maybe show some additional details of province.
			
			activeProvince = province;
			commandersTab = std::make_shared<ProvinceCommandersTab>(this, *province, m_localPlayer);
			provinceMenu.emplace_back(commandersTab);
		}
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