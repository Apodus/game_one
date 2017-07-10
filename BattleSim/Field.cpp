#include "stdafx.h"
#include "Field.hpp"
#include "Battle.h"
#include "FixedPoint.h"

#define VECTORIZATION_ENABLED

const bs::Real bs::Field::TimePerUpdate = static_cast<bs::Real>(bs::Fraction(100, 1000));

namespace
{
	const size_t MaxUpdates = static_cast<int>(bs::Real(15 * 60) / bs::Field::TimePerUpdate);
	constexpr size_t MaxUnits = 20000;
}

bs::Field::Field(StreamingMode streaming) : myStreaming(streaming)
{
	myWorldProcessor = (WorldProcessor*)_aligned_malloc(sizeof(WorldProcessor), 16);
}

bs::Field::~Field()
{
	_aligned_free(myWorldProcessor);
}

void bs::Field::UpdatePriorities()
{
	// TODO: Can refactor unitUpdatePriorities out, store priority to unit data
	Vector<U16> unitUpdatePriorities;
	unitUpdatePriorities.resize(myUnits.size());
	for (size_t j = 0; j < static_cast<size_t>(Unit::Type::Count); j++)
	{
		for (size_t i = 0; i < myActiveUnits[j].size(); i++)
		{
			auto& unit = myUnits[myActiveUnits[j][i]];
			if (j == static_cast<size_t>(Unit::Type::Character))
			{
				Real rlen = ((unit.moveTarget - unit.pos).lengthSquared() - unit.radius) / Real(1 << 10);
				int len = static_cast<int>(rlen);
				if (len < 1)
				{
					len = 1;
				}
				ASSERT(len <= UINT16_MAX, "Too low priority");
				unitUpdatePriorities[myActiveUnits[j][i]] = static_cast<U16>(len);
			}
			else
			{
				unitUpdatePriorities[myActiveUnits[j][i]] = 0;
			}
		}
		std::sort(myActiveUnits[j].begin(), myActiveUnits[j].end(), [&](Unit::Id a, Unit::Id b)
		{
			return unitUpdatePriorities[a] < unitUpdatePriorities[b];
		});
	}
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

	ASSERT(bb.left >= 0, "Out of range"); // TODO: Set proper range
	ASSERT(bb.top >= 0, "Out of range"); // TODO: Set proper range
	ASSERT(bb.bottom <= 1000, "Out of range");
	ASSERT(bb.right <= 1000, "Out of range");
	ASSERT(bb.top <= bb.bottom, "Invalid bounding box");
	ASSERT(bb.left <= bb.right, "Invalid bounding box");
	for (U32 y = bb.top; y <= bb.bottom; y++)
	{
		for (U32 x = bb.left; x <= bb.right; x++)
		{
			auto& list = myLevels[0].GetUnitList(x, y);
			for (size_t i = 0; i < list.size(); i++)
			{
				if (list.at(i) != id)
				{
					collisions.emplace_back(list.at(i));
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
	bb.left = myLevels[0].POS(unit.pos.x - range);
	bb.top = myLevels[0].POS(unit.pos.y - range);
	bb.right = myLevels[0].POS(unit.pos.x + range);
	bb.bottom = myLevels[0].POS(unit.pos.y + range);
	auto closest = FindClosestEnemy(unit, bb, range);
	return closest;
}

bs::Unit::Id bs::Field::FindClosestEnemy(const Unit& unit, const BoundingBox& bb, Real& range) const
{
	range = range * range;
	Unit::Id otherId = Unit::InvalidId;
	for (U32 y = bb.top; y <= bb.bottom; y++)
	{
		for (U32 x = bb.left; x <= bb.right; x++)
		{
			FindClosestEnemy(unit, x, y, range, otherId);
		}
	}
	range = sa::math::sqrt(range);
	return otherId;
}

void bs::Field::FindClosestEnemy(const Unit& unit, U32 x, U32 y, Real& range2, Unit::Id& otherId) const
{
	auto& list = myLevels[0].GetUnitList(x, y);
	for (size_t i = 0; i < list.size(); i++)
	{
		auto& otherUnit = myUnits[list[i]];
		if (otherUnit.team != unit.team)
		{
			auto radius = unit.radius + otherUnit.radius;
			auto len = (otherUnit.pos - unit.pos).lengthSquared() - (radius*radius);
			if (len < range2)
			{
				range2 = len;
				otherId = list[i];
			}
		}
	}
}


bool bs::Field::Update()
{
	UpdatePriorities();

	// Movement
	Array<Vector<Unit::Id>, 2> killed;
	killed[0].reserve(16);
	killed[1].reserve(16);
	Vector<Unit::Id> collisions;
	collisions.reserve(16);

	for (size_t type = 0; type < 2; type++)
	{
		for (size_t start = 0; start < myActiveUnits[type].size(); start += 4)
		{
			size_t end = std::min(start + 3, myActiveUnits[type].size() - 1);
			bs::U32 unitIds[4] = { myActiveUnits[type][start],
				myActiveUnits[type][std::min(start + 1, end)],
				myActiveUnits[type][std::min(start + 2, end)],
				myActiveUnits[type][std::min(start + 3, end)] };
#ifdef VECTORIZATION_ENABLED
			Vec targetDirs[4];
			memset(targetDirs, 0x0, sizeof(Vec) * 4);
			Vec aimDirs[4];

			if (type == static_cast<size_t>(Unit::Type::Character))
			{
				myWorldProcessor->myPositions.Store(
					myUnits[unitIds[0]].pos,
					myUnits[unitIds[1]].pos,
					myUnits[unitIds[2]].pos,
					myUnits[unitIds[3]].pos);

				myWorldProcessor->myMoveTargets.Store(
					myUnits[unitIds[0]].moveTarget,
					myUnits[unitIds[1]].moveTarget,
					myUnits[unitIds[2]].moveTarget,
					myUnits[unitIds[3]].moveTarget);

				myWorldProcessor->myAimTargets.Store(
					myUnits[unitIds[0]].aimTarget,
					myUnits[unitIds[1]].aimTarget,
					myUnits[unitIds[2]].aimTarget,
					myUnits[unitIds[3]].aimTarget);

				myWorldProcessor->Run();

				myWorldProcessor->myMoveTargetDirs.Load(targetDirs);
				myWorldProcessor->myAimTargetDirs.Load(aimDirs);
			}
#endif
			for (size_t i = start; i <= end; i++)
			{
				auto& unit = myUnits[unitIds[i - start]];

				bs::Real Speed;
				const bs::Real SlowDown = static_cast<bs::Real>(bs::Fraction(1, 2));
				Vec targetDir;
				bool hasTargetAngle = true;

				if (unit.hitpoints > 0)
				{
					if (type == static_cast<size_t>(Unit::Type::Character))
					{
						Speed = Real(3);
#ifdef VECTORIZATION_ENABLED
						targetDir = targetDirs[i - start];
						auto targetAngle = TargetAngleGet(aimDirs[i - start]);
#else
						targetDir = unit.moveTarget - unit.pos;
						auto targetDirLen = core::sqrt(targetDir.x * targetDir.x + targetDir.y * targetDir.y);
						if (targetDirLen > Real(0))
						{
							targetDir.x /= targetDirLen;
							targetDir.y /= targetDirLen;
							targetDir.z /= targetDirLen;
						}
						else
						{
							targetDir.x = 0;
							targetDir.y = 0;
							targetDir.z = 0;
						}
						auto aimDir = unit.aimTarget - unit.pos;
						auto aimDirLen = core::sqrt(aimDir.x * aimDir.x + aimDir.y * aimDir.y);
						if (aimDirLen > Real(0))
						{
							aimDir.x /= aimDirLen;
							aimDir.y /= aimDirLen;
							aimDir.z /= aimDirLen;
						}
						else
						{
							aimDir.x = 0;
							aimDir.y = 0;
							aimDir.z = 0;
						}
						auto targetAngle = TargetAngleGet(aimDir);
#endif

						auto angleDelta = core::Math::WrappedDelta<bs::Real>(unit.angle, targetAngle, bs::Real(180));
						if (angleDelta > Real(8) || angleDelta < Real(-8))
						{
							hasTargetAngle = false;
							if (angleDelta > Real(0))
							{
								if (angleDelta > Real(4))
								{
									angleDelta = Real(4);
								}
							}
							else
							{
								if (angleDelta < -Real(4))
								{
									angleDelta = -Real(4);
								}
							}
							unit.angle -= angleDelta;

							if (unit.angle < Real(-180))
							{
								unit.angle = Real(180) + (unit.angle - Real(-180));
							}
							else if (unit.angle > Real(180))
							{
								unit.angle = Real(-180) - (unit.angle - Real(180));
							}
						}
						else
						{
							unit.angle = targetAngle;
						}
						// assert(unit.angle >= Real(-180));
						// assert(unit.angle <= Real(180));

						const bs::Real Agility(10);
						unit.acc = targetDir * Agility;
					}
					else
					{
						Speed = Real(10);
					}
				}
				else
				{
					unit.acc = bs::Vec();
					Speed = Real(0);
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
					if (type == static_cast<size_t>(Unit::Type::Character))
					{
						Vec dir = other.pos - newPos;
						auto len = dir.length();
						if (len > static_cast<Real>(Fraction(1, 16384)))
						{
							dir.x /= len;
							dir.y /= len;
							dir.z /= len;
							auto dp = dir.dotProduct(targetDir);
							newVel.x = -(dp * unit.vel.x) / Real(2);
							newVel.y = -(dp * unit.vel.y) / Real(2);
							newVel.z = (Real(0));
						}
					}
					else
					{
						newVel = Vec();
						unit.receivedDamage = unit.hitpoints;
						if (other.team != unit.team)
						{
							myRand = sa::math::rand(myRand);
							if ((myRand & 1) == 1)
							{
								other.receivedDamage += 10;
							}
						}
					}
				}

				unit.vel = newVel;

				bool isMoved = false;

				if (unit.type == Unit::Type::Projectile)
				{
					if (unit.hitpoints != 0)
					{
						unit.receivedDamage++;
						unit.pos = newPos;
						isMoved = true;
					}
				}
				else
				{
					if ((unit.pos - newPos).lengthSquared() > Fraction(1, 16384))
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
				}

				if (unit.hitpoints != 0)
				{
					if (myTick >= unit.nextAttackAllowed)
					{
						if (hasTargetAngle)
						{
							unit.nextAttackAllowed = myTick + (unit.weaponId == 1 ? 1 : 3);
							const size_t numShots = unit.weaponId == 1 ? 5 : 1;
							for (size_t k = 0; k < numShots; k++)
							{
								Shoot(unit);
							}
						}
					}
				}

				if (!isMoved && unit.hitpoints == 0)
				{
					// Add to killed list only after dead and movement stopped.
					if (unit.timerId != InvalidTimer)
					{
						myTimerSystem.Cancel(unit.timerId);
					}
					killed[type].emplace_back(unit.id);
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
			}
		}
	}

	UpdateDecisions();

	ActivateStartingUnits();

	myTick++;

	// Remove killed from active
	// TODO: Go through all active and add to stopped instead of killed list
	bool isActive = myTick < MaxUpdates;
	for (size_t j = 0; j < 2; j++)
	{
		for (size_t i = 0; i < killed[j].size(); i++)
		{
			auto iter = std::find(myActiveUnits[j].begin(), myActiveUnits[j].end(), killed[j][i]);
			ASSERT(iter != myActiveUnits[j].end(), "Killed not found");
			auto& unit = myUnits[*iter];
			if (j == static_cast<size_t>(Unit::Type::Character))
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
				myFreeUnitIds.Free(unit.id);
			}
			*iter = myActiveUnits[j].back();
			myActiveUnits[j].pop_back();
		}
		// myActiveUnits.erase(iter);
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
		elem.angle = myUnits[elem.id].angle;
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
// http://www.gamasutra.com/view/feature/131424/pool_hall_lessons_fast_accurate_.php?print=1
bool bs::Field::CollisionCheck(const Unit& a, const Unit& b, const Vec& endPos, Vec& N)
{
	Real radii = a.radius + b.radius;

	Real left = b.pos.x < endPos.x ? b.pos.x - radii : endPos.x - radii;
	Real right = b.pos.x > endPos.x ? b.pos.x + radii : endPos.x + radii;
	Real top = b.pos.y < endPos.y ? b.pos.y - radii : endPos.y - radii;
	Real bottom = b.pos.y > endPos.y ? b.pos.y + radii : endPos.y + radii;
	if (a.pos.x < left || a.pos.x > right || a.pos.y < top || a.pos.y > bottom)
	{
		// Not in bounding box
		return false;
	}

	/* movement vector */
	N = endPos - b.pos;
	Real length_N = N.length(); // TODO 2d vec
	if (length_N == Real(0))
	{
		// Not moving, TODO: should probably assert here
		return false;
	}

	Vec C = a.pos - b.pos;
	C.z = Real(0); /* Ignore Z: used for sphere collision */
	Real length_C = core::sqrt((C.x*C.x + C.y*C.y));

	if (length_C == Real(0))
	{
		if (b.type == Unit::Type::Projectile)
		{
			N = (a.pos + b.pos) / Real(2);
			return true;
		}
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
	N.z = Real(0);

	Real D = N.dotProduct(C);
	if (D <= Real(0))
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

	// F and min dist form a right triangle (minDist the hypotenuse) with a
	// third line we'll call square of T.
	Real T = (radii2)-F;
	if (T < Real(0))
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
	const Vec Offset(Real(1000), Real(1000), Real(0));

	const Real Distance(20);

	ASSERT(myUnits.empty(), "Cannot reuse field");
	myUnits = battle.Get();
	for (size_t j = 0; j < myUnits.size(); j++)
	{
		auto& unit = myUnits[j];
		auto id = myFreeUnitIds.Reserve();
		ASSERT(id == unit.id, "Invalid id given;id=%u;expected=%u", unit.id, id);

		unit.pos.y += Distance;
		if (unit.team == 1)
		{
			unit.pos.y = -unit.pos.y;
			unit.pos.x = -unit.pos.x;
			unit.angle = -Real(180);
		}
		unit.pos += Offset;

		unit.moveTarget = unit.pos;
		unit.aimTarget = unit.pos;
		myStartingUnits.emplace_back(unit.id);
	}
	myUnits.reserve(MaxUnits);
}

void bs::Field::FinalUpdate(Battle& battle)
{
	battle.Set(std::move(myUnits));
	battle.totalMilliseconds = static_cast<size_t>(static_cast<double>(TimePerUpdate) * 1000.0 * myTick);
	myUnits = bs::Vector<Unit>();
}

void bs::Field::StopAttacks()
{
	for (size_t j = 0; j < myUnits.size(); j++)
	{
		auto& unit = myUnits[j];
		unit.nextAttackAllowed = InvalidTick;
	}
}

void bs::Field::Shoot(const Unit& unit)
{
	Vec aimDir = unit.aimTarget - unit.pos;
	auto aimLen = aimDir.length();
	if (aimLen <= Fraction(1, 1000))
	{
		return;
	}

	Unit::Id attackId = myFreeUnitIds.Reserve();
	if (attackId == myUnits.size())
	{
		if (attackId == MaxUnits)
		{
			myFreeUnitIds.Free(attackId);
			return;
		}
		myUnits.emplace_back(Unit());
		myUnits.back().id = attackId;
	}

	aimDir.x /= aimLen;
	aimDir.y /= aimLen;
	aimDir.z /= aimLen;

	myStartingUnits.emplace_back(attackId);
	Unit& attack = myUnits[attackId];
	attack.type = Unit::Type::Projectile;

	myRand = sa::math::rand(myRand);
	Fraction errorX(static_cast<int32_t>(myRand & 0xFFFF) - (0xFFFF / 2), 0x10000);
	Fraction errorY((static_cast<int32_t>(myRand >> 16) & 0xFFFF) - (0xFFFF / 2), 0x10000);

	aimDir.x += static_cast<Real>(errorX) * static_cast<Real>(Fraction(40, 100));
	aimDir.y += static_cast<Real>(errorY) * static_cast<Real>(Fraction(40, 100));
	aimDir.normalize();

	attack.radius = static_cast<Real>(Fraction(1, 10));
	attack.pos = unit.pos + (aimDir * (unit.radius + Real(2)*attack.radius));
	attack.vel = aimDir * Real(100);
	attack.hitpoints = 20;
	attack.team = unit.team;
	attack.nextAttackAllowed = InvalidTick;
}

void bs::Field::ActivateStartingUnits()
{
	for (size_t i = 0; i < myStartingUnits.size(); i++)
	{
		auto& unit = myUnits[myStartingUnits[i]];
		if (unit.type == Unit::Type::Character)
		{
			myLevels[0].AddUnit(unit);
			myTeamUnitsLeft[unit.team]++;
			unit.timerId = myTimerSystem.Create(unit.id, myTick + 1);
		}
		myActiveUnits[static_cast<size_t>(unit.type)].emplace_back(myStartingUnits[i]);

		if (IsStreaming())
		{
			myMovingUnits.emplace_back(myStartingUnits[i]);
		}
	}
}

void bs::Field::UpdateDecisions()
{
	myTimerSystem.Update(myTick, [&](Unit::Id id)
	{
		auto& unit = myUnits[id];
		ASSERT(unit.timerId != InvalidTimer, "Unit has no timer");

		myRand = sa::math::rand(myRand);
		Real range = static_cast<Real>(Fraction(myRand % 9000 + 1000, 100)) + unit.radius;

		auto closest = FindClosestEnemy(unit, range);
		if (closest != Unit::InvalidId)
		{
			unit.aimTarget = myUnits[closest].pos;
			unit.moveTarget = myUnits[closest].pos;
			unit.timerId = myTimerSystem.Create(id, myTick + 10);
		}
		else
		{
			unit.timerId = myTimerSystem.Create(id, myTick + 5);
		}
	});
}

bs::Real bs::Field::TargetAngleGet(Vec& aimDir) const
{
	Real angle = core::atan2(aimDir.x, aimDir.y) * Real(180) / core::Math::Pi();
	return angle;
}
