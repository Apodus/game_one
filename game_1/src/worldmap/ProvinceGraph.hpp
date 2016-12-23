#pragma once

#include <vector>
#include "util/vec2.hpp"
#include "math/2d/math.hpp"

class ProvinceGraph
{
public:
	class Province
	{
		// graph properties
		std::vector<int> m_connections;
		sa::vec2<float> m_position;
		size_t m_owner = 0;

		// province properties
		size_t m_population = 1000;
		size_t m_type = 0;

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
	};

	ProvinceGraph()
	{
	}

	void random(size_t numProvinces = 10)
	{
		std::vector<float> radii;
		std::vector<sa::vec2<float>> offsets;
		sa::vec2<float> origin(0, numProvinces / 2);
		
		for (size_t i = 0; i < numProvinces; ++i)
		{
			m_provinces.emplace_back(Province(i, i));
			radii.emplace_back(1.0f + (rand() % 256u) / 256.0f); // todo: nonlinear distribution?
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
				offsets[i] = direction * 0.02f;
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
						direction *= (allowedDistance / sqrDist) * 0.01f;
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

		}
	}

private:
	std::vector<Province> m_provinces;
};