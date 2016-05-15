#include "multiplayer/SessionAdapter.h"

#include "multiplayer/GameSession.h"
#include "NetAdapterInputStream.h"
#include "NetAdapterOutputStream.h"

SessionAdapter::SessionAdapter(GameSession& aSession)
	:
	mySession(aSession)
{}


void SessionAdapter::Deserialize(net::AdapterInputStream& inStream)
{

}

bool SessionAdapter::Serialize(net::AdapterOutputStream& aStream)
{
	if (!myNewMembers.empty() || !myUpdatedLevels.empty())
	{
		std::vector<net::MemberIndex> receivers;
		if (!myNewMembers.empty())
		{
			if (aStream.CanConfigure())
			{
				aStream.SetReceivers(myNewMembers);
				receivers = std::move(myNewMembers);
				myNewMembers = std::vector<net::MemberIndex>();
			}
			else
			{
				receivers = aStream.FilterMatchingReceivers(myNewMembers);
			}

			if (aStream.IsReliable() && aStream.GetReceivers().size() == receivers.size())
			{
				for (size_t i = 0; i < receivers.size(); i++)
				{
					for (size_t j = 0; j < myNewMembers.size(); j++)
					{
						if (myNewMembers[j] == receivers[i])
						{
							myNewMembers.erase(myNewMembers.begin() + j);
							break;
						}
					}
				}
				mySession.Serialize(aStream);
			}
			else
			{
				return false;
			}
		}
		if (!myUpdatedLevels.empty())
		{
			if (aStream.IsReliable() && aStream.GetReceivers().empty())
			{
				for (size_t i = 0; i < myUpdatedLevels.size(); i++)
				{
					aStream.WriteU8(myUpdatedLevels[i]);
				}
			}
			else
			{
				return false;
			}
		}
	}
	return true;
}

bool SessionAdapter::IsStreamSuitable(net::AdapterOutputStream& aStream)
{
	if (!aStream.IsReliable())
	{
		return false;
	}

	if (aStream.IsEmpty())
	{
		return false;
	}

	for (size_t i = 0; i < aStream.GetReceivers().size(); i++)
	{
		bool isFound = false;
		for (size_t j = 0; j < myNewMembers.size(); j++)
		{
			if (myNewMembers[j] == aStream.GetReceivers()[i])
			{
				isFound = true;
				break;
			}
		}
		if (!isFound)
		{
			return false;
		}
	}
	return true;
}

void SessionAdapter::OnNewMember(net::MemberIndex member)
{
	myNewMembers.emplace_back(member);
}
