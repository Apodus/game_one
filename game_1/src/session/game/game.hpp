
#pragma once

#include "script/Scripter.hpp"
#include "util/logging.hpp"
#include "util/vec2.hpp"
#include "util/vec3.hpp"

#include "menu/MenuComponent.hpp"
#include "graphics/graphics.hpp"
#include "graphics/color.hpp"

#include "math/2d/mesh.hpp"
#include "math/2d/polygonTesselator.hpp"
#include "math/matrix/matrix4.hpp"

#include "logic/World.h"
#include "input/userio.hpp"

#include "worldmap/ProvinceGraph.hpp"

#include <cinttypes>
#include <memory>
#include <iterator>

class Game {

public:
	Game(std::shared_ptr<sa::UserIO>)
		: m_world()
	{
		graph.random();
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
		/*
		const auto& objs = m_world.GetObjects();
		for(const auto& obj : objs)
			obj.draw(pGraphics);
		*/

		const auto& provinces = graph.provinces();

		sa::TextureHandler::getSingleton().bindTexture(0, "Empty");

		for (const auto& province : provinces)
		{
			for (auto connectionIndex : province.m_connections)
			{
				pGraphics->m_pRenderer->drawLine(
					province.m_position,
					provinces[connectionIndex].m_position,
					0.1f,
					Color::GREEN
				);
			}
		}

		for (const auto& province : provinces)
		{
			float scale = 0.2f * province.area() / 100.f;

			sa::Matrix4 model;
			model.makeTranslationMatrix(province.m_position.x, province.m_position.y, 0);
			model.rotate(0, 0, 0, 1);
			model.scale(scale, scale, 1);
			
			pGraphics->m_pRenderer->drawRectangle(model, "Hero");
		}
	}

private:
	Scripter m_scripter;
	size_t m_tickID;
	World m_world;

	ProvinceGraph graph;
};