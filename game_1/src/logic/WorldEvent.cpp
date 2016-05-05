
#include "WorldEvent.h"

#include "Box2D/Box2D.h"
#include "World.h"
#include "math/2d/shape.hpp"

void SpawnEvent::Begin(World& world)
{
	b2BodyDef bodyDef;
	bodyDef.angularDamping = 5.0f;
	bodyDef.linearDamping = 5.0f;
	bodyDef.gravityScale = 0.0f;
	bodyDef.type = b2BodyType::b2_dynamicBody;

	b2PolygonShape boxShape;
	boxShape.SetAsBox(1.0f / 2.0f, 1.0f / 2.0f);

	b2FixtureDef boxFixtureDef;
	boxFixtureDef.shape = &boxShape;
	boxFixtureDef.density = 4;

	{
		myBody = world.physicsWorld.CreateBody(&bodyDef);
		myBody->CreateFixture(&boxFixtureDef);

		world.objs.emplace_back(
			myBody,
			Shape::makeBox(1.0f),
			"Hero"
		);
	}
}

void SpawnEvent::End(World& world)
{
	world.physicsWorld.DestroyBody(myBody);
	myBody = nullptr;
}