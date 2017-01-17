#include "stdafx.h"
#include "BattleSimAsync.h"

bs::BattleSimAsync::BattleSimAsync(Battle& battle) : BattleSim(battle, Field::StreamingMode::Enabled)
{

}

bs::BattleSimAsync::~BattleSimAsync()
{
	if (myActiveFlag)
	{
		std::atomic_thread_fence(std::memory_order_acquire);
		myActiveFlag = false;
		Simulate(0);
		myThread.join();
	}
}

void bs::BattleSimAsync::Resolve(size_t milliseconds)
{
	myField.InitialUpdate(myBattle);
	Run();
	Simulate(milliseconds);
}


void bs::BattleSimAsync::Simulate(size_t milliSeconds)
{
	myMutex.lock();
	myTimeToSimulate += milliSeconds;
	myMutex.unlock();
	myCV.notify_all();
}

void bs::BattleSimAsync::Run()
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

void bs::BattleSimAsync::Cancel()
{
	myActiveFlag = false;
	Simulate(0); // Just in case
}