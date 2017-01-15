
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
			game.showBattle();
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