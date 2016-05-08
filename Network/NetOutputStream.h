#pragma once

#include <vector>

namespace net
{

	class OutputStream
	{
	public:
		OutputStream(size_t size)
			:
			myBuffer(size)
		{}

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

		const uint8_t* GetData() const { return &myBuffer[0]; }

		size_t GetSize() const { return myBuffer.size(); }

	private:
		std::vector<uint8_t> myBuffer;
	};
}
