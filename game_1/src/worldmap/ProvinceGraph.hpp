#pragma once

#include <vector>
#include "util/vec2.hpp"
#include "math/2d/math.hpp"

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
		size_t m_owner = 0;

		// province properties
		size_t m_population = 1000;
		size_t m_type = 0;
		size_t m_area = 0;

	public:
		Province() = default;
		Province(sa::vec2<float> pos) : m_position(pos)
		{
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
	};

	ProvinceGraph()
	{
	}

	void random(size_t numProvinces = 30)
	{
		std::vector<float> radii;
		std::vector<sa::vec2<float>> offsets;
		sa::vec2<float> origin(0, 0);
		
		for (size_t i = 0; i < numProvinces; ++i)
		{
			m_provinces.emplace_back(
				Province(
					sa::vec2<float>(
						static_cast<float>(i) - numProvinces / 2,
						static_cast<float>(i)
					)
				)
			);
			radii.emplace_back(1.0f + (rand() % 256u) / 256.0f); // todo: nonlinear distribution?
			m_provinces.back().area(radii.back() * 100);
			offsets.emplace_back();
		}
		
		float volatility = 0;

		for(size_t iteration = 0; iteration < 10000u; ++iteration)
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

			// apply forces
			for (size_t i = 0; i < m_provinces.size(); ++i)
			{
				m_provinces[i].m_position += offsets[i];
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

	const std::vector<Province>& provinces() const
	{
		return m_provinces;
	}

private:
	std::vector<Province> m_provinces;
};