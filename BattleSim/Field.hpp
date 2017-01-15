#pragma once

#include "Level.hpp"
#include "Array.hpp"
#include "Deque.hpp"
#include "VisualizationSystem.h"

namespace bs
{
	class Unit;
	class Field
	{
	public:
		Field();

		Unit::Id Add(Unit& unit);

		void Update();

		Unit::Id FindClosestEnemy(const Unit& unit, Real& range) const;

		static const Real TimePerUpdate;

		BATTLESIM_API const Visualization* GetFrame()
		{	
			return myVisualizationSystem.GetForReading();
		}

		BATTLESIM_API void FreeFrame()
		{
			myVisualizationSystem.ConsumeUpdate();
		}

	private:
		void WriteUpdate();

		void UpdatePriorities();

		void FindCollisions(const Unit& unit, Unit::Id id, const Vec& newPos, std::vector<Unit::Id>& collisions) const;

		Unit::Id FindClosestEnemy(const Unit& unit, const BoundingBox& bb, Real& range) const;

		void FindClosestEnemy(const Unit& unit, U32 x, U32 y, Real& range, Unit::Id& otherId) const;

		bool CollisionCheck(const Unit& a, const Unit& b, const Vec& endPos, Vec& hitPos);

		VisualizationSystem myVisualizationSystem;

		// Field is divided into levels in depth-axis. 
		// E.g. level 0 has underground units, level 1 has ground units and level 2 has flying units
		Array<Level, 1> myLevels;
		Vector<Unit::Id> myActiveUnits;
		Vector<Unit::Id> myStartingUnits;
		Vector<Unit::Id> myMovingUnits;
		Vector<Unit> myUnits;
		uint64_t myRand = 1;
	
	};
}