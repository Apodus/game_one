#pragma once

#include "Box2D/Box2D.h"
#include "logic/SceneObject.hpp"
#include "session/game/collisionCallback.hpp"
#include "logic/Timeline.h"

#include <vector>

class World
{
public:
	friend class Timeline;

	World() : physicsWorld({ 0, 0 }), m_timeline(*this)
	{
		physicsWorld.SetContactListener(&contactListener);
	}

	virtual ~World()
	{
	}

	void AddEvent(WorldEvent* record, int adjustment = 0)
	{
		AddEventAt(record, m_timeline.GetTime() + adjustment);
	}

	void AddEventAt(WorldEvent* record, uint64_t time)
	{
		m_timeline.AddEvent(record, time);
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
	Timeline m_timeline;

};
