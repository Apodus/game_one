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
		static const U32 UniformGridBits = 20;
		static const U32 UniformGridLists = 1 << UniformGridBits;
		static const Real UniformGridScale;
		
		Level()
		{
		}

		void RemoveUnit(Unit& unit);
		void AddUnit(const Unit& unit);
		void FindCollisions(const Unit& unit, const Vec& newPos, std::vector<Unit::Id>& collisions) const;
		
		bool IsGridMove(const Unit& unit, const Vec& newPos) const
		{
			U32 x = POS(unit.pos.x);
			U32 xb = POS(newPos.x);
			if (x != xb)
			{
				return true;
			}
			U32 y = POS(unit.pos.y);
			U32 yb = POS(newPos.y);
			return y != yb;
		}

		// Map position to uniform grid
		U32 POS(Real x) const
		{
			return static_cast<int>(x / UniformGridScale);
		}
		// Scan from x1,y1 to x2,y2 and calls function when hit
		template<typename Callback>
		inline bool InternalScan(const Vec& start, const Vec& end, void* data, Callback&& callback);

	private:
		U32 HashGrid(U32 x, U32 y) const
		{
			return (y * 4096 + x) & (UniformGridLists-1);
		}

		struct BoundingBox
		{
			U32 left;
			U32 top;
			U32 right;
			U32 bottom;
		};

		void FindBoundingBox(const Unit& unit, BoundingBox& bb);
		
		typedef Vector<Unit::Id> UnitList;
		Array<UnitList, UniformGridLists> myGrid;
	};
}
