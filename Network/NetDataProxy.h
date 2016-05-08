#pragma once

namespace net
{
	class DataProxy
	{
	public:
		virtual std::vector<uint8_t> Serialize() = 0;
		virtual void Deserialize(const uint8_t* data, size_t size) = 0;
	};
}

