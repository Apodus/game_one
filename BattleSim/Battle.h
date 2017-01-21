#pragma once

#include "Types.hpp"
#include "Unit.hpp"
#include "Vector.hpp"

#include <unordered_map>

namespace bs
{
	struct Battle
	{
	public:
		void Add(const Unit& unit)
		{
			auto id = static_cast<Unit::Id>(myUnits.size());
			myUnits.emplace_back(unit);			
			myUnits.back().id = id;
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

		void Set(Vector<Unit>&& units) 		
		{ 
			myUnits = std::move(units);
		}

	private:
		Vector<Unit> myUnits;
	};
}