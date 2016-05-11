#pragma once

#include <cstdint>
#include <vector>
#include <array>

#include  "util\vec2.hpp"

struct UniqueId
{
	uint16_t myLocalId;
	uint8_t myVersion;
	uint8_t myClientId;
};

namespace net { class InputStream; class OutputStream; }

class World;
class WorldEvent
{
public:
	enum class Type : uint8_t
	{
		Game,
		Spawn,
		ControlStick,
		None
	};

	virtual void Begin(World& world) = 0;
	virtual void End(World& world) = 0;
	virtual void Serialize(net::OutputStream& aStream) {}
	virtual void Deserialize(net::InputStream& aStream) {}
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

	virtual void Serialize(net::OutputStream& aStream) override final;

	virtual void Deserialize(net::InputStream& aStream) override final;

private:
	Type myType;
	uint16_t myObjId;
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

// Control stick position changed
class ControlStickEvent : public WorldEvent
{
public:
	ControlStickEvent() {}
	void Begin(World& world) {}
	void End(World& world) {}
	virtual WorldEvent::Type GetType() const override final { return WorldEvent::Type::ControlStick; }

private:
	std::array<float,2> myPos; // TODO: Store as quantized value
};