#include "stdafx.h"
#include "BattleSimAsync.h"

bs::BattleSimAsync::BattleSimAsync(Battle& battle) : BattleSim(battle, Field::StreamingMode::Enabled)
{

}

bs::BattleSimAsync::~BattleSimAsync()
{
	Cancel();
	myThread.join();
}

void bs::BattleSimAsync::Resolve(size_t milliseconds)
{
	Run();
	Simulate(milliseconds);
}

void bs::BattleSimAsync::Simulate(size_t milliSeconds)
{
	{
		std::unique_lock<std::mutex> lock(myMutex);
		myTimeToSimulate += milliSeconds;
	}
	myCV.notify_all();
}

void bs::BattleSimAsync::Run()
{
	ASSERT(!myActiveFlag, "Already active");
	myActiveFlag = true;
	myThread = std::thread([this]()
	{
		myField.InitialUpdate(myBattle);
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
			double mySortTimer = static_cast<double>(Field::TimePerUpdate);
			while (myTotalTime + mySortTimer < endTime && myActiveFlag)
			{
				auto start = std::chrono::high_resolution_clock::now();
				if (!myField.Update())
				{
					myActiveFlag = false;
				}
				myTotalTime += mySortTimer;
				auto stop = std::chrono::high_resolution_clock::now();
				using ms = std::chrono::duration<float, std::milli>;
#if 0 // Profiling log
				auto deltaTime = std::chrono::duration_cast<ms>(stop - start).count();
				LOG("Total time simulated = %f;in accu=%f; step=%f; cpu=%f",
					myTotalTime, myTimeAccu, mySortTimer, static_cast<double>(deltaTime) / 1000.0);
#endif
			};
			myTimeAccu = endTime - myTotalTime;
		}
		myField.FinalUpdate(myBattle);
	});	
}

void bs::BattleSimAsync::Cancel()
{
	myActiveFlag = false;
	std::atomic_thread_fence(std::memory_order_acquire);
	Simulate(0);
}