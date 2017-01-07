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

		Unit::Id Add(Unit& unit);

		void Update();

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

		bool CollisionCheck(const Unit& a, const Unit& b, const Vec& endPos, Vec& hitPos);

		// Field is divided into levels in depth-axis. 
		// E.g. level 0 has underground units, level 1 has ground units and level 2 has flying units
		Array<Level, 1> myLevels;
		Vector<Unit::Id> myActiveUnits;
		Vector<Unit> myUnits;
		Deque<Frame> myFrames;
		double myFrameTime = 0;
		uint64_t myRand = 1;
		
		
	};
}