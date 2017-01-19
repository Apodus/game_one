#pragma once

#include "session/game/troops/Commander.hpp"
#include "session/game/troops/Troop.hpp"

#include "util/vec2.hpp"
#include "math/2d/math.hpp"

#include <vector>
#include <cinttypes>


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
		uint32_t m_terrainType = 0;
		size_t m_area = 0;

		std::vector<BattleCommander> commanders;
		std::vector<Troop> units;
		std::vector<Building> buildings;
		std::vector<NaturalResource> resources;
		std::vector<std::string> troopsToRecruit; // names of available troops.

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

	//TODO: Take owners of provinces into account.
	bool canTravel(int moves, uint32_t terrainFast, size_t startProvince, size_t endProvince) {
		auto result = countDistances(terrainFast, startProvince, endProvince);
		return result[endProvince] <= moves;
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
	std::vector<int> countDistances(uint32_t terrainFastTravel, size_t sourceProvince, size_t targetProvince) {
		std::vector<int> moveCosts;
		moveCosts.resize(m_provinces.size(), 10000);
		moveCosts[sourceProvince] = 0;

		std::vector<int> fringe;
		fringe.emplace_back(sourceProvince);

		while (!fringe.empty()) {
			int next = fringe.back();
			fringe.pop_back();

			int currentCost = moveCosts[next];
			auto& province = m_provinces[next];

			uint32_t firstTerrain = province.m_terrainType;
			bool fastFirst = (firstTerrain & terrainFastTravel) == firstTerrain;

			for (size_t i = 0; i < province.m_connections.size(); ++i)
			{
				size_t targetProvinceIndex = province.m_connections[i];
				auto& secondProvince = m_provinces[targetProvinceIndex];
				bool fastSecond = (secondProvince.m_terrainType & terrainFastTravel) == secondProvince.m_terrainType;

				int cost = 2;
				if (!fastFirst)
					cost += 1;
				if (!fastSecond)
					cost += 1;

				if (currentCost + cost < moveCosts[targetProvinceIndex]) {
					moveCosts[targetProvinceIndex] = currentCost + cost;
					fringe.push_back(targetProvinceIndex);
				}
			}
		}

		return std::move(moveCosts);
	}

	std::vector<Province> m_provinces;
};