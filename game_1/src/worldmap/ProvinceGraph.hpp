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
		m_provinces.emplace_back(Province(0, 0));
		while (m_provinces.size() < numProvinces)
		{
			size_t index = rand() % m_provinces.size();
			sa::vec2<float> initialPos = m_provinces[index].m_position;

			float angle = 2.0f * sa::math::PI_float * (rand() % 256) / 256.0f;
			float distance = 0.5f + 1.0f * (rand() % 256) / 256.0f;

			sa::vec2<float> newPos = initialPos;
			newPos += sa::math::rotateXY(
				sa::vec2<float>(distance, 0),
				angle
			);

			bool ok = true;
			for (auto& province : m_provinces)
			{
				float distSquared = (province.m_position - newPos).lengthSquared();
				ok &= distSquared > 0.5f * 0.5f;
			}

			if (ok)
			{

			}
		}
	}

private:
	std::vector<Province> m_provinces;
};