
#include "WorldEvent.h"

#include "Box2D/Box2D.h"
#include "World.h"
#include "math/2d/shape.hpp"
#include "logic/ShapePrototype.hpp"

SpawnEvent::SpawnEvent(Type t, float x, float y, float dir, float vx, float vy)
	:
	myType(t),
	mySpawnPos(x, y),
	mySpawnDir(dir),
	mySpawnVelocity(vx, vy)
{

}

void SpawnEvent::Serialize(std::vector<unsigned char>& data)
{
	data.emplace_back(static_cast<unsigned char>(myType));
	/*data.emplace_back(mySpawnPos.x);
	data.emplace_back(mySpawnPos.y);
	data.emplace_back(mySpawnDir);
	data.emplace_back(mySpawnVelocity.x);
	data.emplace_back(mySpawnVelocity.y);*/
}

void SpawnEvent::Deserialize(net::InputStream& aStream)
{
	myType = static_cast<SpawnEvent::Type>(aStream.ReadU8());
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

	auto body = aWorld.physicsWorld.CreateBody(&bodyDef);
	if (myType == SpawnEvent::Type::Hero)
	{
		ShapePrototype sphereShape(Shape::makeCircle(0.5f, 32));
		sphereShape.attach(body, 4);

		aWorld.newObjs.emplace_back(
			body,
			sphereShape,
			"Hero"
		);

		// TODO: Don't add this to remote heroes.
		aWorld.newObjs.back().inputHandler(
			std::make_unique<OnInput>([this, &aWorld](ObjectController& o, sa::UserIO& userio) {

			if (userio.isKeyDown('A'))
			{
				o.setTurning(1.0f);
			}
			else if (userio.isKeyDown('D'))
			{
				o.setTurning(-1.0f);
			}
			else
			{
				o.setTurning(0);
			}

			if (userio.isKeyDown('W'))
			{
				o.setForwardSpeed(1.0f);
			}
			else if (userio.isKeyDown('S'))
			{
				o.setForwardSpeed(-1.0f);
			}
			else
			{
				o.setForwardSpeed(0);
			}
			
			if (userio.isKeyClicked(' '))
			{
				// TODO: Probably should replace spawn event with trigger event. 
				// I.e. instead of replicating object spawn, replicate user key press
				const int DelayMs = 0;
				sa::vec2<float> forward = o.getTransform().directionForward();			
				auto se = new SpawnEvent(SpawnEvent::Type::Bullet,
					o.getTransform().position.x + forward.x, o.getTransform().position.y + forward.y, // Position
					o.getTransform().direction, // Direction
					forward.x * 10, forward.y * 10); // Velocity
				aWorld.AddEvent(se, DelayMs);
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
		body->CreateFixture(&boxFixtureDef);

		aWorld.newObjs.emplace_back(
			body,
			Shape::makeBox(myType == SpawnEvent::Type::Bullet ? 0.25f : 1.0f),
			"Hero"
		);
	}
	body->SetTransform({ mySpawnPos.x, mySpawnPos.y }, mySpawnDir);
	body->SetLinearVelocity({ mySpawnVelocity.x, mySpawnVelocity.y });
}

void SpawnEvent::End(World& world)
{
	world.physicsWorld.DestroyBody(myObj->getTransform().m_body);
}