#include "NetPch.h"
#include "NetEngine.h"
#include "NetPhoton.h"
#include "NetDataProxy.h"

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

void net::Engine::SetProxy(net::DataProxy& proxy)
{
	myPhoton->SetProxy(proxy);
}


void net::Engine::PostTick()
{
	if (myPhoton->IsSendScheduled())
	{
		myPhoton->Send();
	}	
}