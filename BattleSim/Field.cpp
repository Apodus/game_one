#include "stdafx.h"
#include "Field.hpp"
#include "Battle.h"

const bs::Real bs::Field::TimePerUpdate = bs::Real(100, 1000);
const size_t MaxUpdates = static_cast<size_t>(15 * 60 / bs::Field::TimePerUpdate.toDouble());
const size_t MaxUnits = 20000;

bs::Field::Field(StreamingMode streaming) : myStreaming(streaming)
{
}

void bs::Field::UpdatePriorities()
{
	Vector<U16> unitUpdatePriorities;
	unitUpdatePriorities.resize(myUnits.size());
	for (size_t i = 0; i < myActiveUnits.size(); i++)
	{
		auto& unit = myUnits[myActiveUnits[i]];
		if (unit.type == Unit::Type::Character)
		{
			auto len = ((unit.moveTarget - unit.pos).lengthSquared().getRawValue() - unit.radius.getRawValue()) >> 18;
			if (len < 0)
			{
				len = 0;
			}
			ASSERT(len <= UINT16_MAX, "Too low priority");
			unitUpdatePriorities[myActiveUnits[i]] = static_cast<U16>(len);
		}
		else
		{
			unitUpdatePriorities[myActiveUnits[i]] = 0;
		}
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


bool bs::Field::Update()
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
		if (unit.type == Unit::Type::Character)
		{
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
				Speed = Real(0, 1);
			}
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
			auto& other = myUnits[collisionId];
			if (unit.type == Unit::Type::Character)
			{
				Vec dir = newPos - unit.pos;
				auto dp = dir.dotProduct(targetDir);
				newVel.x = (dp * unit.vel.x) / Real(2, 1);
				newVel.y = (dp * unit.vel.y) / Real(2, 1);
				newVel.z = (Real(0, 1));
			}

			if (other.team != unit.team)
			{
				unit.receivedDamage++;
				myRand = sa::math::rand(myRand);
				if ((myRand & 1) == 1)
				{
					other.receivedDamage++;
				}
			}
		}

		unit.vel = newVel;

		bool isMoved = false;

		if (unit.type == Unit::Type::Projectile)
		{
			if (unit.hitpoints != 0)
			{
				unit.hitpoints--;
				unit.pos = newPos;
				isMoved = true;
			}
		}
		else if ((unit.pos - newPos).lengthSquared().getRawValue() > 1)
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

		if (!isMoved && unit.hitpoints == 0)
		{
			// Add to killed list only after dead and movement stopped.
			killed.emplace_back(unit.id);
		}
		else if (unit.receivedDamage > 0)
		{
			unit.hitpoints = unit.receivedDamage >= unit.hitpoints ? 0 : unit.hitpoints - unit.receivedDamage;
			isMoved = true;
			unit.receivedDamage = 0;
		}

		if (IsStreaming() && isMoved)
		{
			myMovingUnits.emplace_back(unit.id);
		}
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
		if (unit.hitpoints != 0)
		{
			if (myTick >= unit.nextAttackAllowed)
			{
				unit.nextAttackAllowed = myTick + 10;
				Shoot(unit);
			}
		}
	}

	for (size_t i = 0; i < myStartingUnits.size(); i++)
	{
		auto& unit = myUnits[myStartingUnits[i]];
		if (unit.type == Unit::Type::Character)
		{
			myLevels[0].AddUnit(unit);
			myTeamUnitsLeft[unit.team]++;
		}
		myActiveUnits.emplace_back(myStartingUnits[i]);
		if (IsStreaming())
		{
			myMovingUnits.emplace_back(myStartingUnits[i]);
		}
	}

	// Remove killed from active. This will change order of active units, but we are 
	// going to sort active units next update anyway.
	myTick++;
	bool isActive = myTick < MaxUpdates;
	for (size_t i = 0; i < killed.size(); i++)
	{
		auto iter = std::find(myActiveUnits.begin(), myActiveUnits.end(), killed[i]);
		ASSERT(iter != myActiveUnits.end(), "Killed not found");
		auto& unit = myUnits[*iter];
		if (unit.type == Unit::Type::Character)
		{
			myTeamUnitsLeft[unit.team]--;
			if (myTeamUnitsLeft[unit.team] == 0)
			{
				isActive = false;
				StopAttacks();
			}
			myLevels[0].RemoveUnit(unit);
		}
		else
		{
			myStoppingUnits.emplace_back(unit.id);
			myFreeUnitIds.emplace_back(unit.id);
		}
		*iter = myActiveUnits.back();
		myActiveUnits.pop_back();
	}	

	if (IsStreaming())
	{
		WriteUpdate();
	}
	myStartingUnits.clear();
	myStoppingUnits.clear();

	return isActive;
}

void bs::Field::WriteUpdate()
{
	std::sort(myMovingUnits.begin(), myMovingUnits.end());
	const U16 numStartingUnits = static_cast<U16>(myStartingUnits.size());
	const U16 numMovingUnits = static_cast<U16>(myMovingUnits.size());
	const U16 numStoppingUnits = static_cast<U16>(myStoppingUnits.size());

	ByteBuffer& update = myVisualizationSystem.StartWriting(
		sizeof(U16) * 2 +
		sizeof(Visualization::Addition) * numStartingUnits +
		sizeof(Visualization::Removal) * numStoppingUnits +
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
	}

	writer.Write(numMovingUnits);
	for (size_t i = 0; i < numMovingUnits; i++)
	{
		auto& elem = writer.Write<Visualization::Movement>();
		elem.id = myMovingUnits[i];
		elem.pos = myUnits[elem.id].pos;
		elem.hitpoints = myUnits[elem.id].hitpoints;
	}

	writer.Write(numStoppingUnits);
	for (size_t i = 0; i < numStoppingUnits; i++)
	{
		auto& elem = writer.Write<Visualization::Removal>();
		elem.id = myStoppingUnits[i];
	}
	myVisualizationSystem.StopWriting();
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

void bs::Field::InitialUpdate(Battle& battle)
{
	ASSERT(myUnits.empty(), "Cannot reuse field");
	myUnits = battle.Get();// std::move(battle.Get());
	for (size_t j = 0; j < myUnits.size(); j++)
	{
		auto& unit = myUnits[j];
		myStartingUnits.emplace_back(unit.id);
	}
	myUnits.reserve(MaxUnits);
}

void bs::Field::FinalUpdate(Battle& battle)
{
	battle.Set(std::move(myUnits));
	battle.totalMilliseconds = static_cast<size_t>(TimePerUpdate.toDouble() * 1000.0 * myTick);
	myUnits = bs::Vector<Unit>();
}

void bs::Field::StopAttacks()
{
	for (size_t j = 0; j < myUnits.size(); j++)
	{
		auto& unit = myUnits[j];
		unit.nextAttackAllowed = Unit::InvalidTick;
	}
}

void bs::Field::Shoot(const Unit& unit)
{
	Unit::Id attackId;
	if (!myFreeUnitIds.empty())
	{
		attackId = myFreeUnitIds.back();
		myFreeUnitIds.pop_back();
	}
	else
	{
		attackId = myUnits.size();
		if (attackId == MaxUnits)
		{
			return;
		}
		myUnits.emplace_back(Unit());
		myUnits.back().id = attackId;
	}

	myStartingUnits.emplace_back(attackId);
	Unit& attack = myUnits[attackId];
	attack.type = Unit::Type::Projectile;

	Vec aimDir = unit.aimTarget - unit.pos;
	aimDir.normalize();

	myRand = sa::math::rand(myRand);
	Real errorX(static_cast<int32_t>(myRand & 0xFFFF) - (0xFFFF/2), 0x10000);
	Real errorY((static_cast<int32_t>(myRand >> 16) & 0xFFFF) - (0xFFFF / 2), 0x10000);
	
	aimDir.x += errorX * Real(40, 100);
	aimDir.y += errorY * Real(40, 100);
	aimDir.normalize();

	attack.pos = unit.pos + (aimDir * unit.radius);
	attack.vel = aimDir * Real(100, 1);
	attack.hitpoints = 20;
	attack.radius = Real(1, 10);
	attack.team = unit.team;
	attack.nextAttackAllowed = Unit::InvalidTick;
}