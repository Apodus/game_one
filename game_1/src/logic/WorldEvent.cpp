
#include "WorldEvent.h"

#include "Box2D/Box2D.h"
#include "World.h"
#include "math/2d/shape.hpp"
#include "logic/ShapePrototype.hpp"
#include "NetOutputStream.h"

SpawnEvent::SpawnEvent(Type t, float x, float y, float dir, float vx, float vy)
	:
	myType(t),
	mySpawnPos(x, y),
	mySpawnDir(dir),
	mySpawnVelocity(vx, vy)
{

}

void SpawnEvent::Serialize(net::OutputStream& aStream)
{
	aStream.WriteU8(static_cast<uint8_t>(myType));
	aStream.WriteU32(mySpawnPos.x);
	aStream.WriteU32(mySpawnPos.y);
	aStream.WriteU32(mySpawnDir);
	aStream.WriteU32(mySpawnVelocity.x);
	aStream.WriteU32(mySpawnVelocity.y);
}

void SpawnEvent::Deserialize(net::InputStream& aStream)
{
	myType = static_cast<SpawnEvent::Type>(aStream.ReadU8());
	mySpawnPos.x = aStream.ReadU32<float>();
	mySpawnPos.y = aStream.ReadU32<float>();
	mySpawnDir = aStream.ReadU32<float>();
	mySpawnVelocity.x = aStream.ReadU32<float>();
	mySpawnVelocity.y = aStream.ReadU32<float>();
}

void SpawnEvent::Begin(World& aWorld)
{
	b2BodyDef bodyDef;
	bodyDef.gravityScale = 0.0f;
	bodyDef.type = b2BodyType::b2_dynamicBody;
	bodyDef.angularDamping = 5;
	bodyDef.linearDamping = 5;

	if (myType == SpawnEvent::Type::Hero)
	{
		auto& obj = aWorld.createObject(bodyDef, ShapePrototype(Shape::makeCircle(0.5f, 32)), "Hero")
			.teleportTo(mySpawnPos)
			.setVelocity(mySpawnVelocity);

		// TODO, define input handling somewhere else
		obj.inputHandler(
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
		float boxEdgeLength = 0;
		if (myType == SpawnEvent::Type::Bullet)
		{
			bodyDef.linearDamping = 0;
			boxEdgeLength = 0.25f;
		}
		else
			boxEdgeLength = 1.0f;

		aWorld.createObject(bodyDef, ShapePrototype(Shape::makeBox(boxEdgeLength)), "Hero")
			.teleportTo(mySpawnPos)
			.setVelocity(mySpawnVelocity);
	}
}

void SpawnEvent::End(World& world)
{
	if(myObj)
		world.destroyObject(myObj);
}