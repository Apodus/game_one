#pragma once

#include <cstdint>
#include <vector>

#include  "util\vec2.hpp"

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
	enum class Type : uint8_t
	{
		Game,
		Spawn,
		None
	};

	virtual void Begin(World& world) = 0;
	virtual void End(World& world) = 0;
	virtual void Serialize(std::vector<unsigned char>& data) {}
	virtual void Deserialize(const std::vector<uint8_t>& aData, size_t& pos) {}
	virtual ~WorldEvent() {};
	virtual Type GetType() const = 0;
};

class SceneObject;
class World;
class SpawnEvent : public WorldEvent
{
public:
	enum class Type : uint8_t
	{
		Hero,
		Thug,
		Bullet
	};

	SpawnEvent() {}
	SpawnEvent(Type t, float x = 0, float y = 0, float dir = 0, float vx = 0, float vy = 0);
	void Begin(World& world);
	void End(World& world);

	virtual WorldEvent::Type GetType() const override final { return WorldEvent::Type::Spawn; }

	virtual void Serialize(std::vector<unsigned char>& data) override final;

	virtual void Deserialize(const std::vector<uint8_t>& aData, size_t& pos) override final;

private:
	Type myType;
	UniqueId id;
	SceneObject* myObj;
	sa::vec2<float> mySpawnPos;
	float mySpawnDir;
	sa::vec2<float> mySpawnVelocity;
};

class GameEvent : public WorldEvent
{
	void Begin(World& world) {}
	void End(World& world) {}
	virtual WorldEvent::Type GetType() const override final { return WorldEvent::Type::Game; }
};