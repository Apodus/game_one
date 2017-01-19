#pragma once

#include <string>
#include <cinttypes>

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
	uint32_t terrainFast = 0; // terrain types of fast travel.
														//
	uint32_t goldCost = 10;
	uint32_t upkeep = 1;

	// TODO: Abilities / Special properties (eg. fire resistant, trampling, regenerating, medic, ...?)
	// TODO: Equipment definition
	// TODO: Visual representation definition
};