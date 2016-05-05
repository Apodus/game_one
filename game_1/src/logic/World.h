#pragma once

#include "Box2D/Box2D.h"
#include "logic/SceneObject.hpp"
#include "session/game/collisionCallback.hpp"

#include <vector>

class World
{
public:
	World() : physicsWorld({ 0, 0 })
	{
		physicsWorld.SetContactListener(&contactListener);
	}


	GameContactListener contactListener;
	b2World physicsWorld;
	std::vector<SceneObject> objs;
	std::vector<SceneObject> newObjs;

};
