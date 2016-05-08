#pragma once

namespace net
{
	class OutputStream;
	class DataProxy
	{
	public:
		virtual void Serialize(net::OutputStream& aStream) = 0;
		virtual void Deserialize(const uint8_t* data, size_t size) = 0;
	};
}

