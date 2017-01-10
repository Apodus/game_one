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

void bs::Field::UpdatePriorities()
{
	for (size_t i = 0; i < myUnits.size(); i++)
	{
		auto& unit = myUnits[i];		
		auto len = ((unit.moveTarget - unit.pos).lengthSquared().getRawValue() - unit.radius.getRawValue()) >> 18;
		if (len < 0)
		{
			len = 0;
		}

		ASSERT(len <= UINT16_MAX, "Too low priority");
		unit.updatePriority = static_cast<U16>(len);
	}

	std::sort(myActiveUnits.begin(), myActiveUnits.end(), [&](Unit::Id a, Unit::Id b)
	{
		return myUnits[a].updatePriority < myUnits[b].updatePriority;
	});
}

void bs::Field::FindCollisions(
	const Unit& unit, const Vec& newPos, std::vector<Unit::Id>& collisions) const
{
	BoundingBox start;
	myLevels[0].FindBoundingBox(unit.pos, unit.radius, start);

	BoundingBox end;
	myLevels[0].FindBoundingBox(newPos, unit.radius, end);

	BoundingBox bb;
	bb.left = start.left < end.left ? start.left : end.left;
	bb.right = start.right > end.right ? start.right : end.right;
	bb.top = start.top < end.top ? start.top : end.top;
	bb.bottom = start.bottom > end.bottom ? start.bottom : end.bottom;

	for (U32 y = bb.top; y <= bb.bottom; y++)
	{
		for (U32 x = bb.left; x <= bb.right; x++)
		{
			auto& list = myLevels[0].GetUnitList(x, y);
			for (size_t i = 0; i < list.size(); i++)
			{
				if (list.at(i) != unit.id)
				{
					// if (std::find(collisions.begin(), collisions.end(), list.at(i)) == collisions.end())
					{
						collisions.emplace_back(list.at(i));
					}
				}
			}
		}
	}
	// Sort and erase duplicates.
	std::sort(collisions.begin(), collisions.end());
	collisions.erase(std::unique(collisions.begin(), collisions.end()), collisions.end());
}

bs::Unit::Id bs::Field::FindClosestEnemy(const Unit& unit, Real& range) const
{
	BoundingBox bb;
	range = Real(unit.range, 100) + unit.radius;
	bb.left = myLevels[0].POS(unit.pos.x - range);
	bb.top = myLevels[0].POS(unit.pos.y - range);
	bb.right = myLevels[0].POS(unit.pos.x + range);
	bb.bottom = myLevels[0].POS(unit.pos.y + range);
	return FindClosestEnemy(unit, bb, range);
}

bs::Unit::Id bs::Field::FindClosestEnemy(const Unit& unit, const BoundingBox& bb,  Real& range) const
{
	Unit::Id otherId = static_cast<Unit::Id>(-1);
	for (U32 y = bb.top; y <= bb.bottom; y++)
	{
		for (U32 x = bb.left; x <= bb.right; x++)
		{
			FindClosestEnemy(unit, x, y, range, otherId);
		}
	}
	return otherId;
}

void bs::Field::FindClosestEnemy(const Unit& unit, U32 x, U32 y, Real& range, Unit::Id& otherId) const
{
	auto& list = myLevels[0].GetUnitList(x, y);
	for (size_t i = 0; i < list.size(); i++)
	{
		auto& otherUnit = myUnits[list[i]];
		if (otherUnit.team != unit.team)
		{
			auto radius = unit.radius + otherUnit.radius;
			auto len = (otherUnit.pos - unit.pos).lengthSquared() + (radius*radius);
			if (len < range)
			{
				range = len;
				otherId = list[i];
			}
		}
	}
}


void bs::Field::Update()
{
	UpdatePriorities();

	// Movement
	std::vector<Unit::Id> deleted;
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
		FindCollisions(unit, newPos, collisions);

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
				myRand = sa::math::rand(myRand);
				if ((myRand & 1) == 1)
				{
					other.hitpoints--;
					if (other.hitpoints == 0)
					{
						deleted.emplace_back(other.id);
					}
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
	myFrames.push_back(Frame());
	myFrames.back().units = myUnits;

	// Remove deleted from active. This will change order of active units, but we are 
	// going to sort active units next update anyway.
	for (size_t i = 0; i < deleted.size(); i++)
	{
		auto iter = std::find(myActiveUnits.begin(), myActiveUnits.end(), deleted[i]);
		ASSERT(iter != myActiveUnits.end(), "Deleted not found");
		myLevels[0].RemoveUnit(myUnits[*iter]);
		*iter = myActiveUnits.back();
		myActiveUnits.pop_back();
	}
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
