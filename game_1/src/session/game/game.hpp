
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
#include "session/game/hud.hpp"

#include <atomic>
#include <cinttypes>
#include <memory>
#include <iterator>
#include <unordered_map>
#include <string>

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

	void processRecruitments()
	{
		for (auto& player : players)
		{
			for (auto& recruitment : player.trainings)
			{
				auto& province = graph.provinces()[recruitment.provinceIndex];
				for (size_t troopIndex : recruitment.unitsToTrainIndices)
				{
					auto it = troopReferences.find(province.troopsToRecruit[troopIndex]);
					province.units.emplace_back(it->second, ++nextUnitId, province.m_owner);
				}

				player.currency -= recruitment.currencyCost;
			}

			player.trainings.clear();
		}
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
	Game(
		std::shared_ptr<sa::MenuRoot> menuRootNode,
		std::shared_ptr<sa::UserIO>
	)
		: nextUnitId(1)
		, menuRootNode(menuRootNode)
		, hud(menuRootNode.get(), "GameHud", sa::vec3<float>(), sa::vec3<float>(1, 1, 1))
	{
		TroopReference militia;
		militia.accuracy = 10;
		militia.armor = 2;
		militia.attack = 8;
		militia.defence = 8;
		militia.fitness = 1;
		militia.hp = 8;
		militia.leadership = 0;
		militia.name = "militia";
		militia.strength = 8;

		TroopReference zealot;
		zealot.accuracy = 10;
		zealot.armor = 5;
		zealot.attack = 11;
		zealot.defence = 11;
		zealot.fitness = 3;
		zealot.hp = 10;
		zealot.leadership = 30; // if promoted to commander by some means, this would take effect.
		zealot.name = "zealot";
		zealot.strength = 10;

		TroopReference marksman;
		marksman.accuracy = 13;
		marksman.armor = 1;
		marksman.attack = 8;
		marksman.defence = 8;
		marksman.fitness = 3;
		marksman.hp = 10;
		marksman.leadership = 10;
		marksman.name = "marksman";
		marksman.strength = 9;

		TroopReference rider;
		rider.accuracy = 10;
		rider.armor = 3;
		rider.attack = 12;
		rider.defence = 9;
		rider.fitness = 3;
		rider.hp = 10;
		rider.leadership = 30;
		rider.name = "rider";
		rider.strength = 10;

		troopReferences.insert(std::make_pair(militia.name, militia));
		troopReferences.insert(std::make_pair(zealot.name, zealot));
		troopReferences.insert(std::make_pair(marksman.name, marksman));
		troopReferences.insert(std::make_pair(rider.name, rider));

		graph.random();
		auto& provinces = graph.provinces();

		for (auto& province : provinces)
		{
			province.troopsToRecruit.emplace_back(militia.name);
		}

		provinces.front().m_owner = 0;
		provinces.back().m_owner = 1;

		players.emplace_back("Mestari");
		players.emplace_back("Nubu");
	}

	void processTurn()
	{
		processRecruitments();
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

	void update(std::shared_ptr<sa::UserIO> userio, float aspectRatio)
	{
		this->aspectRatio = aspectRatio;

		mousePos = userio->getCursorPosition();
		{
			int mouseKeyCode = userio->getMouseKeyCode(0);
			if (userio->isKeyClicked(mouseKeyCode))
			{
				LOG("clicked mouse 0, %f %f", mousePos.x, mousePos.y);
				
			}
		}

		{
			int mouseKeyCode = userio->getMouseKeyCode(1);
			if (userio->isKeyClicked(mouseKeyCode))
			{
				LOG("clicked mouse 1");
			}
		}
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
			float scale = 0.4f * province.area() / 100.f;

			sa::Matrix4 model;
			model.makeTranslationMatrix(province.m_position.x, province.m_position.y, 0);
			model.rotate(0, 0, 0, 1);
			model.scale(scale, scale, 1);
			
			sa::vec4<float> color(0.3f, 0.3f, 0.3f, 1);
			if (province.m_owner == 0)
			{
				color.r = 1;
				color.g = 0;
				color.b = 0;
			}
			if (province.m_owner == 1)
			{
				color.r = 1;
				color.g = 1;
				color.b = 1;
			}
			pGraphics->m_pRenderer->drawRectangle(model, "Frame", color);
		}

		sa::Matrix4 model;
		model.makeTranslationMatrix(mousePos.x * 20 * 0.985f, mousePos.y * 20 * 0.985f / aspectRatio, 0);
		model.rotate(0, 0, 0, 1);
		model.scale(0.1f, 0.1f, 1);
		pGraphics->m_pRenderer->drawRectangle(model, "Hero");
	}

private:
	Scripter m_scripter;
	size_t m_tickID;

	struct Faction
	{
		Faction() = default;
		Faction(std::string name) : name(std::move(name))
		{
		}

		int64_t currency = 0; // can go to negative
		std::vector<size_t> scienceResources;
		std::string name;

		struct TrainTroopOrder
		{
			size_t provinceIndex = 0;
			size_t resourceCost = 0; // keep track of total resource costs, not allowed to surpass province output.
			size_t currencyCost = 0; // keep track of currency costs.
			std::vector<size_t> unitsToTrainIndices;
		};

		std::vector<TrainTroopOrder> trainings;
	};

	sa::vec3<float> mousePos;
	float aspectRatio = 16.0f / 9.0f;

	std::atomic<size_t> nextUnitId; // starts at 1, zero is reserved for invalid id.
	std::shared_ptr<sa::MenuRoot> menuRootNode;
	Hud hud;
	std::unordered_map<std::string, TroopReference> troopReferences;
	std::vector<Faction> players;
	ProvinceGraph graph;
};