#pragma once

#include "Types.h"
#include "Unit.hpp"
#include "Vector.hpp"

#include <unordered_map>

namespace bs
{
	struct Battle
	{
		friend class Field;
	public:
		void Add(const Unit& unit)
		{
			myUnits.push_back(unit);
		}

		const Unit& Get(size_t index) const
		{
			return myUnits[index];
		}

		Vector<Unit> Get()
		{
			auto tmp = std::move(myUnits);
			myUnits = Vector<Unit>();
			return tmp;
		}

		size_t NumUnits() const { return myUnits.size(); }

		size_t TotalTimeMS() const { return totalMilliseconds; }

	private:
		void Set(Vector<Unit>&& units)
		{
			myUnits = std::move(units);
		}

		size_t totalMilliseconds;
		Vector<Unit> myUnits;
	};
}