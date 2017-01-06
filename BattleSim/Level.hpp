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
		static const U32 UniformGridBits = 16;
		static const U32 UniformGridLists = 1 << UniformGridBits;
		static const Real UniformGridScale;

		Level()
		{
		}

		void RemoveUnit(Unit& unit);
		void AddUnit(Unit& unit);
		void FindCollisions(const Unit& unit, const Vec& newPos, std::vector<Unit::Id>& collisions) const;

		bool IsGridMove(const Unit& unit, const Vec& newPos) const
		{			
			BoundingBox bb;
			FindBoundingBox(newPos, unit.radius, bb);
			return bb != unit.bb;
		}

		// Map position to uniform grid
		U32 POS(Real x) const
		{
			return static_cast<int>(x / UniformGridScale);
		}

	private:
		U32 HashGrid(U32 x, U32 y) const
		{
			return (y * 4096 + x) & (UniformGridLists - 1);
		}

		void FindBoundingBox(const Vec& pos, const Real& radius, BoundingBox& bb) const
		{
			bb.left = POS(pos.x - radius);
			bb.right = POS(pos.x + radius);
			bb.top = POS(pos.y - radius);
			bb.bottom = POS(pos.y + radius);
		}

		typedef Vector<Unit::Id> UnitList;
		Array<UnitList, UniformGridLists> myGrid;
	};
}

inline void bs::Level::FindCollisions(
	const Unit& unit, const Vec& newPos, std::vector<Unit::Id>& collisions) const
{
	BoundingBox start;
	FindBoundingBox(unit.pos, unit.radius, start);

	BoundingBox end;
	FindBoundingBox(newPos, unit.radius, end);

	BoundingBox bb;
	bb.left = start.left < end.left ? start.left : end.left;
	bb.right = start.right > end.right ? start.right : end.right;
	bb.top = start.top < end.top ? start.top : end.top;
	bb.bottom = start.bottom > end.bottom ? start.bottom : end.bottom;

	for (U32 y = bb.top; y <= bb.bottom; y++)
	{
		for (U32 x = bb.left; x <= bb.right; x++)
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
