#include "stdafx.h"
#include "Level.hpp"

static const bool field_scan_trace = true;

void bs::Level::RemoveUnit(Unit& /*unit*/) 
{
#if 0
	int x;
	int y;
	for (x = POS(player->LocationGet()->x - 0.5f);
		x <= POS(player->LocationGet()->x + 0.5f); x++)
	{
		for (y = POS(player->LocationGet()->y - 0.5f);
			y <= POS(player->LocationGet()->y + 0.5f); y++)
		{
			// printf("player %i remove at %i %i\n", player->id, x, y );
			block_remove_player(&mBlocks[x][y], player);
		}
	}
#ifdef _DEBUG
	DebugPlayerNotLeft(player);
#endif /* _DEBUG */
#endif
}

void bs::Level::AddUnit(Unit& /*unit*/)
{
#if 0
	int x;
	int y;
	for (x = POS(player->LocationGet()->x - 0.5f);
		x <= POS(player->LocationGet()->x + 0.5f); x++)
	{
		for (y = POS(player->LocationGet()->y - 0.5f);
			y <= POS(player->LocationGet()->y + 0.5f); y++)
		{
			block_add_player(&mBlocks[x][y], player);
		}
	}
#endif
}
