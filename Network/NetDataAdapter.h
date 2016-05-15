#pragma once

#include <cstdio>
#include <vector>

#include "NetTypes.h"

namespace net
{
	class InputStream;
	class AdapterOutputStream;
	class AdapterInputStream;

	// Adapter for converting between local and network data.
	class DataAdapter
	{
	public:

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
		virtual bool Serialize(AdapterOutputStream& aStream) = 0;
		
		// Called when deserializing data from network
		virtual void Deserialize(AdapterInputStream& aStream) = 0;
	};
}

