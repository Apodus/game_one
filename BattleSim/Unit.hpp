#pragma once

#include "Types.hpp"

namespace bs
{
	class Unit
	{
	public:

		typedef U32 Id;

		// Attributes
		U32 speed = 0;
		U32 agility = 0;
		U32 hitpoints = 0;

		// Weapons, TODO: allow multiple weapons
		U32 strength = 0;
		U32 range = 0;
		U32 weaponspeed = 0;

		Vec moveTarget;

		Vec pos;
		Vec vel;
		Vec acc;
		Id id;
	};
}