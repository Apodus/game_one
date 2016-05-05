#pragma once

#include "Box2D/Box2D.h"
#include "logic/SceneObject.hpp"

#include <vector>

class World
{
public:
	World() : physicsWorld({ 0, 0 })
	{

	}


	b2World physicsWorld;
	std::vector<SceneObject> objs;
	std::vector<SceneObject> newObjs;

};
