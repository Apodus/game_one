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
		static const U32 Size = 1000;
		static const U32 GridSize = 500;
		
		void RemoveUnit(Unit& unit);
		void AddUnit(Unit& unit);

		// Map position to uniform grid
		U32 POS(Real /*x*/) const
		{
			return 0;//static_cast<U32>(x.getRawValue() / GridSize);
		}


		// Scan from x1,y1 to x2,y2 and calls function when hit
		template<typename Function>
		inline bool InternalScan(
			Function&& function, float x1, float y1, const float x2, const float y2, void* data);

	private:
		typedef Vector<Unit::Id> UnitList;
		Array<Array<std::unique_ptr<UnitList>, GridSize>, GridSize> myGrid;

	};
}

#include "LevelInline.hpp"
