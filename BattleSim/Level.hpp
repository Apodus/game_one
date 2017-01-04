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
		static const U32 FacesPerLine = 1024;
		static const Real GridScale;
		
		Level()
		{
		}

		Real Size() const { return GridScale * Real(FacesPerLine, 1); }

		void RemoveUnit(Unit& unit);
		void AddUnit(Unit& unit);

		// Map position to uniform grid
		U32 POS(Real x) const
		{
			return static_cast<int>(x);
		}

		// Scan from x1,y1 to x2,y2 and calls function when hit
		template<typename Callback>
		inline bool InternalScan(const Vec& start, const Vec& end, void* data, Callback&& callback);

	private:
		typedef Vector<Unit::Id> UnitList;
		Array<Array<std::unique_ptr<UnitList>, FacesPerLine>, FacesPerLine> myGrid;
	};
}

template<typename Callback>
inline bool bs::Level::InternalScan(
	const Vec& start, const Vec& end, void* data, Callback&& callback)
{
	Real x1 = start.x;
	Real y1 = start.y;
	Real x2 = end.x;
	Real y2 = end.y;

	// Bresenham's line algorithm
	// https://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm#C.2B.2B
	const bool steep = ((y2 - y1).abs() > (x2 - x1).abs());
	if (steep)
	{
		std::swap(x1, y1);
		std::swap(x2, y2);
	}

	if (x1 > x2)
	{
		std::swap(x1, x2);
		std::swap(y1, y2);
	}

	const Real dx = x2 - x1;
	const Real dy = (y2 - y1).abs();

	Real error = dx / Real(2,1);
	const int ystep = (y1 < y2) ? 1 : -1;
	int y = (int)y1;

	const int maxX = (int)x2;
	
	for (int x = (int)x1; x<=maxX; x++)
	{
		if (steep)
		{
			callback(y, x, data);
		}
		else
		{
			callback(x, y, data);
		}

		error -= dy;
		if (error < Real(0,1))
		{
			y += ystep;
			error += dx;
		}
	}

	return true;
}

