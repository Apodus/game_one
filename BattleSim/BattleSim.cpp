// BattleSim.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "BattleSim.hpp"

bs::BattleSim::BattleSim()
{

}

void bs::BattleSim::SetArmyCount(size_t size)
{
	myArmies.resize(size);
}

void bs::BattleSim::AddUnitToArmy(Unit& unit, size_t armyIndex)
{
	auto id = myField.Add(unit);
	myArmies[armyIndex].units.emplace_back(id);
	
}

void bs::BattleSim::TestSetup()
{
	SetArmyCount(2);
	for (int64_t i = 0; i < 10; i++)
	{
		{
			Unit u;
			u.pos.set(Real(50), Real(50+i), Real(0));
			u.moveTarget.set(Real(100), Real(50+i), Real(0));
			AddUnitToArmy(u, 0);
		}

		{
			Unit u;
			u.pos.set(Real(100), Real(50+i), Real(0));
			u.moveTarget.set(Real(50), Real(50+i), Real(0));
			AddUnitToArmy(u, 1);
		}
	}
}

void bs::BattleSim::Simulate(size_t milliSeconds)
{
	// TODO: This looks like just a wrapper class, remove later
	double endTime = myTimeAccu + myTotalTime + static_cast<double>(milliSeconds) / 1000.0;
	double step = static_cast<double>(Field::TimePerUpdate.getRawValue()) / Real::s_fpOne;
	while (myTotalTime + step < endTime)
	{
		myField.Update();		
		myTotalTime += step;
	};
	myTimeAccu = endTime - myTotalTime;
	LOG("Total time simulated = %f;in accu=%f; step=%f", myTotalTime, myTimeAccu, step);
}
