#pragma once

#include "NetOutputStream.h"
#include "NetTypes.h"

#include <cassert>

namespace net
{
	// Output Stream with configurable settings
	class AdapterOutputStream : public net::OutputStream
	{
	public:
		AdapterOutputStream(size_t init)
			:
			OutputStream(init),
			myIsReliable(true)
		{}

		void SetReceivers(const std::vector<MemberIndex>& receivers)
		{
			assert(CanConfigure());
			myReceivers = receivers;
		}

		// Stream is reliable by default, this will set it unreliable.
		void SetUnreliable()
		{
			assert(CanConfigure());
			myIsReliable = false;
		}

		const std::vector<MemberIndex>& GetReceivers() const { return myReceivers; }

		bool IsReliable() const { return myIsReliable; }

		bool CanConfigure() const { return IsEmpty() && myIsReliable && myReceivers.empty(); }

		std::vector<MemberIndex> FilterMatchingReceivers(const std::vector<MemberIndex>& theOthers) const
		{
			std::vector<MemberIndex> receivers;
			for (size_t i = 0; i < myReceivers.size(); i++)
			{
				for (size_t j = 0; j < theOthers.size(); j++)
				{
					if (myReceivers[i] == theOthers[j])
					{
						receivers.emplace_back(myReceivers[i]);
						break;
					}
				}

			}
			return receivers;
		}

	private:
		// Data receivers. Empty, if data can be received by everyone.
		std::vector<MemberIndex> myReceivers;
		bool myIsReliable; // Is data reliable or unreliable
	};

}