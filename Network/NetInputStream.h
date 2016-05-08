#pragma once

#include <cstdint>

namespace net
{
	class InputStream
	{
	public:
		InputStream(const uint8_t* data, size_t size)
			:
			myData(data),
			mySize(size),
			myPos(0)
		{

		}

		uint8_t ReadU8()
		{
			auto out = myData[myPos];
			myPos++;
			return out;
		}

		bool HasDataLeft() const 
		{
			return myPos < mySize;
		}

	private:
		const uint8_t* myData;
		const size_t mySize;
		size_t myPos;
	};
}
