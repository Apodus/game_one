#include "stdafx.h"
#include "Level.hpp"

const bs::Real bs::Level::UniformGridScale(1, 1);

void bs::Level::RemoveUnit(Unit& unit) 
{
	for (U32 y = unit.bb.top; y <= unit.bb.bottom; y++)
	{
		for (U32 x = unit.bb.left; x <= unit.bb.right; x++)
		{
			U32 hash = HashGrid(x, y);
			ASSERT(!myGrid[hash].empty(), "No units found at [%u, %u]", x, y);
			auto& list = myGrid[hash];
			for (size_t i = 0; i < list.size();)
			{
				if (list.at(i) == unit.id)
				{
					// LOG("UNIT %d removed at[%u, %u](%lu)", unit.id, x, y, hash);
					list[i] = list.back();
					list.pop_back();
					break;
				}
				else
				{
					++i;
				}
			}
		}
	}
}

void bs::Level::AddUnit(Unit& unit)
{
	FindBoundingBox(unit.pos, unit.radius, unit.bb);
	for (U32 y = unit.bb.top; y <= unit.bb.bottom; y++)
	{
		for (U32 x = unit.bb.left; x <= unit.bb.right; x++)
		{
			U32 hash = HashGrid(x, y);
			myGrid[hash].emplace_back(unit.id);
			// LOG("UNIT %d added at[%u, %u](%lu)", unit.id, x, y, hash);
			ASSERT(myGrid[hash].size() < 16, "Lots of units in same block. Hash collision?");
		}
	}
}
