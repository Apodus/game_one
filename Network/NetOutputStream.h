#pragma once

#include <vector>

namespace net
{

	class OutputStream
	{
	public:
		OutputStream(size_t size)
			:
			myBuffer()
		{
			myBuffer.reserve(size);
		}

		void WriteU8(uint8_t val)
		{
			myBuffer.emplace_back(val);
		}

		template<typename T>
		void WriteU32(T val)
		{
			uint8_t const * p = reinterpret_cast<uint8_t const *>(&val);
			WriteU8(p[0]);
			WriteU8(p[1]);
			WriteU8(p[2]);
			WriteU8(p[3]);
		}

		template<typename T>
		void WriteU64(T val)
		{
			uint8_t const * p = reinterpret_cast<uint8_t const *>(&val);
			WriteU8(p[0]);
			WriteU8(p[1]);
			WriteU8(p[2]);
			WriteU8(p[3]);
			WriteU8(p[4]);
			WriteU8(p[5]);
			WriteU8(p[6]);
			WriteU8(p[7]);
		}

		const uint8_t* GetData() const { return &myBuffer[0]; }

		size_t GetSize() const { return myBuffer.size(); }

		bool IsEmpty() const { return myBuffer.empty(); }

		void Clear() { myBuffer.clear(); }

	private:
		std::vector<uint8_t> myBuffer;
	};
}
