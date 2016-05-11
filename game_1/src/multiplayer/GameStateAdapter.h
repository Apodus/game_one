#pragma once

#include "NetDataAdapter.h"
#include "NetInputStream.h"
#include "NetOutputStream.h"

class GameStateAdapter : public net::DataAdapter
{
public:
	GameStateAdapter()
	{}

	virtual void Deserialize(net::InputStream& inStream) override final
	{

	}

	virtual bool Serialize(net::OutputStream& aStream, 
						   net::DataAdapter::Receivers& aReceiverList) override final
	{
		return true;
	}


};
