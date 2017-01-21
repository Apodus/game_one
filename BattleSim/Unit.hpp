#pragma once

#include "Types.hpp"

namespace bs
{
	class Unit
	{
	public:

		typedef U32 Id;
		typedef U16 Tick;
		static const Tick InvalidTick = UINT16_MAX;

		enum class Type : U8
		{
			Character,
			Projectile
		};

		
		Vec pos = Vec();
		Vec vel = Vec();
		Vec acc = Vec();
		Vec moveTarget;
		Vec aimTarget;
		BoundingBox bb;

		Real radius = Real(10,1);
		Id id;
		U32 hitpoints = 0;
		U32 receivedDamage = 0;
		U32 range = 100;
		
		// Attributes
		U32 speed = 0;
		U32 agility = 0;

		// Weapons, TODO: allow multiple weapons
		U16 weaponId = 0;

		Tick nextAttackAllowed = 0;
		U8 team = 0;
		U8 group = 0;
		Type type = Type::Character;



	};
}