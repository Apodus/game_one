#pragma once

#include "session/game/troops/TroopReference.hpp"
#include "session/game/troops/StrategyViewOrder.hpp"

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
};

class Faction
{
public:
	Faction();
	Faction(std::string name);
	void addRecruitOrder(size_t provinceIndex, const TroopReference* reference);
	void removeRecruitOrder(size_t provinceIndex, const TroopReference* reference);

	int64_t currency = 200; // can go to negative
	std::vector<size_t> scienceResources;
	std::string name;

	std::shared_ptr<Turn> turn;
};