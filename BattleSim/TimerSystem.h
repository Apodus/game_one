#pragma once

#include "Types.hpp"
#include "PriorityQueue.hpp"
#include "Array.hpp"
#include "IdPool.h"

namespace bs
{
	template<typename T>
	class TimerSystem
	{
	public:
		static const U16 MaxTimers = 1024 * 16;
		typedef U16 TimerId;
		static const T InvalidTrigger = 0;

		TimerSystem() {}

		TimerId Create(const T& triggerId, U32 time)
		{
			auto id = myIdPool.Reserve();
			myData[id].trigger = triggerId;
			myData[id].flags = 0;
			myQueue.push(Timer(id, time));
			return id;
		}

		void Cancel(const TimerId& id)
		{
			myData[id].flags = UINT8_MAX;
		}

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

		T GetTrigger(const TimerId& id) const
		{
			if (myData[id].flags != UINT8_MAX)
			{
				return myData[id].trigger;
			}
			return InvalidTrigger;
		}

		struct TimerData
		{
			T trigger;
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