#include "stdafx.h"
#include "Field.hpp"

const bs::Real bs::Field::TimePerUpdate = bs::Real(100, 1000);

bs::Field::Field()
{
}

bs::Unit::Id bs::Field::Add(Unit& unit)
{
	unit.id = myUnits.size();
	unit.state = Unit::State::Starting;
	myUnits.emplace_back(unit);
	myLevels[0].AddUnit(myUnits.back());
	myActiveUnits.emplace_back(unit.id);
	myStartingUnits.emplace_back(unit.id);
	return unit.id;
}

void bs::Field::UpdatePriorities()
{
	Vector<U16> unitUpdatePriorities;
	unitUpdatePriorities.resize(myUnits.size());
	for (size_t i = 0; i < myActiveUnits.size(); i++)
	{
		auto& unit = myUnits[myActiveUnits[i]];
		auto len = ((unit.moveTarget - unit.pos).lengthSquared().getRawValue() - unit.radius.getRawValue()) >> 18;
		if (len < 0)
		{
			len = 0;
		}
		ASSERT(len <= UINT16_MAX, "Too low priority");
		unitUpdatePriorities[myActiveUnits[i]] = static_cast<U16>(len);
	}

	std::sort(myActiveUnits.begin(), myActiveUnits.end(), [&](Unit::Id a, Unit::Id b)
	{
		return unitUpdatePriorities[a] < unitUpdatePriorities[b];
	});
}

void bs::Field::FindCollisions(
	const Unit& unit, Unit::Id id, const Vec& newPos, std::vector<Unit::Id>& collisions) const
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
				if (list.at(i) != id)
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
	std::vector<Unit::Id> killed;
	std::vector<Unit::Id> collisions;
	collisions.reserve(16);
	for (size_t i = 0; i < myActiveUnits.size(); i++)
	{
		auto& unit = myUnits[myActiveUnits[i]];

		bs::Real Speed(3);
		const bs::Real SlowDown(1, 2);
		Vec targetDir;
		if (unit.hitpoints > 0)
		{
			targetDir = unit.moveTarget - unit.pos;
			auto targetDirLen = targetDir.length();
			if (targetDirLen > Real(0, 1))
			{
				targetDir.x /= targetDirLen;
				targetDir.y /= targetDirLen;
				targetDir.z /= targetDirLen;
			}

			const bs::Real Agility(10);
			unit.acc = targetDir * Agility;
		}
		else
		{
			unit.acc = bs::Vec();
			Speed = Real(0,1);
		}

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
		FindCollisions(unit, unit.id, newPos, collisions);

		Unit::Id collisionId = static_cast<Unit::Id>(-1);
		for (size_t j = 0; j < collisions.size(); j++)
		{
			auto& other = myUnits[collisions[j]];
			Vec hitPos;
			if (CollisionCheck(other, unit, newPos, hitPos))
			{
				collisionId = myUnits[collisions[j]].id;
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

			if (other.team != unit.team)
			{
				myRand = sa::math::rand(myRand);
				if ((myRand & 1) == 1)
				{
					other.receivedDamage++;
				}
			}
		}

		bool isMoved = unit.state == Unit::State::Starting;

		if ((unit.pos - newPos).lengthSquared().getRawValue() > 1)
		{
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
			isMoved = true;
		}
		else if (unit.hitpoints == 0)
		{
			// Add to killed list only after dead and movement stopped.
			killed.emplace_back(unit.id);
		}

		if (unit.receivedDamage > 0)
		{
			unit.hitpoints = unit.receivedDamage >= unit.hitpoints ? 0 : unit.hitpoints - unit.receivedDamage;
			isMoved = true;
			unit.receivedDamage = 0;
		}

		if (isMoved)
		{
			myMovingUnits.emplace_back(unit.id);
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

	WriteUpdate();

	// Remove killed from active. This will change order of active units, but we are 
	// going to sort active units next update anyway.
	for (size_t i = 0; i < killed.size(); i++)
	{
		auto iter = std::find(myActiveUnits.begin(), myActiveUnits.end(), killed[i]);
		ASSERT(iter != myActiveUnits.end(), "Killed not found");
		myLevels[0].RemoveUnit(myUnits[*iter]);
		*iter = myActiveUnits.back();
		myActiveUnits.pop_back();
	}
}

void bs::Field::WriteUpdate()
{
	std::sort(myMovingUnits.begin(), myMovingUnits.end());
	const U16 numStartingUnits = static_cast<U16>(myStartingUnits.size());
	const U16 numMovingUnits = static_cast<U16>(myMovingUnits.size());
	

	ByteBuffer& update = myVisualizationSystem.StartWriting(
		sizeof(U16) * 2 +
		sizeof(Visualization::Addition) * numStartingUnits +
		sizeof(Visualization::Movement) * numMovingUnits);

	auto writer = update.GetWriter();
	
	writer.Write(numStartingUnits);
	for (size_t i = 0; i < numStartingUnits; i++)
	{
		auto& elem = writer.Write<Visualization::Addition>();
		elem.id = myStartingUnits[i];
		auto& unit = myUnits[elem.id];
		elem.team = unit.team;
		elem.radius = unit.radius;
		unit.state = Unit::State::Active;
	}

	writer.Write(numMovingUnits);
	for (size_t i = 0; i < numMovingUnits; i++)
	{
		auto& elem = writer.Write<Visualization::Movement>();
		elem.id = myMovingUnits[i];
		elem.pos = myUnits[elem.id].pos;
		elem.hitpoints = myUnits[elem.id].hitpoints;
	}
	myVisualizationSystem.StopWriting();

	myStartingUnits.clear();
	myMovingUnits.clear();
}

// Cylinder collision check
bool bs::Field::CollisionCheck(const Unit& a, const Unit& b, const Vec& endPos, Vec& N)
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
	N = endPos - b.pos;
	Real length_N = N.length(); // TODO 2d vec
	if (length_N == Real(0, 1))
	{
		// Not moving, TODO: should probably assert here
		return false;
	}

	Vec C = a.pos - b.pos;
	C.z = Real(0, 1); /* Ignore Z: used for sphere collision */
	Real length_C = sa::math::sqrt((C.x*C.x + C.y*C.y)); 

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
	const Real radii2 = radii*radii;
	if (F >= (radii2))
	{
		return false;
	}

	Real T = (radii2) - F;
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
	return true;
}
