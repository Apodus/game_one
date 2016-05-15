#pragma once

#include "NetSession.h"
#include "multiplayer/SessionAdapter.h"
#include "NetAdapterInputStream.h"
#include "NetAdapterOutputStream.h"

class GameSession : public net::Session
{
public:

	GameSession(net::Engine& engine) : net::Session(engine), myAdapter(*this)
	{
		AddAdapter(0, 22, myAdapter);
	}


	virtual void OnMemberJoined(net::MemberIndex i) final override
	{
		myAdapter.OnNewMember(i);
	}

	virtual void OnMemberLeft(net::MemberIndex i) final override
	{
	}

	void Serialize(net::AdapterOutputStream& aStream)
	{
		// Local Id -> Global player Id Mapping
		aStream.WriteU8(static_cast<uint8_t>(myPlayers.size()));
		for (size_t i = 0; i < myPlayers.size(); i++)
		{
			aStream.WriteU64(GetMemberId(myPlayers[i].localMemberIndex));
			aStream.WriteU8(myPlayers[i].localPlayerId);
		}

		if (!myPlayers.empty())
		{
			// Local player current instance
			aStream.WriteU64(GetMemberId(myPlayers[0].instance.localMemberIndex));
			aStream.WriteU8(myPlayers[0].instance.instanceId);
		}
	}

	void Deserialize(net::AdapterInputStream& aStream)
	{
		// Local Id -> Global player Id Mapping
		uint8_t numPlayers = aStream.ReadU8();
		for (size_t i = 0; i < numPlayers; i++)
		{
			auto memberId = aStream.ReadU64<uint64_t>();
			auto mappedId = aStream.ReadU8();
		}

		// Remote player current instance
		auto instanceOwner = aStream.ReadU64<uint64_t>();
		auto instanceId = aStream.ReadU8();

	}

	typedef uint8_t PlayerIndex;

	struct Instance
	{
		net::MemberIndex localMemberIndex;
		uint8_t instanceId;
	};

	struct Player
	{
		Instance instance; // Level instance player is playing
		net::MemberIndex localMemberIndex; // Local session member index 
		uint8_t localPlayerId; // Local id
	};

	void AddPlayer(PlayerIndex pi, net::MemberIndex mi = net::LocalMemberIndex)
	{
		if (myPlayers.size() < pi)
		{
			myPlayers.reserve(pi);
		}
		myPlayers[pi].localMemberIndex = mi;
		myPlayers[pi].localPlayerId = 0;
		myPlayers[pi].instance.instanceId = 0;
		myPlayers[pi].instance.localMemberIndex = 0;
	}

	void RemovePlayer(PlayerIndex pi)
	{
		myPlayers[pi].localMemberIndex = net::InvalidMemberIndex;
	}

private:
	SessionAdapter myAdapter;

	std::vector<Player> myPlayers;

	struct RemoteLevel
	{
		Instance instance;
		uint64_t levelId;
	};

	std::vector<uint64_t> myLocalLevels;
	std::vector<RemoteLevel> myRemoteLevels;
};