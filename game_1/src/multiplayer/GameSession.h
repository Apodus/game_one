#pragma once

#include "NetSession.h"
#include "multiplayer/SessionAdapter.h"
#include "NetInputStream.h"

class GameSession : public net::Session
{
public:

	GameSession(net::Engine& engine) : net::Session(engine), myAdapter(*this)
	{
		AddAdapter(0, 22, myAdapter);
	}


	virtual void OnMemberJoined(net::MemberIndex i) final override
	{
	}

	virtual void OnMemberLeft(net::MemberIndex i) final override
	{
	}

	void Serialize(net::DataAdapter::OutputStream& aStream)
	{
		aStream.WriteU8(static_cast<uint8_t>(myPlayers.size()));
		for (size_t i = 0; i < myPlayers.size(); i++)
		{
			aStream.WriteU64(GetMemberId(myPlayers[i].localMemberIndex));
			aStream.WriteU8(myPlayers[i].localPlayerId);
			aStream.WriteU64(GetMemberId(myPlayers[i].instance.localMemberIndex));
			aStream.WriteU8(myPlayers[i].instance.instanceId);
		}
	}

	void Deserialize(net::InputStream& inStream)
	{
		uint8_t numPlayers = inStream.ReadU8();

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