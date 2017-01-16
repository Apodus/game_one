// BattleSim.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "BattleSim.hpp"

#include <chrono>
#include <atomic>

bs::BattleSim::BattleSim(Battle& battle, Field::StreamingMode mode) :
	myBattle(battle), myField(mode), myActiveFlag(true)
{
}

bs::BattleSim::~BattleSim()
{
	std::atomic_thread_fence(std::memory_order_acquire);
	myActiveFlag = false;
	Simulate(0);
	myThread.join();
}

bs::Battle bs::BattleSim::Generate()
{
	Battle battle;
	battle.armies.resize(2);
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
				battle.armies[0].units.emplace_back(u);
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
				battle.armies[1].units.emplace_back(u);
			}
		}
	}
	return battle;
}

void bs::BattleSim::ResolveAsync(size_t milliseconds)
{
	myField.InitialUpdate(myBattle);
	Run();
	Simulate(milliseconds);
}

void bs::BattleSim::Resolve()
{
	myField.InitialUpdate(myBattle);
	RunUntilComplete();
	myField.FinalUpdate(myBattle);
}

void bs::BattleSim::Simulate(size_t milliSeconds)
{
	myMutex.lock();
	myTimeToSimulate += milliSeconds;
	myMutex.unlock();
	myCV.notify_all();
}

void bs::BattleSim::Run()
{
	myThread = std::thread([this]()
	{
		while (myActiveFlag)
		{
			double timeToSimulate = 0;
			{
				std::unique_lock<std::mutex> lock(myMutex);
				if (myTimeToSimulate == 0)
				{
					myCV.wait(lock);
				}
				timeToSimulate = static_cast<double>(myTimeToSimulate) / 1000.0;
				myTimeToSimulate = 0;
			}
			double endTime = myTimeAccu + myTotalTime + timeToSimulate;
			double step = static_cast<double>(Field::TimePerUpdate.getRawValue()) / Real::s_fpOne;
			while (myTotalTime + step < endTime && myActiveFlag)
			{
				auto start = std::chrono::high_resolution_clock::now();
				myActiveFlag = myActiveFlag & myField.Update();
				myTotalTime += step;
				auto stop = std::chrono::high_resolution_clock::now();
				using ms = std::chrono::duration<float, std::milli>;
#if 0 // Profiling log
				auto deltaTime = std::chrono::duration_cast<ms>(stop - start).count();
				LOG("Total time simulated = %f;in accu=%f; step=%f; cpu=%f",
					myTotalTime, myTimeAccu, step, static_cast<double>(deltaTime) / 1000.0);
#endif
			};
			myTimeAccu = endTime - myTotalTime;
		}
	});
	myField.FinalUpdate(myBattle);
}

void bs::BattleSim::RunUntilComplete()
{
	ASSERT(!myField.IsStreaming(), "Do not use streaming when doing synchronous resolve!");
	while (myActiveFlag)
	{
		myActiveFlag = myActiveFlag & myField.Update();
	}
}

double bs::BattleSim::GetTimePerUpdate() const
{
	return myField.TimePerUpdate.toDouble();
}

BATTLESIM_API void bs::BattleSim::Cancel()
{
	myActiveFlag = false;
	Simulate(0); // Just in case
}