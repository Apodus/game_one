#pragma once

#include <cstdint>


struct UniqueId
{
	uint16_t myLocalId;
	uint8_t myVersion;
	uint8_t myClientId;
};

class World;
class WorldEvent
{
public:
	virtual void Begin(World& world) = 0;
	virtual void End(World& world) = 0;
};

class b2Body;
class World;
class SpawnEvent : public WorldEvent
{
public:
	SpawnEvent() : myBody(nullptr) {}
	void Begin(World& world);
	void End(World& world);

	UniqueId id;

	b2Body* myBody;
};

class GameEvent : public WorldEvent
{
	void Begin(World& world) {}
	void End(World& world) {}
};