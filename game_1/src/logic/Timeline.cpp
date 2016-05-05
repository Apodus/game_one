#include "Timeline.h"

#include <assert.h>
#include "World.h"

static const uint32_t SimulationStepMs = 10;

Timeline::Timeline(World& world)
	:
	myWorld(world)
{
	Record gameBeginEvent;
	gameBeginEvent.time = 0;
	gameBeginEvent.events.push_back(new GameEvent());
	myRecords.push_back(gameBeginEvent);
}

void Timeline::AddEvent(uint64_t time, WorldEvent* nextEvent)
{
	assert(nextEvent != nullptr);
	auto pos = myRecordPos;
	
	Record nextRecord;
	nextRecord.time = time;

	while (myRecords[pos].time != time)
	{
		if (myRecords[pos].time < time)
		{
			pos++;
			if (myRecords.size() <= pos)
			{
				myRecords.emplace_back(nextRecord);
				break;
			}
			else if (myRecords[pos].time > time)
			{
				myRecords.insert(myRecords.begin() + pos, nextRecord);
				break;
			}
		}
		else
		{
			assert(pos > 0);
			pos--;
			if (myRecords[pos].time < time)
			{
				myRecords.insert(myRecords.begin() + pos + 1, nextRecord);
				break;
			}
		}
	}
	myRecords[pos].events.emplace_back(nextEvent);
}

void Timeline::Run(uint64_t time)
{
	if (myCurrentTime > time)
	{
		if (myRecords[myRecordPos].time == myCurrentTime)
		{
			CloseRecords();
		}
		while (myRecords[myRecordPos-1].time < time)
		{
			myRecordPos--;
			myCurrentTime = myRecords[myRecordPos].time;
			CloseRecords();
		} 
		mySimulationAccumulator = myCurrentTime % SimulationStepMs;
		myCurrentTime = time;
	}
	else
	{				
		while (myRecordPos + 1 < myRecords.size() && myRecords[myRecordPos+1].time < time)
		{
			myRecordPos++;
			Simulate(myRecords[myRecordPos].time);
			OpenRecords();
		}
	}
	Simulate(time);
}

void Timeline::Simulate(uint64_t ttime)
{
	assert(myCurrentTime <= ttime); // Can simulate forward only
	mySimulationAccumulator += static_cast<uint32_t>(ttime - myCurrentTime);

	while (mySimulationAccumulator >= SimulationStepMs)
	{
		mySimulationAccumulator -= SimulationStepMs;

		// Constant physics step needed to properly replicate simulation
		const float32 SimulationStep = static_cast<float32>(SimulationStepMs) / 1000;
		myWorld.physicsWorld.Step(SimulationStep, 8, 3); // Box2d doc recommended iteration counts
		for (auto&& obj : myWorld.objs)
		{
			obj.tick(SimulationStep);
		}

		myWorld.objs.insert(myWorld.objs.end(),
			std::make_move_iterator(myWorld.newObjs.begin()),
			std::make_move_iterator(myWorld.newObjs.end()));
		myWorld.newObjs.clear();
	}

	myCurrentTime = ttime;
}

void Timeline::OpenRecords()
{
	Record& records = myRecords[myRecordPos];
	myCurrentTime = records.time;
	for (auto iter = records.events.begin(); iter != records.events.end(); ++iter)
	{
		(*iter)->Begin(myWorld);
	}
}

void Timeline::CloseRecords()
{
	Record& records = myRecords[myRecordPos];
	for (auto iter = records.events.begin(); iter != records.events.end(); ++iter)
	{
		(*iter)->End(myWorld);
	}
}

