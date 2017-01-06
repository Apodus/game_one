#include "stdafx.h"
#include "Level.hpp"

const bs::Real bs::Level::UniformGridScale(2, 1);

void bs::Level::RemoveUnit(Unit& unit) 
{
	BoundingBox bb;
	FindBoundingBox(unit, bb);
	for (U32 x = bb.left; x <= bb.right; x++)
	{
		for (U32 y = bb.top; y <= bb.bottom; y++)
		{
			U32 hash = HashGrid(x, y);
			ASSERT(!myGrid[hash].empty(), "No units found at [%u, %u]", x, y);
			auto& list = myGrid[hash];
			for (size_t i = 0; i < list.size();)
			{
				if (list.at(i) == unit.id)
				{
					list[i] = list.back();
					list.pop_back();
				}
				else
				{
					++i;
				}
			}
		}
	}
}

void bs::Level::AddUnit(const Unit& unit)
{
	BoundingBox bb;
	FindBoundingBox(unit, bb);
	for (U32 x = bb.left; x <= bb.right; x++)
	{
		for (U32 y = bb.top; y <= bb.bottom; y++)
		{
			U32 hash = HashGrid(x, y);
			myGrid[hash].emplace_back(unit.id);
		}
	}
}

void bs::Level::FindCollisions(const Unit& unit, const Vec& newPos, std::vector<Unit::Id>& collisions) const
{
	BoundingBox start;
	start.left = POS(unit.pos.x - unit.radius);
	start.right = POS(unit.pos.x + unit.radius);
	start.bottom = POS(unit.pos.y + unit.radius);
	start.top = POS(unit.pos.y - unit.radius);

	BoundingBox end;
	end.left = POS(newPos.x - unit.radius);
	end.right = POS(newPos.x + unit.radius);
	end.bottom = POS(newPos.y + unit.radius);
	end.top = POS(newPos.y - unit.radius);

	BoundingBox bb;
	bb.left = start.left < end.left ? start.left : end.left;
	bb.right = start.right > end.right ? start.right : end.right;
	bb.top = start.top < end.top ? start.top : end.top;
	bb.bottom = start.bottom > end.bottom ? start.bottom : end.bottom;
	
	for (U32 x = bb.left; x <= bb.right; x++)
	{
		for (U32 y = bb.top; y <= bb.bottom; y++)
		{
			U32 hash = HashGrid(x, y);
			auto& list = myGrid[hash];
			for (size_t i = 0; i < list.size(); i++)
			{
				if (list.at(i) != unit.id)
				{
					if (std::find(collisions.begin(), collisions.end(), unit.id) == collisions.end())
					{
						collisions.emplace_back(list.at(i));
					}
				}
			}
		}
	}
}

void bs::Level::FindBoundingBox(const Unit& unit, BoundingBox& bb)
{
	bb.left = POS(unit.pos.x - unit.radius);
	bb.right = POS(unit.pos.x + unit.radius);
	bb.bottom = POS(unit.pos.y + unit.radius);
	bb.top = POS(unit.pos.y - unit.radius);
}
