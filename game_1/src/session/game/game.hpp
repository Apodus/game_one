
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
#include "multiplayer/GameStateAdapter.h"

#include <cinttypes>
#include <memory>
#include <iterator>

class Game {

public:
	Game(std::shared_ptr<sa::UserIO>)
		:
		m_world(m_netWorldEventAdapter)
		
	{
		m_netEngine.AddAdapter(0, 33, m_netWorldEventAdapter);
		m_netEngine.AddAdapter(0, 22, m_netGameStateAdapter);
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
		m_netEngine.PreTick();
		auto events = m_netWorldEventAdapter.GetReceivedEvents();
		for (size_t i = 0; i < events.size(); i++)
		{
			auto& e = events[i];
			m_world.AddEvent(e.serializable, 0, false);
		}
	}

	void postTick()
	{
		m_netEngine.PostTick();
	}

	void tick(long long timeMs)
	{		
		m_world.tick(timeMs);
	}

	void update(std::shared_ptr<sa::UserIO> userio)
	{
		m_world.update(*userio);
	}

	void draw(std::shared_ptr<sa::Graphics> pGraphics)
	{
		const auto& objs = m_world.GetObjects();
		for(const auto& obj : objs)
			obj.draw(pGraphics);
	}

private:
	// Net
	net::Engine m_netEngine;
	WorldEventAdapter m_netWorldEventAdapter;
	GameStateAdapter m_netGameStateAdapter;
	
	Scripter m_scripter;
	size_t m_tickID;
	World m_world;
};