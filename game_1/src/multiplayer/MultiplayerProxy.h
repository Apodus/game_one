#pragma once

#include "logic\World.h"
#include "logic\WorldEvent.h"
#include "NetDataProxy.h"
#include "NetInputStream.h"

class MultiplayerProxy : public net::DataProxy
{
public:
	MultiplayerProxy() 
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

	virtual void Deserialize(const uint8_t* aData, size_t size) override final
	{
		bool isHeader = true;
		net::InputStream inStream(aData, size);
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

	virtual std::vector<uint8_t> Serialize() override final
	{
		std::sort(myOutEvents.begin(), myOutEvents.end());
		std::vector<uint8_t> data;
		for (size_t i = 0; i < myOutEvents.size(); i++)
		{
			data.emplace_back(static_cast<uint8_t>(myOutEvents[i].serializable->GetType()));
			myOutEvents[i].serializable->Serialize(data);
		}
		myOutEvents.clear();
		return data;
	}

private:
	std::vector<Event> myOutEvents;
	std::vector<Event> myInEvents;
};