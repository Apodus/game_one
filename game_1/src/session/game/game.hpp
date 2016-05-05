
#pragma once

#include "script/Scripter.hpp"
#include "util/logging.hpp"
#include "util/vec2.hpp"

#include "menu/MenuComponent.hpp"
#include "graphics/graphics.hpp"
#include "graphics/color.hpp"

#include "math/2d/mesh.hpp"
#include "math/2d/polygonTesselator.hpp"
#include "math/matrix/matrix4.hpp"

#include "NetEngine.h"

#include "logic/World.h"
#include "logic/Timeline.h"
#include "input/userio.hpp"

#include "logic/ShapePrototype.hpp"

#include <cinttypes>
#include <memory>
#include <iterator>

class Game : public World {

public:
	Game(std::shared_ptr<sa::UserIO>) : m_timeline(*this)	
	{
		m_timeline.AddEvent(3000, new SpawnEvent());
		m_timeline.AddEvent(10000, new SpawnEvent());
		m_timeline.AddEvent(5000, new SpawnEvent());

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
			ShapePrototype sphereShape(Shape::makeCircle(0.5f, 32));
			auto heroBody = physicsWorld.CreateBody(&bodyDef);
			sphereShape.attach(heroBody, 4);
			
			objs.emplace_back(
				heroBody,
				sphereShape,
				"Hero"
			);
		}

		objs.back().inputHandler(
			std::make_unique<OnInput>([this](Transform& t, sa::UserIO& userio) {
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
					b2BodyDef bodyDef;
					bodyDef.angularDamping = 0.0f;
					bodyDef.linearDamping = 0.0f;
					bodyDef.gravityScale = 0.0f;
					bodyDef.type = b2BodyType::b2_dynamicBody;

					b2PolygonShape boxShape;
					boxShape.SetAsBox(0.25f / 2.0f, 0.25f / 2.0f);

					b2FixtureDef boxFixtureDef;
					boxFixtureDef.shape = &boxShape;
					boxFixtureDef.density = 4;

					auto world = t.m_body->GetWorld();
					{
						sa::vec2<float> forward = t.directionForward();
						auto heroBody = world->CreateBody(&bodyDef);
						heroBody->CreateFixture(&boxFixtureDef);
						heroBody->SetLinearVelocity({ forward.x * 10, forward.y * 10});
						heroBody->SetTransform({ t.position.x + forward.x, t.position.y + forward.y }, t.direction);
						
						newObjs.emplace_back(
							heroBody,
							Shape::makeBox(0.25f),
							"Hero"
						);
					}
				}

				if (userio.isKeyClicked('F'))
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

					auto world = t.m_body->GetWorld();
					{
						auto heroBody = world->CreateBody(&bodyDef);
						heroBody->CreateFixture(&boxFixtureDef);

						newObjs.emplace_back(
							heroBody,
							Shape::makeBox(1.0f),
							"Hero"
						);
					}
				}
			})
		);
		
	}

	void preTick()
	{
		netEngine.Tick();
	}

	void postTick()
	{
		// TODO: Send local to remote
	}

	void update(std::shared_ptr<sa::UserIO> userio)
	{
		for (auto&& obj : objs)
			obj.update(*userio);
	}

	void tick(long long timeMs)
	{
		m_timeline.Run(timeMs);
	}

	void draw(std::shared_ptr<sa::Graphics> pGraphics)
	{
		for(auto&& obj : objs)
			obj.draw(pGraphics);
	}

private:
	Scripter m_scripter;
	size_t m_tickID;

	Timeline m_timeline;
	net::Engine netEngine;
};