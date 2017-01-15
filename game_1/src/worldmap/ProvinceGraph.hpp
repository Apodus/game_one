#pragma once

#include <vector>
#include "util/vec2.hpp"
#include "math/2d/math.hpp"

struct TroopReference
{
	std::string name;
	std::string icon;

	uint32_t hp = 0;

	// these are innate values of the troop, without counting the effects of equipment.
	uint32_t strength = 0; // melee damage addition
	uint32_t fitness = 0; // recovery of stamina
	uint32_t accuracy = 0; // ranged accuracy
	uint32_t attack = 0; // hit chance
	uint32_t defence = 0; // evade chance
	uint32_t armor = 0; // damage mitigation
	uint32_t leadership = 0; // how many troops can control.
	uint32_t isCommander = 0; // starts out with commander status?

	// TODO: Abilities / Special properties (eg. fire resistant, trampling, regenerating, medic, ...?)
	// TODO: Equipment definition
	// TODO: Visual representation definition
};

struct Troop
{
	Troop() = default;
	Troop(const TroopReference& prototype, size_t id, size_t owner)
		: id(id)
		, owner(owner)
		, commanderId(0)
		, upkeep(0) // TODO
		, reference(prototype)
	{
	}

	size_t id = 0;
	size_t owner = 0;
	size_t commanderId = 0; // follows this commander. this is kind of redundant - but makes life easier.
	size_t upkeep = 0;

	const TroopReference& reference;
	
	// TODO: Afflictions
};

struct BattleCommander
{
	BattleCommander() = default;
	BattleCommander(const TroopReference& prototype, size_t id, size_t owner)
		: id(id)
		, owner(owner)
		, reference(prototype)
	{
		std::vector<std::string> names = {"Paavo", "Irmeli", "Kaapo", "Perse", "Kikkeli", "Kulli", "Pena", "Milla", "Suvi"};
		name = names[sa::math::rand(id) % names.size()];
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
	size_t upkeep = 0;
	
	std::string name;
	const TroopReference& reference;
	bool m_selected = false; // for menu use only. has no effect on game logic.

	std::vector<Squad> squads;
	
	enum class OrderType
	{
		Idle,
		Move,
		Patrol, // battle outside of fortifications if attacked. pacify region.
		RunTests, // discover natural resources
		Research, // discover tech stuff for scientists to perform
		CastSpell // perform miracles of science
	};

	struct StrategyViewOrder
	{
		OrderType orderType = OrderType::Idle;
		size_t moveTo = 0;
		size_t spellToCast = 0;
	};

	StrategyViewOrder myOrder;
};

struct Building
{
	// ?? - We probably need some sort of constructibles.
};

struct NaturalResource
{
	int8_t discovered = 0;

	// TODO: requirements for discovery
	// TODO: produced resources
	// TODO: peculiar effects
};

class ProvinceGraph
{
public:
	class Province
	{
		// Todo: Would be nice if these were private.
	public:
		// graph properties
		std::vector<int> m_connections;
		sa::vec2<float> m_position;
		size_t m_owner = ~size_t(0);
		size_t m_index = 0; // index of the province in the province vector.

		// province properties
		size_t m_population = 1000;
		size_t m_type = 0;
		size_t m_area = 0;

		std::vector<BattleCommander> commanders;
		std::vector<Troop> units;
		std::vector<Building> buildings;
		std::vector<NaturalResource> resources;
		std::vector<std::string> troopsToRecruit; // names of available troops.

		std::vector<const TroopReference*> currentRecruitmentRequests;

		struct UIProperties
		{
			float alpha = 0.3f;
			float scale = 1;
		};

		UIProperties visualProperties;

	public:
		Province() = default;
		Province(size_t index, sa::vec2<float> pos) : m_position(pos)
		{
			m_index = index;
		}

		const std::vector<const TroopReference*> inspectRecruitOrders() const {
			return currentRecruitmentRequests;
		}

		void addRecruitOrder(const TroopReference* troopReference)
		{
			currentRecruitmentRequests.emplace_back(troopReference);
		}

		void removeRecruitOrder(const TroopReference* troopReference)
		{
			for (size_t i = 0; i < currentRecruitmentRequests.size(); ++i)
			{
				if (currentRecruitmentRequests[i] == troopReference)
				{
					currentRecruitmentRequests.erase(currentRecruitmentRequests.begin() + i);
					return;
				}
			}
		}

		void clearRecruitmentOrders()
		{
			currentRecruitmentRequests.clear();
		}

		size_t supplies() const
		{
			return 1 * m_population;
		}

		size_t income() const
		{
			return 1 * m_population;
		}

		size_t area() const
		{
			return m_area;
		}

		void area(size_t area)
		{
			m_area = area;
		}

		void updatevisual(float targetAlpha = 0.3f, float targetScale = 1.0f)
		{
			visualProperties.alpha += (targetAlpha - visualProperties.alpha) * 0.1f;
			visualProperties.scale += (targetScale - visualProperties.scale) * 0.1f;
		}

		float scale() const
		{
			return visualProperties.scale;
		}

		float alpha() const
		{
			return visualProperties.alpha;
		}
	};

	ProvinceGraph()
	{
	}

	void random(size_t numProvinces = 60)
	{
		std::vector<float> radii;
		std::vector<sa::vec2<float>> offsets;
		sa::vec2<float> origin(0, 0);
		
		for (size_t i = 0; i < numProvinces; ++i)
		{
			m_provinces.emplace_back(
				Province(
					m_provinces.size(),
					sa::vec2<float>(
						static_cast<float>(i) - numProvinces / 2,
						static_cast<float>(i)
					)
				)
			);
			radii.emplace_back(1.0f + (rand() % 256u) / 256.0f); // todo: nonlinear distribution?
			m_provinces.back().area(static_cast<size_t>(radii.back() * 100));
			offsets.emplace_back();
		}
		
		float volatility = 2;

		while(volatility > 0.00001f)
		{
			// initialize with pull force
			for (size_t i = 0; i < m_provinces.size(); ++i)
			{
				sa::vec2<float> direction = (origin - m_provinces[i].m_position);
				direction.normalize();
				offsets[i] = direction * 0.005f;
			}

			// accumulate push forces
			for (size_t i = 0; i < m_provinces.size(); ++i)
			{
				for (size_t k = i + 1; k < m_provinces.size(); ++k)
				{
					float allowedDistance = radii[i] + radii[k];
					sa::vec2<float> direction = m_provinces[i].m_position - m_provinces[k].m_position;
					float sqrDist = direction.lengthSquared();
					if (sqrDist < allowedDistance * allowedDistance)
					{
						direction.normalize();
						direction *= ((allowedDistance * allowedDistance / sqrDist) - 1.0f) * 0.05f;
						offsets[i] += direction;
						offsets[k] -= direction;
					}
				}
			}

			volatility = 0;

			// apply forces
			for (size_t i = 0; i < m_provinces.size(); ++i)
			{
				m_provinces[i].m_position += offsets[i];
				volatility += offsets[i].lengthSquared();
			}
		}

		// apply connections
		{
			for (size_t i = 0; i < m_provinces.size(); ++i)
			{
				for (size_t k = i + 1; k < m_provinces.size(); ++k)
				{
					float radius = radii[i] + radii[k];
					if ((m_provinces[i].m_position - m_provinces[k].m_position).lengthSquared() < (radius * radius))
					{
						m_provinces[i].m_connections.emplace_back(k);
						m_provinces[k].m_connections.emplace_back(i);
					}
				}
			}
		}
	}

	Province* getNearestProvince(sa::vec2<float> worldPos)
	{
		ProvinceGraph::Province* best = nullptr;
		float bestVal = 100000000.0f;

		for (auto& province : m_provinces)
		{
			float dist = (province.m_position - worldPos).lengthSquared();
			if (dist < bestVal)
			{
				bestVal = dist;
				best = &province;
			}
		}

		return best;
	}

	const std::vector<Province>& provinces() const
	{
		return m_provinces;
	}

	std::vector<Province>& provinces()
	{
		return m_provinces;
	}

private:
	std::vector<Province> m_provinces;
};