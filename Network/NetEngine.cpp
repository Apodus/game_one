#include "NetPch.h"
#include "NetEngine.h"
#include "NetPhoton.h"
#include "NetDataAdapter.h"

net::Engine::Engine()
{
}

void net::Engine::PreTick()
{
	for (size_t i = 0; i < mySessions.size(); i++)
	{
		mySessions[i]->PreTick();
	}
}

void net::Engine::PostTick()
{
	for (size_t i = 0; i < mySessions.size(); i++)
	{
		mySessions[i]->PostTick();
	}
}

void net::Engine::AddSession(Session* aSession)
{
	mySessions.emplace_back(aSession);
}

void net::Engine::RemoveSession(Session* aSession)
{
	for (size_t i = 0; i < mySessions.size(); i++)
	{
		if (mySessions[i] == aSession)
		{
			mySessions.erase(mySessions.begin() + i);
			break;
		}
	}
}

net::Engine::~Engine()
{
}
