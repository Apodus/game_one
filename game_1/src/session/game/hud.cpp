
#include "session/game/hud.hpp"
#include "session/game/game.hpp"

void Hud::childComponentCall(const std::string& who, const std::string& what, int value)
{
	if (who == "CommandersTab")
	{
		if (what == "UpdatePossibleMovement")
		{
			if (activeProvince)
			{
				// refresh movement possibilities visualization.
				std::vector<uint32_t> selectedCommanders = getSelectedCommanders();
				
				if (selectedCommanders.empty())
				{
					movementPossibilities.clear();
					return;
				}

				uint32_t fastMovement = ~0;
				int32_t movement = 1000;
				for (uint32_t commanderId : selectedCommanders)
				{
					for (auto& commander : activeProvince->commanders)
					{
						if (commander.id == commanderId)
						{
							fastMovement &= commander.reference->terrainFast;
							int mov = commander.strategyMovement();
							if (mov < movement)
								movement = mov;
							break;
						}
					}
				}
				
				movementPossibilities = game.graph.movementInTurns(m_localPlayer, movement, fastMovement, activeProvince->m_index);
			}
		}
	}

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


std::vector<uint32_t> Hud::getSelectedCommanders() const
{
	std::vector<uint32_t> selectedCommanders;
	if (activeProvince && commandersTab)
	{
		for (auto& icon : commandersTab->icons)
			if (icon->selected)
				selectedCommanders.emplace_back(icon->m_commander.id);
	}
	return std::move(selectedCommanders);
}

bool Hud::isCommanderSelected(uint32_t id) const
{
	if (activeProvince && commandersTab)
	{
		bool found = false;
		for (auto& icon : commandersTab->icons)
			found |= (icon->m_commander.id == id) && icon->selected;
		return found;
	}
	return false;
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
			commandersTab = std::make_shared<ProvinceCommandersTab>(this, game, *province, m_localPlayer);
			provinceMenu.emplace_back(commandersTab);
		}
	}
}

void Hud::orderToProvince(ProvinceGraph::Province* province)
{
	if (activeProvince && province)
	{
		if (movementPossibilities.empty())
			return;
		if (movementPossibilities[province->m_index] > 1)
			return;

		std::vector<uint32_t> commanderIds = getSelectedCommanders();
		std::shared_ptr<Turn> turn = game.getPlayer(m_localPlayer).turn;

		turn->addMovementOrder(commanderIds, activeProvince->m_index, province->m_index);

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
}

void Hud::unselectProvince()
{
	commandersTab.reset();
	recruitmentTab.reset();
	for (auto& entry : provinceMenu)
		entry->hide();
	activeProvince = nullptr;

	movementPossibilities.clear();
}