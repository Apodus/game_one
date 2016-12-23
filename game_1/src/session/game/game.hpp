
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
#include "multiplayer/GameSession.h"

#include <cinttypes>
#include <memory>
#include <iterator>

class Game {

public:
	Game(std::shared_ptr<sa::UserIO>)
		: m_world()
	{
	}

	void preTick()
	{
	}

	void postTick()
	{
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
	Scripter m_scripter;
	size_t m_tickID;
	World m_world;
};