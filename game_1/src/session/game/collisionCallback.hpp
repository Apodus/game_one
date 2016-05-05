
#include "Box2D/Box2D.h"

// Note
/*
/// Implement this class to provide collision filtering. In other words, you can implement
/// this class if you want finer control over contact creation.
class b2ContactFilter
{
public:
virtual ~b2ContactFilter() {}

/// Return true if contact calculations should be performed between these two shapes.
/// @warning for performance reasons this is only called when the AABBs begin to overlap.
virtual bool ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB);
};
*/

class GameContactListener : public b2ContactListener
{
	virtual void BeginContact(b2Contact* contact) override {
		void* bodyUserData1 = contact->GetFixtureA()->GetBody()->GetUserData();
		void* bodyUserData2 = contact->GetFixtureB()->GetBody()->GetUserData();
	}

	virtual void EndContact(b2Contact* contact) override {
		void* bodyUserData1 = contact->GetFixtureA()->GetBody()->GetUserData();
		void* bodyUserData2 = contact->GetFixtureB()->GetBody()->GetUserData();
	}

	virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override {
		// does not seem very useful.
	}

	virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override {
		// reactions based on impulses
	}
};