#pragma once

#include "Box2D/Box2D.h"
#include "logic/SceneObject.hpp"
#include "session/game/collisionCallback.hpp"
#include "logic/Timeline.h"
#include "multiplayer/MultiplayerProxy.h"
#include "logic/ShapePrototype.hpp"

#include <vector>

class World
{
public:
	friend class Timeline;

	World(MultiplayerProxy& proxy) 
		: 
		physicsWorld({ 0, 0 }), 
		m_proxy(proxy),
		m_timeline(*this)		
	{
		physicsWorld.SetContactListener(&contactListener);
	}

	virtual ~World()
	{
	}

	void AddEvent(WorldEvent* record, int adjustment = 0, bool broadcast = true)
	{
		auto time = m_timeline.GetTime() + adjustment;
		AddEventAt(record, time, broadcast);
	}

	void AddEventAt(WorldEvent* record, uint64_t time, bool broadcast = true)
	{
		m_timeline.AddEvent(record, time);
		if (broadcast)
		{
			m_proxy.OnEvent(record, time);
		}
	}

	SceneObject& createObject(const b2BodyDef& bodyDef, const ShapePrototype& shape, const std::string& texture)
	{
		auto body = physicsWorld.CreateBody(&bodyDef);
		shape.attach(body, 4);
		newObjs.emplace_back(
			body,
			shape,
			texture
		);

		return newObjs.back();
	}

	// TODO: Mark as dead, and remove at end of frame or something
	void destroyObject(SceneObject* obj)
	{
		physicsWorld.DestroyBody(obj->getTransform().m_body);
	}

	void tick(long long timeMs)
	{
		m_timeline.Run(timeMs);
	}

	const std::vector<SceneObject>& GetObjects() const
	{
		return objs;
	}

	void update(sa::UserIO& io)
	{
		for (auto&& obj : objs)
			obj.update(io);
	}

private:
	std::vector<SceneObject> objs;
	std::vector<SceneObject> newObjs;

	GameContactListener contactListener;
	b2World physicsWorld;

	MultiplayerProxy& m_proxy;
	Timeline m_timeline;	

};
