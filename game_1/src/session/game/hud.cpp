
#include "session/game/hud.hpp"
#include "session/game/game.hpp"

void Hud::childComponentCall(const std::string& who, const std::string& what, int)
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
}