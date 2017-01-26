#pragma once

#include "Level.hpp"
#include "Array.hpp"
#include "Deque.hpp"
#include "VisualizationSystem.h"
#include "Visualization.h"
#include "TimerSystem.h"

namespace bs
{
	class Unit;
	struct Battle;
	class Field
	{
	public:
		enum StreamingMode // Is frame streaming enabled
		{
			Enabled,
			Disabled
		};

		Field(StreamingMode streaming);

		// Returns true if victory conditions not met.
		bool Update();

		Unit::Id FindClosestEnemy(const Unit& unit, Real& range) const;

		static const Real TimePerUpdate;

		static const size_t MaxTeams = 8;

		BATTLESIM_API const ByteBuffer* GetFrame()
		{	
			return myVisualizationSystem.GetForReading();
		}

		BATTLESIM_API void FreeFrame()
		{
			myVisualizationSystem.ConsumeUpdate();
		}

		bool IsStreaming() const { return myStreaming == StreamingMode::Enabled;  }

		void InitialUpdate(Battle& battle);

		void FinalUpdate(Battle& battle);

	private:
		void WriteUpdate();

		void UpdatePriorities();

		void FindCollisions(const Unit& unit, Unit::Id id, const Vec& newPos, std::vector<Unit::Id>& collisions) const;

		Unit::Id FindClosestEnemy(const Unit& unit, const BoundingBox& bb, Real& range) const;

		void FindClosestEnemy(const Unit& unit, U32 x, U32 y, Real& range, Unit::Id& otherId) const;

		bool CollisionCheck(const Unit& a, const Unit& b, const Vec& endPos, Vec& hitPos);

		void StopAttacks();

		void Shoot(const Unit& unit);

		VisualizationSystem myVisualizationSystem;



		// Field is divided into levels in depth-axis. 
		// E.g. level 0 has underground units, level 1 has ground units and level 2 has flying units
		Array<Level, 1> myLevels;
		Array<size_t, MaxTeams> myTeamUnitsLeft;
		Vector<Unit::Id> myActiveUnits;
		Vector<Unit::Id> myStartingUnits;
		Vector<Unit::Id> myMovingUnits;
		Vector<Unit::Id> myStoppingUnits;
		IdPool<Unit::Id> myFreeUnitIds;
		Vector<Unit> myUnits;
		TimerSystem<Unit::Id> myTimerSystem;
		uint64_t myRand = 1;
		Unit::Tick myTick = 0;
		StreamingMode myStreaming; // Is frame streaming enabled
	
	};
}