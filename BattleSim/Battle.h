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
		void Add(const Unit& unit, size_t guid)
		{
			auto id = static_cast<Unit::Id>(myUnits.size());
			myUnits.emplace_back(unit);			
			myUnits.back().id = id;
			ASSERT(myGuidToUnitId.find(guid) == myGuidToUnitId.end(), "Duplicate GUID 0x%zx", guid);
			myGuidToUnitId[guid] = id;
		}

		const Unit& Get(size_t guid) const 
		{ 
			return myUnits[myGuidToUnitId.find(guid)->second]; 
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
		std::unordered_map<size_t, Unit::Id> myGuidToUnitId;

	};
}