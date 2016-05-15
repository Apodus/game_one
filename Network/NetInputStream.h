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
			uint8_t val = myPos < mySize ? myData[myPos] : 0;
			myPos++;
			return val;
		}

		template<typename T>
		T ReadU16()
		{
			uint8_t value[2];
			value[0] = ReadU8();
			value[1] = ReadU8();
			return *reinterpret_cast<T const *>(value);
		}

		template<typename T>
		T ReadU32()
		{
			uint8_t value[4];
			for (size_t i = 0; i < 4; i++)
			{
				value[i] = ReadU8();
			}
			return *reinterpret_cast<T const *>(value);
		}

		template<typename T>
		T ReadU64()
		{
			uint8_t value[8];
			for (size_t i = 0; i < 8; i++)
			{
				value[i] = ReadU8();
			}
			return *reinterpret_cast<T const *>(value);
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
