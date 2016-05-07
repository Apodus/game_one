
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
#include "input/userio.hpp"

#include <cinttypes>
#include <memory>
#include <iterator>

class Game {

public:
	Game(std::shared_ptr<sa::UserIO>)
	{
		m_world.AddEventAt(new SpawnEvent(SpawnEvent::Type::Thug), 3000);
		m_world.AddEventAt(new SpawnEvent(SpawnEvent::Type::Thug), 10000);
		m_world.AddEventAt(new SpawnEvent(SpawnEvent::Type::Thug), 5000);
		addLocalPlayer();
	}

	void addLocalPlayer()
	{
		m_world.AddEventAt(new SpawnEvent(SpawnEvent::Type::Hero), 1);
	}

	void preTick()
	{
		netEngine.Tick();
	}

	void postTick()
	{
		// TODO: Send local to remote
	}

	void tick(long long timeMs)
	{
		m_world.tick(timeMs);
	}

	void update(std::shared_ptr<sa::UserIO> userio)
	{
		auto& objs = m_world.GetObjects();
		for (auto&& obj : objs)
			obj.update(*userio);
	}

	void draw(std::shared_ptr<sa::Graphics> pGraphics)
	{
		auto& objs = m_world.GetObjects();
		for(auto&& obj : objs)
			obj.draw(pGraphics);
	}

private:
	Scripter m_scripter;
	size_t m_tickID;
	World m_world;
	net::Engine netEngine;
};