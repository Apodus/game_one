
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

#include "input/userio.hpp"

#include "worldmap/ProvinceGraph.hpp"

#include <cinttypes>
#include <memory>
#include <iterator>

class Game {

	void castSpells()
	{

	}

	void friendlyMovement()
	{

	}

	void offensiveMovement()
	{

	}

	void applyIncome()
	{
		const auto& provinces = graph.provinces();
		for (const auto& province : provinces)
		{
			if (province.m_owner != ~0ull)
			{
				// income from province
				players[province.m_owner].currency += province.income();

				// upkeep of troops
				for (const auto& commander : province.commanders)
				{
					players[province.m_owner].currency -= commander.upkeep;
				}
				for (const auto& troop : province.units)
				{
					players[province.m_owner].currency -= troop.upkeep;
				}
			}
		}
	}

public:
	Game(std::shared_ptr<sa::UserIO>)
	{
		graph.random();
	}

	void processTurn()
	{
		castSpells();
		friendlyMovement();
		offensiveMovement();
		applyIncome();
	}

	void preTick()
	{
	}

	void postTick()
	{
	}

	void tick(long long timeMs)
	{
	}

	void update(std::shared_ptr<sa::UserIO> userio)
	{
	}

	void draw(std::shared_ptr<sa::Graphics> pGraphics)
	{
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
	
	struct Faction
	{
		int64_t currency = 0; // can go to negative
		std::vector<size_t> resources;
	};

	std::vector<TroopReference> troopReferences;
	std::vector<Faction> players;
	ProvinceGraph graph;
};