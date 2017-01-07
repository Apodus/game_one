#pragma once

#include "Types.hpp"

namespace bs
{
	class Unit
	{
	public:

		typedef U32 Id;
		Vec moveTarget;
		Vec pos;
		Vec vel;
		Vec acc;

		Real radius;

		BoundingBox bb;

		// Attributes
		U32 speed = 0;
		U32 agility = 0;
		U32 hitpoints = 0;

		// Weapons, TODO: allow multiple weapons
		U32 strength = 0;
		U32 range = 0;
		U32 weaponspeed = 0;
		U16 updatePriority = 0;


		Id id;
		U8 team;

	};
}