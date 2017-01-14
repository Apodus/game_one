// BattleSim.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "BattleSim.hpp"

#include <chrono>
#include <atomic>

bs::BattleSim::BattleSim() :
	myActiveFlag(true)
{
	Run();
}

bs::BattleSim::~BattleSim()
{
	std::atomic_thread_fence(std::memory_order_acquire);
	myActiveFlag = false;
	myThread.join();
}

void bs::BattleSim::SetArmyCount(size_t size)
{
	myArmies.resize(size);
}

void bs::BattleSim::AddUnitToArmy(Unit& unit, size_t armyIndex)
{
	// Unit unit;
	// Unit::Detail detail;
	unit.team = static_cast<uint8_t>(armyIndex);

	auto id = myField.Add(unit);
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
				if (i % 10 == 0 && y == 0)
				{
					u.radius = Real(10, 10);
					u.hitpoints = 10;
				}
				else
				{
					u.radius = Real(4, 10);
					u.hitpoints = 5;
				}
				u.pos.set(Real(50 + i), Real(50 + y), Real(0));
				u.moveTarget.set(Real(50 + i + (rand() % 4)), Real(75), Real(0));
				AddUnitToArmy(u, 0);
			}

			{
				Unit u;
				if (i % 10 == 0 && y == 9)
				{
					u.radius = Real(10, 10);
					u.hitpoints = 10;
				}
				else
				{
					u.radius = Real(4, 10);
					u.hitpoints = 5;
				}
				u.pos.set(Real(50 + i), Real(100 + y), Real(0));
				u.moveTarget.set(Real(50 + i + (rand() % 4)), Real(75), Real(0));
				AddUnitToArmy(u, 1);
			}
		}
	}	
}

void bs::BattleSim::Simulate(size_t milliSeconds)
{
	myMutex.lock();
	myTimeToSimulate += milliSeconds;
	myMutex.unlock();
}

void bs::BattleSim::Run()
{
	myThread = std::thread([this]()
	{
		while (myActiveFlag)
		{			
			myMutex.lock();
			double timeToSimulate = static_cast<double>(myTimeToSimulate) / 1000.0;
			myTimeToSimulate = 0;
			myMutex.unlock();
			double endTime = myTimeAccu + myTotalTime + timeToSimulate;
			double step = static_cast<double>(Field::TimePerUpdate.getRawValue()) / Real::s_fpOne;
			while (myTotalTime + step < endTime && myActiveFlag)
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
	});
}
