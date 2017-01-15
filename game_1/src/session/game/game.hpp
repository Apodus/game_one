
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

	inline sa::vec2<float> mouseToWorld(sa::vec3<float> mousePos)
	{
		return sa::vec2<float>(cameraPosition.x + mousePos.x * cameraPosition.z * 0.985f, cameraPosition.y + (mousePos.y * cameraPosition.z * 0.985f) / aspectRatio);
	}

	sa::vec3<float> getCameraPosition() const
	{
		return cameraPosition;
	}

	void update(std::shared_ptr<sa::UserIO> userio, float aspectRatio)
	{
		this->aspectRatio = aspectRatio;

		mousePos = userio->getCursorPosition();
		auto worldPos = mouseToWorld(mousePos);

		int mouseKeyCode = userio->getMouseKeyCode(0);
		auto* nearest = graph.getNearestProvince(worldPos);
		if ((nearest->m_position - worldPos).lengthSquared() > 1.5f * 1.5f)
			nearest = nullptr;

		for (auto& province : graph.provinces())
		{
			province.updatevisual();
		}

		{
			if (nearest)
			{
				nearest->updatevisual(1.3f, 2.0f);

				int mouseKeyCodePrimary = userio->getMouseKeyCode(0);
				if (userio->isKeyClicked(mouseKeyCodePrimary) && !userio->isKeyConsumed(mouseKeyCodePrimary))
				{
					hud->selectProvince(nearest);
				}

				int mouseKeyCodeSecondary = userio->getMouseKeyCode(1);
				if (userio->isKeyClicked(mouseKeyCodeSecondary) && !userio->isKeyConsumed(mouseKeyCodeSecondary))
				{
					hud->orderToProvince(nearest);
				}
			}

			// move camera
			if (userio->isKeyDown(mouseKeyCode))
			{
				auto modifier = (mousePosPrev - mousePos) * cameraPosition.z;
				modifier.y /= aspectRatio;
				targetCameraPosition += modifier;
			}

			float scroll = userio->getMouseScroll();
			if (scroll != 0)
			{
				if (scroll > 0)
					targetCameraPosition.z *= 1.0f - scroll * 0.1f;
				else
					targetCameraPosition.z *= 1.0f - 0.1f / scroll;
			}
		}

		{
			int mouseKeyCode = userio->getMouseKeyCode(1);
			if (userio->isKeyPressed(mouseKeyCode))
			{
				LOG("clicked mouse 1");
			}
		}

		mousePosPrev = mousePos;
	}

	const TroopReference* troopReference(const std::string& name) const
	{
		auto it = troopReferences.find(name);
		if (it == troopReferences.end())
			return nullptr;
		return &(it->second);
	}

	int64_t gold(size_t player) const {
		return players[player].currency;
	}

	void draw(std::shared_ptr<sa::Graphics> pGraphics);
private:
	void drawProvinces(std::shared_ptr<sa::Graphics> pGraphics);
	void drawBattle(std::shared_ptr<sa::Graphics> pGraphics);

	Scripter m_scripter;
	size_t m_tickID = 0;

	struct Faction
	{
		Faction() = default;
		Faction(std::string name) : name(std::move(name))
		{
		}

		int64_t currency = 200; // can go to negative
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
	sa::vec3<float> mousePosPrev;
	
	sa::vec3<float> cameraPosition;
	sa::vec3<float> targetCameraPosition;
	float aspectRatio = 16.0f / 9.0f;

	std::atomic<size_t> nextUnitId; // starts at 1, zero is reserved for invalid id.
	std::shared_ptr<sa::MenuRoot> menuRootNode;
	std::shared_ptr<Hud> hud;
	
	std::unordered_map<std::string, TroopReference> troopReferences;
	std::vector<Faction> players;
	ProvinceGraph graph;

	// Battle simulation
	struct Unit
	{
		struct Status
		{
			Status() : isValid(false) {}
			float x;
			float y;
			float z;
			bool isValid;
		};
		Status current;
		Status next;
		float size;
		int hitpoints;
		int team;
	};
	std::unique_ptr<bs::BattleSim> m_sim;
	uint64_t m_lastSimUpdate;
	double m_simAccu;
	std::vector<Unit> m_units;
};