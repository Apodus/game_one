#pragma once

namespace net
{
	class OutputStream;
	class InputStream;
	class DataAdapter
	{
	public:
		virtual void Serialize(net::OutputStream& aStream) = 0;
		virtual void Deserialize(net::InputStream& aStream) = 0;
	};
}

