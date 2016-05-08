#include "NetPch.h"
#include "NetEngine.h"
#include "NetPhoton.h"
#include "NetDataAdapter.h"

net::Engine::Engine()
{
	myPhoton = std::make_unique<net::Photon>();
}

net::Engine::~Engine()
{
}

void net::Engine::PreTick()
{
	myPhoton->Receive();
	myPhoton->Update();
}

void net::Engine::AddAdapter(uint8_t eventId, uint8_t keyId, net::DataAdapter& adapter)
{
	myPhoton->AddAdapter(eventId, keyId, adapter);
}


void net::Engine::PostTick()
{
	if (myPhoton->IsSendScheduled())
	{
		myPhoton->Send();
	}	
}