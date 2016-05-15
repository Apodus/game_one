#pragma once

#include "NetInputStream.h"

namespace net
{
	class AdapterInputStream : public InputStream
	{
	public:
		AdapterInputStream(const uint8_t* data, size_t size, uint64_t sourceId)
			:
			InputStream(data, size),
			mySource(sourceId)
		{
		}

		uint64_t GetSource() const { return mySource; }

	private:
		uint64_t mySource;


	};
}
