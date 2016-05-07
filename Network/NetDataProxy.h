#pragma once

namespace net
{
	class DataProxy
	{
	public:
		virtual std::vector<uint8_t> Serialize() = 0;
		virtual void Deserialize(std::vector<uint8_t>& ) = 0;
	};
}

