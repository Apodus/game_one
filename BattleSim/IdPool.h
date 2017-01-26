#pragma once

#include "Deque.hpp"

namespace bs
{
	template<typename T>
	class IdPool
	{
	public:
		IdPool() : myTotalItems(0)
		{
		}

		T Reserve()
		{
			T val;
			if (myFreeItems.empty())
			{
				val = myTotalItems;
				ASSERT(static_cast<T>(myTotalItems + 1) != 0, "Out of id space");
				myTotalItems++;
			}
			else
			{
				val = myFreeItems.front();
				myFreeItems.pop_front();
			}
			return val;
		}

		void Free(const T& index)
		{
			ASSERT(index < myTotalItems, "Invalid index");
			if (myFreeItems.empty() || myFreeItems.back() < index)
			{
				if (index + 1 != myTotalItems)
				{
					myFreeItems.emplace_back(index);
				}
				else
				{
					ASSERT(myTotalItems > 0, "Invalid item");
					myTotalItems--;
				}
			}
			else
			{
				myFreeItems.emplace_back(index);
			}
		}

		T Size() const { return myTotalItems - static_cast<T>(myFreeItems.Size()); }

	private:
		Deque<T> myFreeItems;
		T myTotalItems;
	};
}