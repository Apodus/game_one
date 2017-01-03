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
	{
		Unit u;
		u.pos.set(Real(50), Real(50), Real(0));
		u.moveTarget.set(Real(100), Real(100), Real(0));
		AddUnitToArmy(u, 0);
	}

	Army b;
	{
		Unit u;
		u.pos.set(Real(100), Real(100), Real(0));
		u.moveTarget.set(Real(50), Real(50), Real(0));
		AddUnitToArmy(u, 1);
	}
}

void bs::BattleSim::Tick()
{
	// TODO: This looks like just a wrapper class, remove later
	for (size_t i = 0; i < 1000; i++)
	{
		myField.Tick();
	}
}