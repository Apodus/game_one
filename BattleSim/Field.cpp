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

void bs::Field::UpdatePriorities()
{
	for (size_t i = 0; i < myUnits.size(); i++)
	{
		auto& unit = myUnits[i];
		auto len = (unit.pos - unit.moveTarget).lengthSquared().abs().getRawValue();
		unit.updatePriority = static_cast<U32>(len);
	}

	std::sort(myActiveUnits.begin(), myActiveUnits.end(), [&](Unit::Id a, Unit::Id b)
	{
		return myUnits[a].updatePriority < myUnits[b].updatePriority;
	});
}

void bs::Field::Update()
{
	UpdatePriorities();

	// Movement
	std::vector<Unit::Id> collisions;
	collisions.reserve(16);
	for (size_t i = 0; i < myActiveUnits.size(); i++)
	{
		auto& unit = myUnits[myActiveUnits[i]];
		if (unit.hitpoints > 0)
		{
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

			Unit::Id collisionId = static_cast<Unit::Id>(-1);
			for (size_t j = 0; j < collisions.size(); j++)
			{
				auto& other = myUnits[collisions[j]];
				Vec hitPos;
				if (CollisionCheck(other, unit, newPos, hitPos))
				{
					collisionId = other.id;
					newPos = hitPos;
				}
			}

			if (collisionId != static_cast<Unit::Id>(-1))
			{
				Vec dir = newPos - unit.pos;
				auto dp = dir.dotProduct(targetDir);
				auto& other = myUnits[collisionId];
				newVel.x = (dp * unit.vel.x) / Real(2, 1);
				newVel.y = (dp * unit.vel.y) / Real(2, 1);
				newVel.z = (Real(0, 1));

				if (other.team != unit.team && other.hitpoints > 0)
				{
					other.hitpoints--;
					if (other.hitpoints == 0)
					{
						myLevels[0].RemoveUnit(other);
					}
				}
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
	}
	myFrames.push_back(Frame());
	myFrames.back().units = myUnits;
}


// Cylinder collision check
bool bs::Field::CollisionCheck(const Unit& a, const Unit& b, const Vec& endPos, Vec& hitPos)
{
	Real radii = a.radius + b.radius;

	Real left = b.pos.x < endPos.x ? b.pos.x - radii : endPos.x - radii;
	Real right = b.pos.x > endPos.x ? b.pos.x + radii: endPos.x + radii;
	Real top = b.pos.y < endPos.y ? b.pos.y - radii : endPos.y - radii;
	Real bottom = b.pos.y > endPos.y ? b.pos.y + radii : endPos.y + radii;
	if (a.pos.x < left || a.pos.x > right || a.pos.y < top || a.pos.y > bottom)
	{
		// No in bounding box
		return false;
	}

	/* movement vector */
	Vec N = endPos - b.pos;
	Real length_N = N.length(); // TODO 2d vec
	if (length_N == Real(0, 1))
	{
		// Not moving, TODO: should probably assert here
		return false;
	}

	Vec C = a.pos - b.pos;
	C.z = Real(0, 1); /* Ignore Z: used for sphere collision */
	Real length_C = C.length(); // TODO 2d vec

	if (length_C == Real(0,1))
	{
		// Inside each other, don't consider collision though
		return false;
	}

	if (length_N + radii < length_C)
	{
		return false;
	}

	/*
	* Make sure that A is moving
	* towards B! If the dot product between the N and C
	* is less that or equal to 0, A isn't isn't moving towards B
	*/
	N.x /= length_N;
	N.y /= length_N;
	N.z = Real(0, 1);
	
	C.x /= length_C;
	C.y /= length_C;
	C.z = Real(0,1);

	Real D = C.dotProduct(N);
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
}
