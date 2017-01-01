#pragma once

#include "Types.hpp"

namespace bs
{
	class Unit
	{
	public:

		// Attributes
		U32 speed;
		U32 agility;
		U32 hitpoints;

		// Weapons, TODO: allow multiple weapons
		U32 strength;
		U32 range;
		U32 weaponspeed;
	};
}