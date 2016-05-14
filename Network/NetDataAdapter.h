#pragma once

#include <cstdio>
#include <vector>
#include <cassert>

#include "NetOutputStream.h"
#include "NetTypes.h"

namespace net
{
	class InputStream;

	// Adapter for converting between local and network data.
	class DataAdapter
	{
	public:
		// Output Stream with configurable settings
		class OutputStream : public net::OutputStream
		{
		public:
			OutputStream(size_t init) 
				: 
				net::OutputStream(init),
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

		// This method is called when serializing data to network. Method must write its serializable
		// data to the stream if data can be sent with the given stream configuration. Method can call
		// CanConfigure() to check if stream settings can be changed. Only if CanConfigure() returns
		// true, method can and must change stream parameters to match its needs.
		//
		// aStream: Output stream
		//
		// Returns true if adapter was able to serialize everything with the given configuration.
		// Returning false means that Serialize() will be called again with different configuration
		// during current outgoing data update. Also please note that,
		// - Serialize() may be called multiple times during data update whether false was returned or not.
		// - Returning false, when stream can be configured, is illegal.
		virtual bool Serialize(OutputStream& aStream) = 0;
		
		// Called when deserializing data from network
		virtual void Deserialize(net::InputStream& aStream) = 0;
	};
}

