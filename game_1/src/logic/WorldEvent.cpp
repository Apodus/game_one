
#include "WorldEvent.h"

#include "Box2D/Box2D.h"
#include "World.h"
#include "math/2d/shape.hpp"
#include "logic/ShapePrototype.hpp"


SpawnEvent::SpawnEvent(Type t, float x, float y, float dir, float vx, float vy)
	:
	myType(t),
	myBody(nullptr),
	mySpawnPos(x, y),
	mySpawnDir(dir),
	mySpawnVelocity(vx, vy)
{

}

void SpawnEvent::Begin(World& aWorld)
{
	b2BodyDef bodyDef;
	bodyDef.gravityScale = 0.0f;
	bodyDef.type = b2BodyType::b2_dynamicBody;

	b2PolygonShape boxShape;
	if (myType == SpawnEvent::Type::Bullet)
	{
		boxShape.SetAsBox(0.25f / 2.0f, 0.25f / 2.0f);
		bodyDef.angularDamping = 0.0f;
		bodyDef.linearDamping = 0.0f;
	}
	else
	{
		boxShape.SetAsBox(1.0f / 2.0f, 1.0f / 2.0f);
		bodyDef.angularDamping = 5.0f;
		bodyDef.linearDamping = 5.0f;
	}

	b2FixtureDef boxFixtureDef;
	boxFixtureDef.shape = &boxShape;
	boxFixtureDef.density = 4;

	myBody = aWorld.physicsWorld.CreateBody(&bodyDef);
	if (myType == SpawnEvent::Type::Hero)
	{
		ShapePrototype sphereShape(Shape::makeCircle(0.5f, 32));
		sphereShape.attach(myBody, 4);

		aWorld.newObjs.emplace_back(
			myBody,
			sphereShape,
			"Hero"
		);

		// TODO: Don't add this to remote heroes.
		aWorld.newObjs.back().inputHandler(
			std::make_unique<OnInput>([this, &aWorld](Transform& t, sa::UserIO& userio) {
			if (userio.isKeyDown('A'))
			{
				t.m_body->ApplyTorque(10.0f, false);
			}
			if (userio.isKeyDown('D'))
			{
				t.m_body->ApplyTorque(-10.0f, false);
			}
			if (userio.isKeyDown('W'))
			{
				t.m_body->ApplyForceToCenter({
					+sa::math::sin(t.direction) * 200.1f,
					-sa::math::cos(t.direction) * 200.1f
				},
					true
				);
			}
			if (userio.isKeyDown('S'))
			{
				t.m_body->ApplyForceToCenter({
					-sa::math::sin(t.direction) * 20.0f,
					+sa::math::cos(t.direction) * 20.0f
				},
					true
				);
			}
			
			if (userio.isKeyClicked(' '))
			{
				// TODO: Probably should replace spawn event with trigger event. 
				// I.e. instead of replicating object spawn, replicate user key press
				const int DelayMs = 0;
				sa::vec2<float> forward = t.directionForward();			
				aWorld.AddEvent(new SpawnEvent(SpawnEvent::Type::Bullet,
					t.position.x + forward.x, t.position.y + forward.y, // Position
					t.direction, // Direction
					forward.x * 10, forward.y * 10),
					DelayMs); // Velocity
			}
			if (userio.isKeyClicked('F'))
			{
				aWorld.AddEvent(new SpawnEvent(SpawnEvent::Type::Thug));
			}
		})
		);
	}
	else
	{
		myBody->CreateFixture(&boxFixtureDef);

		aWorld.newObjs.emplace_back(
			myBody,
			Shape::makeBox(myType == SpawnEvent::Type::Bullet ? 0.25f : 1.0f),
			"Hero"
		);
	}
	myBody->SetTransform({ mySpawnPos.x, mySpawnPos.y }, mySpawnDir);
	myBody->SetLinearVelocity({ mySpawnVelocity.x, mySpawnVelocity.y });
}

void SpawnEvent::End(World& world)
{
	world.physicsWorld.DestroyBody(myBody);
	myBody = nullptr;
}