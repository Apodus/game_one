
#include "stdafx.h"
#include "TimerSystem.h"

bs::TimerSystem::TimerSystem()
{

}

bs::TimerSystem::TimerId bs::TimerSystem::Create(const TriggerId& triggerId, U32 time)
{
	auto id = myIdPool.Reserve();
	myData[id].trigger = triggerId;
	myData[id].flags = 0;
	myQueue.push(Timer(id, time));
	return id;
}

void bs::TimerSystem::Cancel(const TimerId& id)
{
	myData[id].flags = UINT8_MAX;
}

