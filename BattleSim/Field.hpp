#pragma once

#include "Level.hpp"
#include "Array.hpp"
#include "Deque.hpp"

namespace bs
{
	class Unit;
	class Field
	{
	public:
		struct Frame
		{
			Vector<Unit> units;
		};

		Field();

		Unit::Id Add(Unit& unit, Unit::Detail& detail);

		void Update();

		Unit::Id FindClosestEnemy(const Unit& unit, Real& range) const;

		static const Real TimePerUpdate;

		BATTLESIM_API const Frame& GetFrame()
		{
			while (myFrames.size() > 1)
			{
				myFrames.pop_front();
			}
			return myFrames.front();
		}

	private:
		void UpdatePriorities();

		void FindCollisions(const Unit& unit, Unit::Id id, const Vec& newPos, std::vector<Unit::Id>& collisions) const;

		Unit::Id FindClosestEnemy(const Unit& unit, const BoundingBox& bb, Real& range) const;

		void FindClosestEnemy(const Unit& unit, U32 x, U32 y, Real& range, Unit::Id& otherId) const;

		bool CollisionCheck(const Unit& a, const Unit& b, const Vec& endPos, Vec& hitPos);

		// Field is divided into levels in depth-axis. 
		// E.g. level 0 has underground units, level 1 has ground units and level 2 has flying units
		Array<Level, 1> myLevels;
		Vector<Unit::Id> myActiveUnits;
		Vector<Unit> myUnits;
		Vector<Unit::Detail> myUnitDetails;
		Deque<Frame> myFrames;
		double myFrameTime = 0;
		uint64_t myRand = 1;
		
		
	};
}