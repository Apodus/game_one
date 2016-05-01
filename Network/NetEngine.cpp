#include "NetPch.h"
#include "NetEngine.h"

#include "NetPhoton.h"

net::Engine::Engine()
{
	myPhoton = std::make_unique<net::Photon>();
}

net::Engine::~Engine()
{
}

void net::Engine::Tick()
{
	myPhoton->run();
}