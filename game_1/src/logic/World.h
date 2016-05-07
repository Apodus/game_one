#pragma once

#include "Box2D/Box2D.h"
#include "logic/SceneObject.hpp"
#include "session/game/collisionCallback.hpp"
#include "logic/Timeline.h"

#include <vector>

class World
{
public:
	World() : physicsWorld({ 0, 0 }), m_timeline(*this)
	{
		physicsWorld.SetContactListener(&contactListener);
	}

	void AddEvent(WorldEvent* record, int adjustment = 0)
	{
		m_timeline.AddEvent(record, m_timeline.GetTime() + adjustment);
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
	std::vector<SceneObject> objs;
	std::vector<SceneObject> newObjs;

protected:

private:
	Timeline m_timeline;

};
