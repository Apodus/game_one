#pragma once

#include "NetDataAdapter.h"
#include "NetTypes.h"

namespace net { class InputStream; }
class GameSession;

class SessionAdapter : public net::DataAdapter
{
public:
	SessionAdapter(GameSession& aSession);

	virtual void Deserialize(net::InputStream& inStream) override final;

	virtual bool Serialize(net::DataAdapter::OutputStream& aStream) override final;

	void OnNewMember(net::MemberIndex member);

private:
	bool IsStreamSuitable(net::DataAdapter::OutputStream& aStream);

	// Send all player & level info to these players
	std::vector<net::MemberIndex> myNewMembers;

	// Level updates for everyone
	std::vector<uint8_t> myUpdatedLevels;

	GameSession& mySession;
};
