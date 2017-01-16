#pragma once

#include "Types.hpp"
#include "Vector.hpp"
#include "Deque.hpp"
#include "ByteBuffer.h"

#include <memory>
#include <mutex>

namespace bs
{
	class VisualizationSystem
	{
	public:

		// Minimum memory allocation for a visual stream
		static const size_t MinAllocation = 1024;

		// Minimum number of attempts until memory reallocation is done.
		// To prevent frequent reallocations due to rare small allocation request.
		static const size_t MinReallocRequests = 8;

		VisualizationSystem()
		{
			Reset();
		}

		void Reset()
		{
			Clear();
			MarkUpdateWritten();
		}

		~VisualizationSystem()
		{
			Clear();
		}
		
		// Next update for reading or nullptr if no update available
		const ByteBuffer* GetForReading()
		{
			myMutex.lock();
			auto ptr = myWriteIndex != 0 ? myUpdates[0] : nullptr;
			myMutex.unlock();
			return ptr;
		}

		// Latest update for reading or nullptr if no update available
		const ByteBuffer* GetLatestForReading()
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

		ByteBuffer& StartWriting(size_t numBytes)
		{
			// Choose hottest memory for writing - it should be last added
			myMutex.lock();
			auto ptr = myUpdates.back();
			myUpdates.back() = myUpdates[myWriteIndex];
			myUpdates[myWriteIndex] = ptr;		
			myMutex.unlock();
			
			// Update allocation
			ptr->updateStream.clear(); // To avoid copy
			size_t allocationHint = numBytes + (numBytes >> 2) + MinAllocation;
			allocationHint = (allocationHint % MinAllocation) + allocationHint;
			if (numBytes > ptr->updateStream.capacity())
			{
				ptr->updateStream.resize(allocationHint);
				myNumReallocRequests = 0;
			}
			else
			{
				if (ptr->updateStream.capacity() > allocationHint * 2)
				{
					if (myNumReallocRequests > MinReallocRequests)
					{
						ptr->updateStream.shrink_to_fit();
						ptr->updateStream.resize(allocationHint);
						myNumReallocRequests = 0;
					}
					else
					{
						myNumReallocRequests++;
					}
				}
				ptr->updateStream.resize(numBytes);
			}
#if 0
			LOG("Visualization Stream is %zi bytes (hint %zi bytes)", 
				ptr->updateStream.capacity(), allocationHint);
#endif
			return *ptr;
		}

		void StopWriting()
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
				myUpdates.emplace_back(new ByteBuffer());
			}
		}

		void Clear()
		{
			myMutex.lock();
			for (size_t i = 0; i < myUpdates.size(); i++)
			{
				delete myUpdates[i];
			}
			myUpdates.clear();
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
		Deque<ByteBuffer*> myUpdates;
		std::mutex myMutex;
		size_t myNumReallocRequests = 0;
	};
}
