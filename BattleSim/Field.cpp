#include "stdafx.h"
#include "Field.hpp"

const bs::Real bs::Field::TimePerUpdate = bs::Real(100, 1000);

bs::Unit::Id bs::Field::Add(Unit& unit)
{
	unit.id = myUnits.size();
	myUnits.emplace_back(unit);
	myLevels[0].AddUnit(unit);
	myActiveUnits.emplace_back(unit.id);
	return unit.id;
}

// Real to Float conversion only for logging purposes
double bs::Field::RToF(const Real& real)
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
		targetDir.normalize();
		const bs::Real Agility(500);
		const bs::Real Speed(500);
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
		
		if (collisions.size() > 0)
		{
			// Stupid collision checking, so really not checking collision
			newPos = unit.pos;
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
		/*LOG("[%lu] POS = %f, %f DIR=%f, %f VEL= %f, %f",
			unit.id,
			RToF(unit.pos.x), RToF(unit.pos.y),
			RToF(targetDir.x), RToF(targetDir.y),
			RToF(unit.vel.x), RToF(unit.vel.y));*/
	}
}