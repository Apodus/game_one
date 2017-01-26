#pragma once

#include "Types.hpp"
#include "PriorityQueue.hpp"
#include "Array.hpp"
#include "IdPool.h"

namespace bs
{
	class TimerSystem
	{
	public:
		static const U16 MaxTimers = 1024*16;
		typedef U16 TimerId;
		static const U16 InvalidTrigger = 0;
		typedef U16 TriggerId;

		TimerSystem();

		TimerId Create(const TriggerId& id, U32 time);

		void Cancel(const TimerId& id);

		template<typename TCallback>
		void Update(U32 time, TCallback&& callback)
		{
			while (!myQueue.empty() && time >= myQueue.top().time)
			{
				const auto& top = myQueue.top();
				callback(top.trigger);
				myIdPool.Free(top.id);
				myQueue.pop();
			}
		}

	private:

		TriggerId GetTrigger(const TimerId& id) const
		{
			if (myData[id].flags != UINT8_MAX)
			{
				return myData[id].trigger;
			}
			return InvalidTrigger;
		}

		struct TimerData
		{
			TriggerId trigger;
			U8 flags;
		};

		Array<TimerData, MaxTimers> myData;
		IdPool<TimerId> myIdPool;

		struct Timer
		{
			Timer(TimerId anId, U32 aTime) : id(anId), time(aTime) {}
			TimerId id;
			U32 time;
			bool operator<(const Timer &other) const { return time > other.time; }
			bool operator>(const Timer &other) const { return time < other.time; }
			const U32 GetPriority() const { return time; }
		};

		bs::PriorityQueue<Timer> myQueue;

	};

}