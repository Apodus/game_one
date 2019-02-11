#pragma once

#include "session/game/troops/TroopReference.hpp"
#include "session/game/troops/StrategyViewOrder.hpp"
#include "session/game/Combat.hpp"

#include <string>
#include <vector>
#include <cstdint>

#include <unordered_map>
#include <memory>

// Turn must be serializable. Therefore, TODO: troopreference pointers -> strings, or indices, or something.
class Turn
{
public:
	Turn() = default;

	std::unordered_map<uint32_t, std::vector<const TroopReference*>> recruitmentRequests;
	std::unordered_map<uint32_t, std::unordered_map<uint32_t, StrategyViewOrder>> commanderOrders;

	void addMovementOrder(const std::vector<uint32_t>& commanderIds, uint32_t sourceProvince, uint32_t targetProvince);
	void addRecruitOrder(size_t provinceIndex, const TroopReference* troopReference);
	void removeRecruitOrder(size_t provinceIndex, const TroopReference* troopReference);
	void clearRecruitmentOrders();
	void reassignTroopCommander(size_t provinceIndex, std::vector<uint32_t> troopIndices, size_t targetCommander);

	std::unordered_map <uint32_t, std::unordered_map<uint32_t, uint32_t>> m_troopReassignments;
	std::vector<Combat> m_combats;
};

class Faction
{
public:
	Faction();
	Faction(std::string name, size_t index);
	void addRecruitOrder(size_t provinceIndex, const TroopReference* reference);
	void removeRecruitOrder(size_t provinceIndex, const TroopReference* reference);

	size_t playerIndex = 0;
	int64_t currency = 200; // can go to negative
	std::vector<size_t> scienceResources;
	std::string name;

	std::shared_ptr<Turn> turn;
};