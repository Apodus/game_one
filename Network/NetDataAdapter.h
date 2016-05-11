#pragma once

#include <cstdio>
#include <vector>

namespace net
{
	class OutputStream;
	class InputStream;

	// Adapter for converting between local and network data.
	class DataAdapter
	{
	public:
		// Data receivers. Empty, if data can be received by everyone.
		typedef std::vector<bool> Receivers;

		// Called when serializing data to network
		//
		// aStream: output stream
		// aReceiverList: List of receivers for the stream. Can be changed if the stream is empty.
		//
		// returns true if adapter was able to serialize everything to given receiver list
		virtual bool Serialize(net::OutputStream& aStream, Receivers& aReceiverList) = 0;
		
		// Called when deserialized data from network
		virtual void Deserialize(net::InputStream& aStream) = 0;
	};
}

