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
	Battle battle;
	size_t guid = 0;
	for (int64_t y = 0; y < 10; y++)
	{
		for (int64_t i = 0; i < 100; i++)
		{
			{
				Unit u;
				if (i % 10 == 0 && y == 0)
				{
					u.radius = Real(10, 10);
					u.hitpoints = 10;
				}
				else
				{
					u.radius = Real(4, 10);
					u.hitpoints = 5;
				}
				u.pos.set(Real(50 + i), Real(50 + y), Real(0));
				u.moveTarget.set(Real(50 + i + (rand() % 4)), Real(75), Real(0));
				u.team = 0;
				battle.Add(u, guid);
				guid++;
			}

			{
				Unit u;
				if (i % 10 == 0 && y == 9)
				{
					u.radius = Real(10, 10);
					u.hitpoints = 10;
				}
				else
				{
					u.radius = Real(4, 10);
					u.hitpoints = 5;
				}
				u.pos.set(Real(50 + i), Real(100 + y), Real(0));
				u.moveTarget.set(Real(50 + i + (rand() % 4)), Real(75), Real(0));
				u.team = 1;
				battle.Add(u, guid);
				guid++;
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