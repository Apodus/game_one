#pragma once

#include "session/game/troops/TroopReference.hpp"
#include "session/game/troops/StrategyViewOrder.hpp"

#include "util/vec2.hpp"
#include <vector>
#include <string>
#include <cinttypes>

struct BattleCommander
{
	BattleCommander() = default;
	BattleCommander(const BattleCommander&) = default;
	BattleCommander(const TroopReference& prototype, size_t id, size_t owner)
		: id(id)
		, owner(owner)
		, reference(&prototype)
	{
		std::vector<std::string> names = { "Paavo", "Irmeli", "Kaapo", "Perse", "Kikkeli", "Kulli", "Pena", "Milla", "Suvi" };
		name = names[sa::math::rand<2>(id) % names.size()];
	}

	struct Squad
	{
		enum class Behaviour
		{
			ProtectCaptain,
			ProtectPosition,
			AttackClosest,
			AttackRear
		};

		// TODO: Targeting priorities (e.g. prefer ranged target, prefer scientist target, prefer armored...)
		// TODO: Formation handling

		std::vector<size_t> unitIds;

		// battle logic description
		sa::vec2<int> startPosition;
		Behaviour behaviour;
	};

	size_t id = 0;
	size_t owner = 0;

	std::string name;
	const TroopReference* reference;
	
	std::vector<Squad> squads;

	StrategyViewOrder myOrder;
};