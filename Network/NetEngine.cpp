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
	myPhoton->run();
}

void net::Engine::PostTick(net::DataProxy& proxy)
{
	if (myPhoton->IsServiceScheduled())
	{
		myPhoton->Service(proxy.Serialize());		
	}	
}