#pragma once

#include "logic\World.h"
#include "logic\WorldEvent.h"
#include "NetDataProxy.h"

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
		myEvents.emplace_back(Event(serializable, time));
	}

	virtual void Deserialize(std::vector<uint8_t>& aData) override final
	{
		bool isHeader = true;
		size_t pos = 0;
		while (pos < aData.size())
		{
			if (isHeader)
			{
				WorldEvent::Type type = static_cast<WorldEvent::Type>(aData[pos]);
				pos++;
				if (pos < aData.size())
				{
					if (type == WorldEvent::Type::Spawn)
					{
						auto ev = new SpawnEvent();
						ev->Deserialize(aData, pos);
					}
				}
			}
		}
	}

	virtual std::vector<uint8_t> Serialize() override final
	{
		std::sort(myEvents.begin(), myEvents.end());
		std::vector<uint8_t> data;
		for (size_t i = 0; i < myEvents.size(); i++)
		{
			data.emplace_back(static_cast<uint8_t>(myEvents[i].serializable->GetType()));
			myEvents[i].serializable->Serialize(data);
		}
		myEvents.clear();
		return data;
	}

private:
	std::vector<Event> myEvents;
};