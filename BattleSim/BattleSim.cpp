// BattleSim.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "BattleSim.hpp"

#include <chrono>
#include <atomic>

bs::BattleSim::BattleSim(Battle& battle, Field::StreamingMode mode) :
	myBattle(battle), myField(mode), myActiveFlag(false)
{
}

bs::BattleSim::~BattleSim()
{
}

bs::Battle bs::BattleSim::Generate()
{
	const Vec Offset(Real(10, 1), Real(10, 1), Real(0, 1));

	Battle battle;
	for (int64_t y = 0; y < 10; y++)
	{
		for (int64_t i = 0; i < 100; i++)
		{
			{
				Unit u;
				if (i % 10 == 0 && y == 0)
				{
					u.radius = Real(10, 10);
					u.hitpoints = 100;
					u.weaponId = 1;
				}
				else
				{
					u.radius = Real(4, 10);
					u.hitpoints = 5;
				}
				u.pos.set(Real(50 + i), Real(50 + y), Real(0));
				u.moveTarget.set(Real(50 + i), Real(75), Real(0));
				u.aimTarget.set(Real(50 + i), Real(175), Real(0));

				u.pos += Offset;
				u.moveTarget += Offset;
				u.aimTarget += Offset;

				u.team = 0;
				u.id = battle.NumUnits();
				battle.Add(u);
			}

			{
				Unit u;
				if (i % 10 == 0 && y == 9)
				{
					u.radius = Real(10, 10);
					u.hitpoints = 100;
					u.weaponId = 1;
				}
				else
				{
					u.radius = Real(4, 10);
					u.hitpoints = 5;
				}
				u.pos.set(Real(50 + i), Real(100 + y), Real(0));
				u.moveTarget.set(Real(50 + i), Real(75), Real(0));
				u.aimTarget.set(Real(50 + i), Real(0), Real(0));

				u.pos += Offset;
				u.moveTarget += Offset;
				u.aimTarget += Offset;

				u.team = 1;
				u.id = battle.NumUnits();
				battle.Add(u);
			}
		}
	}
	return battle;
}


void bs::BattleSim::Resolve()
{
	myField.InitialUpdate(myBattle);
	RunUntilComplete();
	myField.FinalUpdate(myBattle);
}

void bs::BattleSim::RunUntilComplete()
{
	myActiveFlag = true;
	ASSERT(!myField.IsStreaming(), "Do not use streaming when doing synchronous resolve!");
	while (myActiveFlag)
	{
		if (!myField.Update())
		{
			myActiveFlag = false;
		}
	}
}

double bs::BattleSim::GetTimePerUpdate() const
{
	return myField.TimePerUpdate.toDouble();
}

void bs::BattleSim::Cancel()
{
	myActiveFlag = false;
}