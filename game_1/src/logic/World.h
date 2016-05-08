#pragma once

#include "Box2D/Box2D.h"
#include "logic/SceneObject.hpp"
#include "session/game/collisionCallback.hpp"
#include "logic/Timeline.h"
#include "multiplayer\MultiplayerProxy.h"

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

	void tick(long long timeMs)
	{
		m_timeline.Run(timeMs);
	}

	// TODO: Reduce visibility
	GameContactListener contactListener;
	b2World physicsWorld;	
	std::vector<SceneObject> newObjs;

	std::vector<SceneObject>& GetObjects() { return objs; }


protected:

private:
	std::vector<SceneObject> objs;
	MultiplayerProxy& m_proxy;
	Timeline m_timeline;	

};
