#pragma once

#include "logic/SceneObject.hpp"
#include "session/game/collisionCallback.hpp"
#include "logic/ShapePrototype.hpp"

#include <vector>

class World
{
public:
	World()
	{
	}

	virtual ~World()
	{
	}

	void tick(long long timeMs)
	{
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
	GameContactListener contactListener;

	std::vector<SceneObject> objs;
	std::vector<SceneObject> newObjs;
};
