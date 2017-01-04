#pragma once

#include "Army.hpp"
#include "Field.hpp"

#ifdef BATTLESIM_DLL
#define BATTLESIM_API __declspec(dllimport)
#else
#define BATTLESIM_API __declspec(dllexport)
#endif

namespace bs
{
	class BattleSim
	{
	public:
		BATTLESIM_API BattleSim();

		void BATTLESIM_API TestSetup();

		void BATTLESIM_API Simulate(size_t milliSeconds);

	private:
		void SetArmyCount(size_t size);
		void AddUnitToArmy(Unit& unit, size_t armyIndex);

		Vector<Army> myArmies;
		Field myField;

		double myTotalTime = 0; // Total time simulated
		double myTimeAccu = 0; // Time requested to be simulated, but could not fit in time step
	};
}