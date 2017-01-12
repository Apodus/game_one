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

		void AddUnit(const Unit& unit, Unit::Detail& detail);

		void RemoveUnit(Unit::Detail& detail);
		
		bool IsGridMove(const Unit& unit, const Unit::Detail& detail, const Vec& newPos) const
		{			
			BoundingBox bb;
			FindBoundingBox(newPos, unit.radius, bb);
			return bb != detail.bb;
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

inline void bs::Level::RemoveUnit(Unit::Detail& detail)
{
	for (U32 y = detail.bb.top; y <= detail.bb.bottom; y++)
	{
		for (U32 x = detail.bb.left; x <= detail.bb.right; x++)
		{
			U32 hash = HashGrid(x, y);
			ASSERT(!myGrid[hash].empty(), "No units found at [%u, %u]", x, y);
			auto& list = myGrid[hash];
			for (size_t i = 0; i < list.size();)
			{
				if (list.at(i) == detail.id)
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

inline void bs::Level::AddUnit(const Unit& unit, Unit::Detail& detail)
{
	FindBoundingBox(unit.pos, unit.radius, detail.bb);
	for (U32 y = detail.bb.top; y <= detail.bb.bottom; y++)
	{
		for (U32 x = detail.bb.left; x <= detail.bb.right; x++)
		{
			U32 hash = HashGrid(x, y);
			myGrid[hash].emplace_back(detail.id);
			// LOG("UNIT %d added at[%u, %u](%lu)", unit.id, x, y, hash);
			ASSERT(myGrid[hash].size() < 64, "Lots of units in same block. Hash collision?");
		}
	}
}

