#pragma once

#include "session/game/troops/TroopReference.hpp"

#include <string>
#include <vector>
#include <cstdint>

#include <unordered_map>
#include <memory>

// Turn must be serializable. Therefore, TODO: troopreference pointers -> strings
class Turn
{
public:
	Turn() = default;

	std::unordered_map<uint32_t, std::vector<const TroopReference*>> recruitmentRequests;

	void addRecruitOrder(size_t provinceIndex, const TroopReference* troopReference)
	{
		recruitmentRequests[provinceIndex].emplace_back(troopReference);
	}

	void removeRecruitOrder(size_t provinceIndex, const TroopReference* troopReference)
	{
		auto& provinceRequests = recruitmentRequests[provinceIndex];
		for (size_t i = 0; i < provinceRequests.size(); ++i)
		{
			if (provinceRequests[i] == troopReference)
			{
				provinceRequests.erase(provinceRequests.begin() + i);
				return;
			}
		}
	}

	void clearRecruitmentOrders()
	{
		recruitmentRequests.clear();
	}
};

class Faction
{
public:
	Faction()
	{
		turn = std::make_shared<Turn>();
	}

	Faction(std::string name) : name(std::move(name))
	{
		turn = std::make_shared<Turn>();
	}

	void addRecruitOrder(size_t provinceIndex, const TroopReference* reference)
	{
		turn->addRecruitOrder(provinceIndex, reference);
	}

	void removeRecruitOrder(size_t provinceIndex, const TroopReference* reference)
	{
		turn->removeRecruitOrder(provinceIndex, reference);
	}

	int64_t currency = 200; // can go to negative
	std::vector<size_t> scienceResources;
	std::string name;

	std::shared_ptr<Turn> turn;
};