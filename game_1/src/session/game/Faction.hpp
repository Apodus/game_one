#pragma once

#include <string>
#include <vector>
#include <cstdint>

class Faction
{
public:
	Faction() = default;
	Faction(std::string name) : name(std::move(name))
	{
	}

	int64_t currency = 200; // can go to negative
	std::vector<size_t> scienceResources;
	std::string name;
};