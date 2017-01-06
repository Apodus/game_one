#include "stdafx.h"
#include "Field.hpp"

const bs::Real bs::Field::TimePerUpdate = bs::Real(100, 1000);

bs::Field::Field()
{
	myFrames.emplace_back();
}

bs::Unit::Id bs::Field::Add(Unit& unit)
{
	unit.id = myUnits.size();
	myUnits.emplace_back(unit);
	myLevels[0].AddUnit(myUnits.back());
	myActiveUnits.emplace_back(unit.id);
	return unit.id;
}

// Real to Float conversion only for logging purposes
double bs::Field::RToF(const Real& real) const
{
	return static_cast<double>(real.getRawValue()) / real.s_fpOne;
}

void bs::Field::Update()
{
	// Movement
	std::vector<Unit::Id> collisions;
	collisions.reserve(16);
	for (size_t i = 0; i < myActiveUnits.size(); i++)
	{
		auto& unit = myUnits[myActiveUnits[i]];

		Vec targetDir = unit.moveTarget - unit.pos;
		auto targetDirLen = targetDir.length();
		if (targetDirLen > Real(0, 1))
		{
			targetDir.x /= targetDirLen;
			targetDir.y /= targetDirLen;
			targetDir.z /= targetDirLen;
		}

		const bs::Real Agility(10);
		const bs::Real Speed(3);
		const bs::Real SlowDown(1, 2);
		unit.acc = targetDir * Agility;

		Vec newVel = unit.acc * TimePerUpdate + unit.vel;
		auto speed = newVel.length();
		if (speed > Speed)
		{
			auto excessSpeed = speed - Speed;
			excessSpeed *= SlowDown;
			newVel.normalize();
			newVel *= (Speed + excessSpeed);
		}

		Vec newPos = unit.vel * TimePerUpdate + unit.pos;
		collisions.clear();
		myLevels[0].FindCollisions(unit, newPos, collisions);

		bool foundCollision = false;
		for (size_t j = 0; j < collisions.size(); j++)
		{
			auto& other = myUnits[collisions[j]];
			Vec hitPos;
			if (CollisionCheck(other, unit, newPos, hitPos))
			{
				foundCollision = true;
				newPos = hitPos;
			}
		}

		if (foundCollision)
		{
			newVel.x = (Real(0,1) - unit.vel.x) / Real(2, 1);
			newVel.y = (Real(0,1) - unit.vel.y) / Real(2, 1);
			newVel.z = (Real(0, 1));

		}

		if (myLevels[0].IsGridMove(unit, newPos))
		{
			myLevels[0].RemoveUnit(unit);
			unit.pos = newPos;
			myLevels[0].AddUnit(unit);
		}
		else
		{
			unit.pos = newPos;
		}

		unit.vel = newVel;
#if 0
		if (unit.id == 0)
		{
			LOG("[%lu] POS = %f, %f DIR=%f, %f VEL= %f, %f",
				unit.id,
				RToF(unit.pos.x), RToF(unit.pos.y),
				RToF(targetDir.x), RToF(targetDir.y),
				RToF(unit.vel.x), RToF(unit.vel.y));
		}
#endif
	}
	myFrames.push_back(Frame());
	myFrames.back().units = myUnits;
}


// Cylinder collision check
bool bs::Field::CollisionCheck(const Unit& a, const Unit& b, const Vec& endPos, Vec& hitPos)
{
	/* movement vector */
	Vec N = endPos - b.pos;
	Real length_N = N.length(); // TODO 2d vec
	if (length_N <= Real(0, 1))
	{
		// Not moving
		return false;
	}

	/*    if ( id == 6 && radii != 0.8f)
	{
	printf("player hit check at %f,%f\n",
	pos.x, pos.y );
	printf("Projectile: %f,%f -> %f,%f (rad:%f)\n",
	info->start.x, info->start.y,
	info->end.x, info->end.y, radii );
	} */

	Vec C = a.pos - b.pos;
	/*C.x = mLocation.x - info->start.x;
	C.y = mLocation.y - info->start.y;
	C.z = 0;*/
	C.z = Real(0, 1); /* Ignore Z: used for sphere collision */


	/* N.x = info->end.x - info->start.x;
	N.y = info->end.y - info->start.y;
	N.z = info->end.z - info->start.z;*/

	Real length_C = C.length(); // TODO 2d vec

	/* if ( length_C < radii )
	{
	printf("Already inside target!\n");
	info->end = info->start;
	info->hit_id = id;
	return;
	} */

	Real radii = a.radius + b.radius;
	if (length_N < (length_C - radii))
	{
		return false;
	}

	/*
	* Make sure that A is moving
	* towards B! If the dot product between the N and C
	* is less that or equal to 0, A isn't isn't moving towards B
	*/
	N.normalize();
	// vect_normalize3d(&N);
	C.normalize();
	// vect_normalize3d(&C);
	Real D = C.dotProduct(N);
	// float D = dot_product2d(&C, &N);
	if (D <= Real(0, 1))
	{
		return false;
	}

	/*
	* Escape test: if the closest that A will get to B
	* is more than the sum of their radii, there's no
	* way they are going collide
	*/
	Real F = (length_C * length_C) - (D * D);
	if (F >= (radii*radii))
	{
		return false;
	}

	Real T = (radii*radii) - F;
	if (T < Real(0,1) )
	{
		return false;
	}

	/*
	* Therefore the distance the circle has to travel along
	* movement vector is D - sqrt(T)
	*/
	Real dist = D - sa::math::sqrt(T);

	if (length_N < dist)
	{
		return false;
	}
	N.x = N.x*(dist)+b.pos.x;
	N.y = N.y*(dist)+b.pos.y;
	N.z = N.z*(dist)+b.pos.z;
	hitPos = N;
	return true;
#if 0
	if (
#ifdef _DEBUG
		NULL == bounds ||
#endif /* _DEBUG */
		(N.x >= bounds->x_min) &&
		(N.y >= bounds->y_min) &&
		(N.x < bounds->x_max) &&
		(N.x < bounds->x_max))
	{
		if (bouncing)
		{
			assert(info->source_id != this->id);
			{
				if (visitor == info->friendlyTeamId && tagged == PLAYER_ID_UNDEFINED)
				{
					if (vect_dist(&info->originalStart, &this->mLocation) < PLAYER_FRIEND_VISIBLE_RADIUS)
					{
						// printf("own shooting from close range\n");
						return;
					}
				}

				float speed;
				if (info->isPartialTick)
				{
					speed = vect_dist3d(&info->realEnd, &info->start) / TICK;
				}
				else
				{
					speed = vect_dist3d(&info->end, &info->start) / TICK;
				}

#ifdef _DEBUG
				if (speed <= 20.0f)
				{
					printf("SIMERROR: radii: %f speed: %f <= 25.0f\n", radii, speed);
					PBSimEnv::PBSimVideoGet()->video_record_debug(
						SimEnv::SimEnvGet()->TimeGet(),
						(uint16)(100 * (info->start.x - VIDEO_FIELD_OFFSET_X)),
						(uint16)(100 * (info->start.y - VIDEO_FIELD_OFFSET_Y)),
						(uint16)(100 * (info->end.x - VIDEO_FIELD_OFFSET_X)),
						(uint16)(100 * (info->end.y - VIDEO_FIELD_OFFSET_Y)));
				}
#endif /* _DEBUG */

				if (speed < BOUNCE_OFF_SPEED_LIMIT)
				{
					if ((rand_uint16() % 1001) <=
						static_cast<long int>(100.0f*(1.0f - (speed / BOUNCE_OFF_SPEED_LIMIT))))
					{
						// printf("bounced off\n");
						/*
						printf("bounce: %f vect_dist: %f\n", radii, speed );
						PBSimEnv::PBSimVideoGet()->video_record_debug(
						SimEnv::SimEnvGet()->TimeGet(),
						(uint16)(100*(info->start.x-VIDEO_FIELD_OFFSET_X)),
						(uint16)(100*(info->start.y-VIDEO_FIELD_OFFSET_Y)),
						(uint16)(100*(info->end.x-VIDEO_FIELD_OFFSET_X)),
						(uint16)(100*(info->end.y-VIDEO_FIELD_OFFSET_Y)) );*/
						info->end = N;
						info->end.z = 0.0f;
						return;
					}
				}
			}
		}
		info->end = N;
		info->hit_id = id;
	}

	return;
#endif
}
