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

private:
	std::vector<BattleCommander> m_commanders;

	std::unordered_map<size_t, size_t> m_ownerToIndex;
};
