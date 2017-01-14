#pragma once

#include "Types.hpp"
#include "Vector.hpp"
#include "Deque.hpp"
#include "UpdateData.h"

#include <memory>
#include <mutex>

namespace bs
{
	class UpdateSystem
	{
	public:

		UpdateSystem()
		{
			Reset();
		}

		void Reset()
		{
			Clear();
			MarkUpdateWritten();
		}

		~UpdateSystem()
		{
			Clear();
		}
		
		// Next update for reading or nullptr if no update available
		const UpdateData* GetForReading()
		{
			myMutex.lock();
			auto ptr = myWriteIndex != 0 ? myUpdates[0] : nullptr;
			myMutex.unlock();
			return ptr;
		}

		// Latest update for reading or nullptr if no update available
		const UpdateData* GetLatestForReading()
		{
			myMutex.lock();
			if (myWriteIndex > 1)
			{
				ConsumeUpdate(myWriteIndex - 1);
			}
			auto ptr = myWriteIndex != 0 ? myUpdates[0] : nullptr;
			myMutex.unlock();
			return ptr;
		}

		void ConsumeUpdate()
		{
			myMutex.lock();
			ConsumeUpdate(1);
			myMutex.unlock();
		}

		UpdateData& StartUpdate()
		{
			myMutex.lock();
			auto ptr = myUpdates[myWriteIndex];
			myMutex.unlock();
			return *ptr;
		}

		void StopUpdate()
		{
			myMutex.lock();
			MarkUpdateWritten();
			myMutex.unlock();
		}

	private:
		void MarkUpdateWritten()
		{
			myWriteIndex++;
			if (myWriteIndex >= myUpdates.size())
			{
				myUpdates.emplace_back(new UpdateData());
			}
		}

		void Clear()
		{
			myMutex.lock();
			for (size_t i = 0; i < myUpdates.size(); i++)
			{
				delete myUpdates[i];
			}
			myWriteIndex = static_cast<size_t>(-1);
			myMutex.unlock();
		}

		void ConsumeUpdate(size_t numConsumes)
		{
			for (size_t i = 0; i < numConsumes; i++)
			{
				auto tmp = myUpdates[i];
				myUpdates.emplace_back(tmp);
				myWriteIndex--;
			}
			myUpdates.erase(myUpdates.begin(), myUpdates.begin() + numConsumes);
		}

		size_t myWriteIndex;
		Deque<UpdateData*> myUpdates;
		std::mutex myMutex;
	};
}
