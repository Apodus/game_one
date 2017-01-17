#pragma once

#include "session/game/troops/TroopReference.hpp"
#include <cinttypes>

struct Troop
{
	Troop() = default;
	Troop(const TroopReference& prototype, size_t id, size_t owner)
		: id(id)
		, owner(owner)
		, commanderId(0)
		, reference(&prototype)
	{}

	size_t id = 0;
	size_t owner = 0;
	size_t commanderId = 0; // follows this commander. this is kind of redundant - but makes life easier.

	const TroopReference* reference;

	// TODO: Afflictions
};