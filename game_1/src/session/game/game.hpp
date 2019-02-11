
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
#include "session/game/Camera.h"

#include "session/game/Faction.hpp"
#include "CombatView.hpp"

#include <atomic>
#include <cinttypes>
#include <memory>
#include <iterator>
#include <unordered_map>
#include <string>

namespace sa { class MenuRoot; }

class Game {

	void castSpells()
	{

	}

	void reallocateTroopsFromTurn() {
		for (const auto& player : players) {
			for (const auto& entry : player.turn->m_troopReassignments) {
				uint32_t sourceProvince = entry.first;
				for (const auto& troopEntry : entry.second) {
					uint32_t unitId = troopEntry.first;
					uint32_t newCommander = troopEntry.second;

					for (auto& troop : graph.provinces()[sourceProvince].units) {
						if (troop.id == unitId && troop.owner == player.playerIndex) {
							
							for (BattleCommander& commander : graph.provinces()[sourceProvince].commanders) {
								
								// remove from previous commander
								if (commander.id == troop.commanderId) {
									auto& squad = commander.squad;
									for (auto it = squad.unitIds.begin(); it != squad.unitIds.end(); ++it) {
										if (*it == unitId) {
											squad.unitIds.erase(it++);
											--it;
										}
									}
								}

								// assign to new commander
								if (commander.id == newCommander) {
									commander.squad.unitIds.emplace_back(unitId);
									troop.commanderId = commander.id;
								}

							}
						}
					}
				}
			}
		}
	}

	void updateCommanderOrdersFromTurn()
	{
		for (const auto& player : players)
		{
			for (const auto& entry : player.turn->commanderOrders)
			{
				uint32_t sourceProvince = entry.first;
				for (const auto& order : entry.second)
				{
					uint32_t unitId = order.first;

					for (BattleCommander& commander : graph.provinces()[sourceProvince].commanders)
					{
						if (commander.id == unitId)
						{
							commander.strategyOrder = order.second;
							break;
						}
					}
				}
			}
		}
	}

	struct MovementBin
	{
		struct Move
		{
			Move() = default;
			Move(size_t provinceId, size_t commanderId) : provinceId(provinceId), commanderId(commanderId) {}
			size_t provinceId;
			size_t commanderId;
		};

		MovementBin() = default;
		MovementBin(size_t playerId, size_t targetProvinceIndex)
			: targetProvinceIndex(targetProvinceIndex)
			, playerId(playerId)
		{}

		std::vector<Move> moves;
		size_t targetProvinceIndex;
		size_t playerId;
	};

	std::vector<MovementBin> movementBins(bool friendlyMovement)
	{
		std::vector<MovementBin> bins;

		auto& provinces = graph.provinces();
		for (auto& province : provinces)
		{
			for (size_t i = 0; i < province.commanders.size(); ++i)
			{
				auto& commander = province.commanders[i];
				if (commander.strategyOrder.orderType == OrderType::Move)
				{
					if ((commander.strategyOrder.moveTo == province.m_index) != friendlyMovement)
						continue;

					size_t targetProvince = commander.strategyOrder.moveTo;

					bool binFound = false;
					for (auto& bin : bins)
					{
						bool correctOwner = bin.playerId == commander.owner;
						bool correctTarget = bin.targetProvinceIndex == targetProvince;
						if (correctOwner && correctTarget)
						{
							// bin already exists, use that.
							bin.moves.emplace_back(province.m_index, commander.id);
							binFound = true;
							break;
						}
					}

					if (!binFound)
					{
						// oh no! need to create a new bin.
						bins.emplace_back(commander.owner, targetProvince);
						bins.back().moves.emplace_back(province.m_index, commander.id);
					}
				}
			}
		}

		return std::move(bins);
	}

	void resolveCombat(size_t provinceIndex);

	void applyMovement(bool isFriendlyMovement)
	{
		// TODO: randomize order of elements in bins
		auto bins = movementBins(isFriendlyMovement);
		auto& provinces = graph.provinces();

		for (auto& bin : bins)
		{
			for (auto& move : bin.moves)
			{
				if (move.provinceId == bin.targetProvinceIndex)
					continue;

				auto& commanders = provinces[move.provinceId].commanders;
				for (size_t i = 0; i < commanders.size(); ++i)
				{
					if (commanders[i].id == move.commanderId)
					{
						// apply move
						provinces[bin.targetProvinceIndex].commanders.emplace_back(commanders[i]);
						auto& squad = commanders[i].squad;
						{
							// TODO
						}

						provinces[move.provinceId].commanders.erase(commanders.begin() + i);
						break;
					}
				}
			}

			if (!isFriendlyMovement)
			{
				resolveCombat(bin.targetProvinceIndex);
			}
		}
	}

	void friendlyMovement()
	{
		applyMovement(true);
	}

	void offensiveMovement()
	{
		applyMovement(false);
	}

	void processRecruitments()
	{
		auto& provinces = graph.provinces();
		for (auto& player : players)
		{
			for (const auto& ordersForProvince : player.turn->recruitmentRequests)
			{
				for (const auto* order : ordersForProvince.second)
				{
					player.currency -= order->goldCost;
					if (order->isCommander)
					{
						provinces[ordersForProvince.first].commanders.emplace_back(
							*order,
							++nextUnitId,
							provinces[ordersForProvince.first].m_owner
						);
					}
					else
					{
						provinces[ordersForProvince.first].units.emplace_back(
							*order,
							++nextUnitId,
							provinces[ordersForProvince.first].m_owner
						);
					}
				}
			}

			player.turn->recruitmentRequests.clear();
		}
	}

	void applyIncome()
	{
		const auto& provinces = graph.provinces();
		for (const auto& province : provinces)
		{
			if (province.m_owner != ~size_t(0))
			{
				// income from province
				players[province.m_owner].currency += province.income();

				// upkeep of troops
				for (const auto& commander : province.commanders)
				{
					players[province.m_owner].currency -= commander.reference->upkeep;
				}
				for (const auto& troop : province.units)
				{
					players[province.m_owner].currency -= troop.reference->upkeep;
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
		reallocateTroopsFromTurn();
		updateCommanderOrdersFromTurn();
		processRecruitments();
		castSpells();
		friendlyMovement();
		offensiveMovement();
		applyIncome();

		hud->newTurn();
	}

	void toggleBattle();

	void preTick()
	{
	}

	void postTick()
	{
	}

	void tick(long long timeMs);

	inline sa::vec2<float> mouseToWorld(sa::vec3<float> mousePos)
	{
		return sa::vec2<float>(m_activeCamera->pos.x + mousePos.x * m_activeCamera->pos.z * 0.985f, m_activeCamera->pos.y + (mousePos.y * m_activeCamera->pos.z * 0.985f) / (aspectRatio + 0.0000001f));
	}

	sa::vec3<float> getCameraPosition() const
	{
		return m_activeCamera->pos;
	}

	const std::vector<ProvinceGraph::Province>& provinces() const
	{
		return graph.provinces();
	}

	void update(std::shared_ptr<sa::UserIO> userio, float aspectRatio)
	{
		this->aspectRatio = aspectRatio;
		for (auto& province : graph.provinces()) {
			province.updatevisual();
		}

		mousePos = userio->getCursorPosition();
		auto worldPos = mouseToWorld(mousePos);

		int mouseKeyCode = userio->getMouseKeyCode(0);
		auto* nearest = graph.getNearestProvince(worldPos);
		if ((nearest->m_position - worldPos).lengthSquared() > 1.5f * 1.5f)
			nearest = nullptr;

		if (userio->isKeyReleased(mouseKeyCode))
			m_mapDragActionActive = false;

		// move camera if map drag input action was started for me.
		if (m_mapDragActionActive && userio->isKeyDown(mouseKeyCode))
		{
			auto modifier = (mousePosPrev - mousePos) * m_activeCamera->pos.z;
			modifier.y /= aspectRatio;
			m_activeCamera->target += modifier;
		}
		mousePosPrev = mousePos;

		// if input not meant for us, don't check inputs.
		if (hud->capture(mousePos))
			return;

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

			float scroll = userio->getMouseScroll();
			if (scroll != 0)
			{
				if (scroll > 0)
					m_activeCamera->target.z *= 1.0f - scroll * 0.1f;
				else
					m_activeCamera->target.z *= 1.0f - 0.1f / scroll;
			}
		}

		{
			if (userio->isKeyPressed(mouseKeyCode))
				m_mapDragActionActive = true;
		}
	}

	const TroopReference* troopReference(const std::string& name) const
	{
		auto it = troopReferences.find(name);
		if (it == troopReferences.end())
			return nullptr;
		return &(it->second);
	}

	const Faction& getPlayer(size_t player) const {
		return players[player];
	}

	void draw(std::shared_ptr<sa::Graphics> pGraphics);
private:
	friend class Hud;

	void drawProvinces(std::shared_ptr<sa::Graphics> pGraphics);

	Scripter m_scripter;

	bool m_mapDragActionActive = false;
	size_t m_tickID = 0;

	sa::vec3<float> mousePos;
	sa::vec3<float> mousePosPrev;

	float aspectRatio = 16.0f / 9.0f;

	std::atomic<size_t> nextUnitId; // starts at 1, zero is reserved for invalid id.
	std::shared_ptr<sa::MenuRoot> menuRootNode;
	std::shared_ptr<Hud> hud;

	std::unordered_map<std::string, TroopReference> troopReferences;
	std::vector<Faction> players;
	ProvinceGraph graph;

	bool m_showCombat = false;
	CombatView m_combatView;
	Camera m_provinceCamera;
	Camera* m_activeCamera;

};