
#pragma once

#include "script/Scripter.hpp"
#include "util/logging.hpp"
#include "util/vec2.hpp"
#include "util/vec3.hpp"

#include "graphics/graphics.hpp"
#include "menu/MenuComponent.hpp"
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

namespace bs { class BattleSim; }
namespace sa { class MenuRoot; }

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
	);

	virtual ~Game();

	void processTurn()
	{
		processRecruitments();
		castSpells();
		friendlyMovement();
		offensiveMovement();
		applyIncome();
	}

	void showBattle();

	void preTick()
	{
	}

	void postTick()
	{
	}

	void tick(long long timeMs);

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

	void draw(std::shared_ptr<sa::Graphics> pGraphics);
private:
	void drawProvinces(std::shared_ptr<sa::Graphics> pGraphics);
	void drawBattle(std::shared_ptr<sa::Graphics> pGraphics);

	Scripter m_scripter;
	size_t m_tickID;
	std::unique_ptr<bs::BattleSim> m_sim;
	uint64_t m_lastSimUpdate;

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
	std::shared_ptr<Hud> hud;
	
	std::unordered_map<std::string, TroopReference> troopReferences;
	std::vector<Faction> players;
	ProvinceGraph graph;
};