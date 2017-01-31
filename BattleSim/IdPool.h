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

		bool HasFreeItems() const { return !myFreeItems.empty(); }

		T Reserve()
		{
			return myFreeItems.empty() ? ReserveNew() : ReserveFree();
		}

		void Free(const T& index)
		{
			myFreeItems.emplace_back(index);
		}

		T Size() const { return myTotalItems - static_cast<T>(myFreeItems.Size()); }

	private:
		T ReserveNew()
		{
			T val = myTotalItems;
			ASSERT(static_cast<T>(myTotalItems + 1) != 0, "Out of id space");
			myTotalItems++;
			return val;
		}

		T ReserveFree()
		{
			T val = myFreeItems.back();
			myFreeItems.pop_back();
			return val;
		}

		Vector<T> myFreeItems;
		T myTotalItems;
	};
}