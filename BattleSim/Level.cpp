#include "stdafx.h"
#include "Level.hpp"

static const bool field_scan_trace = true;

void bs::Level::RemoveUnit(Unit& unit) 
{
	U32 x = POS(unit.pos.x);
	U32 y = POS(unit.pos.y);
	ASSERT(myGrid[x][y] != nullptr, "No unit list found at [%u, %u]", x, y);
	if (myGrid[x][y]->size() == 1)
	{
		myGrid[x][y] = nullptr;
	}
	else
	{
		auto& list = *myGrid[x][y];
		for (size_t i = 0; i < list.size(); i++)
		{
			if (list.at(i) == unit.id)
			{
				list[i] = list.back();
				list.pop_back();
			}
		}
	}


#if 0
	for (U32 x = POS(player->LocationGet()->x - 0.5f);
		x <= POS(player->LocationGet()->x + 0.5f); x++)
	{
		for (U32 y = POS(player->LocationGet()->y - 0.5f);
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

void bs::Level::AddUnit(Unit& unit)
{
	U32 x = POS(unit.pos.x);
	U32 y = POS(unit.pos.y);
	if (myGrid[x][y] == nullptr)
	{
		myGrid[x][y] = std::make_unique<UnitList>(4);
	}
	myGrid[x][y]->emplace_back(unit.id);
}
