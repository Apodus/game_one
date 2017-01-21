#pragma once

#include "BattleSim/Battle.h"
#include "session/game/troops/Commander.hpp"

#include <unordered_map>

// Combat to bs::Battle adapter
class Combat
{
public:
	Combat();

	void add(const std::vector<BattleCommander>& commanders);

	void resolve();
	
	void setup(bs::Battle& battle);

	size_t factionCount() const { return m_ownerToIndex.size(); }

	struct Status
	{
		uint16_t hitpoints;
	};

	const Status& getPostBattleCommander(size_t index) const { return getPostBattleStatus(index); }

	const Status& getPostBattleTroop(size_t index) const { return getPostBattleStatus(index + m_commanders.size()); }

private:
	const Status& getPostBattleStatus(size_t index) const { return m_status[index]; }

	std::vector<BattleCommander> m_commanders;

	// Status of units after battle
	std::vector<Status> m_status;

	std::unordered_map<size_t, size_t> m_ownerToIndex;
};
