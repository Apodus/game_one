#pragma once

#include "Level.hpp"
#include "Array.hpp"
#include "Deque.hpp"
#include "VisualizationSystem.h"
#include "Visualization.h"
#include "TimerSystem.h"
#include "WorldProcessor.h"

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
		~Field();

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

		size_t GetTick() const 
		{
			return myTick;
		}

	private:
		void WriteUpdate();

		void ActivateStartingUnits();

		void UpdatePriorities();

		void UpdateDecisions();

		void FindCollisions(const Unit& unit, Unit::Id id, const Vec& newPos, std::vector<Unit::Id>& collisions) const;

		Unit::Id FindClosestEnemy(const Unit& unit, const BoundingBox& bb, Real& range) const;

		void FindClosestEnemy(const Unit& unit, U32 x, U32 y, Real& range, Unit::Id& otherId) const;

		bool CollisionCheck(const Unit& a, const Unit& b, const Vec& endPos, Vec& hitPos);

		void StopAttacks();

		void Shoot(const Unit& unit);

		Real TargetAngleGet(Vec& aimDir) const;

		VisualizationSystem myVisualizationSystem;


		WorldProcessor* myWorldProcessor;
		// Field is divided into levels in depth-axis. 
		// E.g. level 0 has underground units, level 1 has ground units and level 2 has flying units
		Array<Level, 1> myLevels;
		Array<size_t, MaxTeams> myTeamUnitsLeft;
		
		Array<Vector<Unit::Id>, 2> myActiveUnits;

		Vector<Unit::Id> myStartingUnits;
		Vector<Unit::Id> myMovingUnits;
		Vector<Unit::Id> myStoppingUnits;
		IdPool<Unit::Id> myFreeUnitIds;
		Vector<Unit> myUnits;
		TimerSystem<Unit::Id> myTimerSystem;
		uint64_t myRand = 2;
		Tick myTick = 0;
		StreamingMode myStreaming; // Is frame streaming enabled
	
	};
}