#pragma once

#include "Unit.hpp"
#include "Vector.hpp"
#include "Array.hpp"

#include <memory>

namespace bs
{
	class Level
	{
	public:
		static const U32 UniformGridBits = 17;
		static const U32 UniformGridLists = 1 << UniformGridBits;
		static const U32 UniformGridScale;

		BATTLESIM_API Level();

		BATTLESIM_API ~Level();

		void AddUnit(Unit& unit);

		void RemoveUnit(Unit& unit);
		
		bool IsGridMove(const Unit& unit, const Vec& newPos) const
		{			
			BoundingBox bb;
			FindBoundingBox(newPos, unit.radius, bb);
			return bb != unit.bb;
		}

		// Map position to uniform grid
		U32 POS(Real x) const
		{
			return static_cast<U32>(x >> UniformGridScale);
		}

		typedef Vector<Unit::Id> UnitList;
		const UnitList& GetUnitList(U32 x, U32 y) const
		{
			U32 hash = HashGrid(x, y);
			return myGrid[hash];
		}

		void FindBoundingBox(const Vec& pos, const Real& radius, BoundingBox& bb) const
		{
			bb.left = POS(pos.x - radius);
			bb.right = POS(pos.x + radius);
			bb.top = POS(pos.y - radius);
			bb.bottom = POS(pos.y + radius);
		}

	private:
		
		U32 HashGrid(U32 x, U32 y) const
		{
			return ((y * 53836093) ^ (x * 79349663)) & (UniformGridLists - 1);
		}

		Array<UnitList, UniformGridLists> myGrid;
	};
}

inline void bs::Level::RemoveUnit(Unit& unit)
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

inline void bs::Level::AddUnit(Unit& unit)
{
	FindBoundingBox(unit.pos, unit.radius, unit.bb);
	for (U32 y = unit.bb.top; y <= unit.bb.bottom; y++)
	{
		for (U32 x = unit.bb.left; x <= unit.bb.right; x++)
		{
			U32 hash = HashGrid(x, y);
			myGrid[hash].emplace_back(unit.id);
			// LOG("UNIT %d added at[%u, %u](%lu)", unit.id, x, y, hash);
			ASSERT(myGrid[hash].size() < 64, "Lots of units in same block. Hash collision?");
		}
	}
}

