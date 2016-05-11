#pragma once

#include "logic\World.h"
#include "logic\WorldEvent.h"
#include "NetDataAdapter.h"
#include "NetInputStream.h"
#include "NetOutputStream.h"

class WorldEventAdapter : public net::DataAdapter
{
public:
	WorldEventAdapter()
	{}

	struct Event
	{
		Event(WorldEvent* aSerializable, uint64_t aTime) 
			: serializable(aSerializable), time(aTime)
		{
		}

		WorldEvent* serializable;
		uint64_t time;

		bool operator <(Event& other) const
		{
			return time < other.time;
		}
	};

	void OnEvent(WorldEvent* serializable, uint64_t time)
	{
		myOutEvents.emplace_back(Event(serializable, time));
	}

	std::vector<Event> GetReceivedEvents()
	{
		std::vector<Event> events = std::move(myInEvents);
		myInEvents = std::vector<Event>();
		return events;
	}

	virtual void Deserialize(net::InputStream& inStream) override final
	{
		bool isHeader = true;
		while (inStream.HasDataLeft())
		{
			if (isHeader)
			{
				WorldEvent::Type type = static_cast<WorldEvent::Type>(inStream.ReadU8());
				WorldEvent* ev = nullptr;
				if (inStream.HasDataLeft())
				{
					if (type == WorldEvent::Type::Spawn)
					{
						ev = new SpawnEvent();
						ev->Deserialize(inStream);
					}
				}
				if (ev != nullptr)
				{
					myInEvents.emplace_back(Event(ev, 0));
				}
			}
		}
	}

	virtual bool Serialize(net::OutputStream& aStream,
						   net::DataAdapter::Receivers& aReceiverList) override final
	{
		if (aReceiverList.empty())
		{
			if (!myOutEvents.empty())
			{
				std::sort(myOutEvents.begin(), myOutEvents.end());
				for (size_t i = 0; i < myOutEvents.size(); i++)
				{
					aStream.WriteU8(static_cast<uint8_t>(myOutEvents[i].serializable->GetType()));
					myOutEvents[i].serializable->Serialize(aStream);
				}
				myOutEvents.clear();
			}
			return true;
		}
		return false;
	}

private:
	std::vector<Event> myOutEvents;
	std::vector<Event> myInEvents;
};