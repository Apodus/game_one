// BattleSim.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "BattleSim.hpp"

bs::BattleSim::BattleSim()
{

}

void bs::BattleSim::TestSetup()
{
	Army a;
	{
		Unit u;
		a.units.emplace_back(u);
	}

	Army b;
	{
		Unit u;
		b.units.emplace_back(u);
	}
	myArmies.emplace_back(a);
	myArmies.emplace_back(b);
}

void bs::BattleSim::Tick()
{

}