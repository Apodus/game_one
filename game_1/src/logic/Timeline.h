#pragma once

#include "WorldEvent.h"

#include <deque>
#include <vector>

class World;

// Records game so that it's possible to rollback physics or play replays.
class Timeline
{
public:
	Timeline(World& world);
	void Run(uint64_t time);
	void AddEvent(WorldEvent* record, uint64_t time);

	struct Record
	{
		std::vector<WorldEvent*> events;
		uint64_t time;
	};

	uint64_t GetTime() const { return myCurrentTime; }

private:
	void Simulate(uint64_t ttime);
	void OpenRecords();
	void CloseRecords();

	World& myWorld;
	std::deque<Record> myRecords;
	size_t myRecordPos;
	uint64_t myCurrentTime;
	uint32_t mySimulationAccumulator;
};
