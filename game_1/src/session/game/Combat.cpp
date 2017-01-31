
#include "session/game/Combat.hpp"

#include "BattleSim/BattleSimAsync.h"


Combat::Combat() 
{
}

void Combat::add(const std::vector<BattleCommander>& commanders)
{
	m_commanders = commanders;
	
	for (size_t i = 0; i < m_commanders.size(); i++)
	{
		auto iter = m_ownerToIndex.find(m_commanders[i].owner);
		if (iter == m_ownerToIndex.end())
		{
			m_ownerToIndex[m_commanders[i].owner] = m_ownerToIndex.size();
		}
	}
}

void Combat::setup(bs::Battle& battle)
{	
	for (size_t i = 0; i < m_commanders.size(); i++)
	{
		auto& cmdr = m_commanders[i];
		size_t factionIndex = m_ownerToIndex[cmdr.owner];
		bs::Unit u;
		ASSERT(factionIndex <= UINT8_MAX, "Num factions not supported");
		u.team = static_cast<bs::U8>(factionIndex);
		u.radius = bs::Real(4, 10);
		u.hitpoints = cmdr.reference->hp;
		
		bs::Real x = bs::Real(100 + static_cast<int64_t>(i));
		switch (factionIndex)
		{
		case 0:
			u.pos.set(x, bs::Real(50), bs::Real(0));
			u.aimTarget.set(x, bs::Real(100), bs::Real(0));
			break;
		case 1:
			u.pos.set(x, bs::Real(100), bs::Real(0));
			u.aimTarget.set(x, bs::Real(50), bs::Real(0));
		default:
			u.pos.set(x, bs::Real(75), bs::Real(0));
			u.aimTarget.set(x, bs::Real(0), bs::Real(0));
			break;
		}

		u.moveTarget.set(bs::Real(100), bs::Real(75), bs::Real(0));
		u.id = battle.NumUnits();
		battle.Add(u);
	}
}

void Combat::resolve()
{
	bs::Battle battle;
	setup(battle);

	// Simulate
	LOG("Owners entering battle: %lu", m_ownerToIndex.size());
	auto sim = std::make_unique<bs::BattleSim>(battle);
	sim->Resolve();
	LOG("Total battle length %f seconds", static_cast<double>(battle.TotalTimeMS()) / 1000.0 )

	for (size_t i = 0; i < m_commanders.size(); i++)
	{
		auto& cmdr = m_commanders[i];
		auto& unit = battle.Get(i);
		Status status;
		status.hitpoints = unit.hitpoints;
		LOG("Commander %zu (team=%u) hitpoints=%u/%u", i, cmdr.owner, status.hitpoints, cmdr.reference->hp);
		m_status.emplace_back(status);
	}
}

