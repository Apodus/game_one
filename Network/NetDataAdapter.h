#pragma once

#include <cstdio>
#include <vector>
#include <cassert>

#include "NetOutputStream.h"

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
			
			void SetReceivers(std::vector<uint16_t>& receivers)
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

			const std::vector<uint16_t>& GetReceivers() const { return myReceivers; }

			bool IsReliable() const { return myIsReliable; }

			bool CanConfigure() const { return IsEmpty() && myIsReliable && myReceivers.empty(); }

		private:
			// Data receivers. Empty, if data can be received by everyone.
			std::vector<uint16_t> myReceivers;
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

