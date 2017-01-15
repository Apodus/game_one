#pragma once

#include "Types.hpp"

namespace bs
{
	class Unit
	{
	public:

		typedef U32 Id;

		Vec pos;
		Real radius;
		U32 hitpoints = 0;
		U32 receivedDamage = 0;
		U32 range = 100;
		U8 team;
		U8 group;

		enum class State : U8
		{
			Inactive,
			Starting,
			Active,
			Stopping
		};
		State state = State::Inactive;

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