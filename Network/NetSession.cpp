#include "NetPch.h"
#include "NetSession.h"
#include "NetPhoton.h"
#include "NetEngine.h"

net::Session::Session(Engine& engine) 
	:
	myEngine(engine)
{
	myEngine.AddSession(this);
	myPhoton = std::make_unique<net::Photon>(*this);
}

net::Session::~Session()
{
	myEngine.RemoveSession(this);
}

void net::Session::AddAdapter(uint8_t eventId, uint8_t keyId, net::DataAdapter& adapter)
{
	if (myPhoton)
	{
		myPhoton->AddAdapter(eventId, keyId, adapter);
	}
}

uint64_t net::Session::GetMemberId(MemberIndex i) const
{
	return myMembers[i].id;
}

void net::Session::AddMember(uint64_t id)
{
	size_t i;
	for (i = 0; i < myMembers.size(); i++)
	{
		if (myMembers[i].id == 0)
		{
			break;
		}
	}
	if (i == myMembers.size())
	{
		myMembers.emplace_back();
	}
	myMembers[i].id = id;
	
	OnMemberJoined(static_cast<MemberIndex>(i));
}

void net::Session::RemoveMember(uint64_t id)
{
	for (size_t i = 0; i < myMembers.size(); i++)
	{
		if (myMembers[i].id == id)
		{
			OnMemberLeft(static_cast<MemberIndex>(i));
			myMembers[i].id = 0;
		}
	}
	
}

void net::Session::PreTick()
{
	if (myPhoton)
	{
		myPhoton->Receive();
		myPhoton->Update();
	}
}

void net::Session::PostTick()
{
	if (myPhoton && myPhoton->IsSendScheduled())
	{
		myPhoton->Send();
	}
}
