
#include "session/game/Faction.hpp"


void Turn::addMovementOrder(
	const std::vector<uint32_t>& commanderIds,
	uint32_t sourceProvince,
	uint32_t targetProvince
)
{
	auto& provinceOrders = commanderOrders[sourceProvince];
	for (uint32_t commanderId : commanderIds)
	{
		provinceOrders[commanderId].orderType = OrderType::Move;
		provinceOrders[commanderId].moveTo = targetProvince;
	}
}

void Turn::addRecruitOrder(size_t provinceIndex, const TroopReference* troopReference)
{
	recruitmentRequests[provinceIndex].emplace_back(troopReference);
}

void Turn::removeRecruitOrder(size_t provinceIndex, const TroopReference* troopReference)
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

void Turn::clearRecruitmentOrders()
{
	recruitmentRequests.clear();
}




Faction::Faction()
{
	turn = std::make_shared<Turn>();
}

Faction::Faction(std::string name) : name(std::move(name))
{
	turn = std::make_shared<Turn>();
}

void Faction::addRecruitOrder(size_t provinceIndex, const TroopReference* reference)
{
	turn->addRecruitOrder(provinceIndex, reference);
}

void Faction::removeRecruitOrder(size_t provinceIndex, const TroopReference* reference)
{
	turn->removeRecruitOrder(provinceIndex, reference);
}