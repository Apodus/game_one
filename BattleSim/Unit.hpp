#pragma once

#include "Types.hpp"

namespace bs
{
	class Unit
	{
	public:

		typedef U32 Id;

		// Public unit data - visible to UI
		Vec pos;
		Real radius;
		U32 hitpoints = 0;
		U32 range = 100;
		U8 team;
		U8 group;


		// Private data
		struct Weapon
		{
			U32 strength = 0;
			U32 speed = 0;
		};


		Vec vel;
		Vec acc;
		Vec moveTarget;
		BoundingBox bb;

		Id id;

		// Attributes
		U32 speed = 0;
		U32 agility = 0;

		// Weapons, TODO: allow multiple weapons
		U16 weaponId;
	};
}