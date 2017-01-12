// BattleSim.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "BattleSim.hpp"

#include <chrono>

bs::BattleSim::BattleSim()
{

}

void bs::BattleSim::SetArmyCount(size_t size)
{
	myArmies.resize(size);
}

void bs::BattleSim::AddUnitToArmy(Unit& unit, Unit::Detail& detail, size_t armyIndex)
{
	auto id = myField.Add(unit, detail);
	myArmies[armyIndex].units.emplace_back(id);
}

void bs::BattleSim::TestSetup()
{
	SetArmyCount(2);
	for (int64_t y = 0; y < 10; y++)
	{
		for (int64_t i = 0; i < 100; i++)
		{
			{
				Unit u;
				Unit::Detail d;
				u.team = 0;
				if (i % 10 == 0 && y == 0)
				{
					u.radius = Real(1, 1);
					u.hitpoints = 10;
				}
				else
				{
					u.radius = Real(1, 2);
					u.hitpoints = 5;
				}
				u.pos.set(Real(50 + i), Real(50 + y), Real(0));
				d.moveTarget.set(Real(50 + i + (rand() % 4)), Real(75), Real(0));
				AddUnitToArmy(u, d, 0);
			}

			{
				Unit u;
				Unit::Detail d;
				u.team = 1;
				if (i % 10 == 0 && y == 9)
				{
					u.radius = Real(1, 1);
					u.hitpoints = 10;
				}
				else
				{
					u.radius = Real(1, 2);
					u.hitpoints = 5;
				}
				u.pos.set(Real(50 + i), Real(100 + y), Real(0));
				d.moveTarget.set(Real(50 + i + (rand() % 4)), Real(75), Real(0));
				AddUnitToArmy(u, d, 1);
			}
		}
	}	
}

void bs::BattleSim::Simulate(size_t milliSeconds)
{
	double endTime = myTimeAccu + myTotalTime + static_cast<double>(milliSeconds) / 1000.0;
	double step = static_cast<double>(Field::TimePerUpdate.getRawValue()) / Real::s_fpOne;

	while (myTotalTime + step < endTime)
	{
		auto start = std::chrono::high_resolution_clock::now();
		myField.Update();
		myTotalTime += step;
		auto stop = std::chrono::high_resolution_clock::now();
		using ms = std::chrono::duration<float, std::milli>;
		auto deltaTime = std::chrono::duration_cast<ms>(stop - start).count();
		LOG("Total time simulated = %f;in accu=%f; step=%f; cpu=%f",
			myTotalTime, myTimeAccu, step, static_cast<double>(deltaTime) / 1000.0);
	};
	myTimeAccu = endTime - myTotalTime;
}
