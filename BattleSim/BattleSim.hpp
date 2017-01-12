#pragma once

#include "Army.hpp"
#include "Field.hpp"

namespace bs
{
	class BattleSim
	{
	public:
		BATTLESIM_API BattleSim();

		BATTLESIM_API void TestSetup();

		BATTLESIM_API void Simulate(size_t milliSeconds);

		BATTLESIM_API Field& GetField() { return myField; }

	private:
		void SetArmyCount(size_t size);
		void AddUnitToArmy(Unit& unit, Unit::Detail& detail, size_t armyIndex);

		Vector<Army> myArmies;
		Field myField;

		double myTotalTime = 0; // Total time simulated
		double myTimeAccu = 0; // Time requested to be simulated, but could not fit in time step
	};
}