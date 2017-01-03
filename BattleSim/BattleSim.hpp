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

		void BATTLESIM_API Tick();

	private:
		void SetArmyCount(size_t size);
		void AddUnitToArmy(Unit& unit, size_t armyIndex);

		Vector<Army> myArmies;
		Field myField;
	};
}