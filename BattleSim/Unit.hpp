#pragma once

#include "Types.hpp"

namespace bs
{
	class Unit
	{
	public:

		typedef U32 Id;
		static const Id InvalidId = static_cast<Id>(-1);

		enum class Type : U8
		{
			Character,
			Projectile
		};

		
		Vec pos = Vec();
		Vec vel = Vec();
		Vec acc = Vec();
		Real angle = Real(0);
		Vec moveTarget;
		Vec aimTarget;
		BoundingBox bb;

		Real radius = Real(10);
		Id id;
		U32 hitpoints = 0;
		U32 receivedDamage = 0;
		U32 range = 5000;
		
		// Attributes
		U32 speed = 0;
		U32 agility = 0;

		// Weapons, TODO: allow multiple weapons
		U16 weaponId = 0;
		TimerId timerId = InvalidTimer;

		Tick nextAttackAllowed = 0;
		U8 team = 0;
		U8 group = 0;
		Type type = Type::Character;



	};
}