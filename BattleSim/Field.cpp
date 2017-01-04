#include "stdafx.h"
#include "Field.hpp"

const bs::Real bs::Field::TimePerUpdate = bs::Real(10, 1000);

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
		myLevels[0].InternalScan(unit.pos, newPos, nullptr, [](int x, int y, void* /*data*/) -> bool
		{
			// TODO
			LOG("FIELD SCAN at %i, %i", x, y);
			return true;
		});

		unit.vel = newVel;
		unit.pos = newPos;
		LOG("[%lu] POS = %f, %f DIR=%f, %f VEL= %f, %f",
			unit.id,
			RToF(unit.pos.x), RToF(unit.pos.y),
			RToF(targetDir.x), RToF(targetDir.y),
			RToF(unit.vel.x), RToF(unit.vel.y));
	}
}