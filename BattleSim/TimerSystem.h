#pragma once

#include "Types.hpp"
#include "PriorityQueue.hpp"
#include "Array.hpp"
#include "IdPool.h"

namespace bs
{
	template<typename TTrigger, typename TTime = Tick>
	class TimerSystem
	{
	public:
		static const U16 MaxTimers = 1024 * 16;
		static const TTrigger InvalidTrigger = 0;

		TimerSystem() {}

		TimerId Create(const TTrigger& triggerId, TTime time)
		{
			auto id = myIdPool.Reserve();
			myData[id].trigger = triggerId;
			myData[id].flags = 0;
			myQueue.push(Timer(id, time));
			return id;
		}

		void Cancel(const TimerId& id)
		{
			ASSERT(id != InvalidTimer, "Invalid timer");
			myData[id].flags = UINT8_MAX;
		}

		template<typename TCallback>
		void Update(TTime time, TCallback&& callback)
		{
			while (!myQueue.empty() && time >= myQueue.top().time)
			{
				const auto top = myQueue.top();
				const bool isActive = myData[top.id].flags != UINT8_MAX;
				const TTrigger triggerId = myData[top.id].trigger;
				myIdPool.Free(top.id);
				myQueue.pop();
				if (isActive)
				{
					callback(triggerId);
				}
			}
		}

	private:

		TTrigger GetTrigger(const TimerId& id) const
		{
			if (myData[id].flags != UINT8_MAX)
			{
				return myData[id].trigger;
			}
			return InvalidTrigger;
		}

		struct TimerData
		{
			TTrigger trigger;
			U8 flags;
		};

		Array<TimerData, MaxTimers> myData;
		IdPool<TimerId> myIdPool;

		struct Timer
		{
			Timer(TimerId anId, TTime aTime) : id(anId), time(aTime) {}
			TimerId id;
			TTime time;
			bool operator<(const Timer &other) const { return time > other.time; }
			bool operator>(const Timer &other) const { return time < other.time; }
			const TTime GetPriority() const { return time; }
		};

		bs::PriorityQueue<Timer> myQueue;

	};

}