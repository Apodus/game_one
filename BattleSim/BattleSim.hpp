#pragma once

#include "Field.hpp"
#include "Battle.h"

#include <mutex>
#include <atomic>

namespace bs
{
	class BattleSim
	{
	public:

		// Note: ownership of 'battle' is given to BattleSim until IsComplete() is true
		BATTLESIM_API BattleSim(Battle& battle, Field::StreamingMode mode = Field::StreamingMode::Enabled);

		BATTLESIM_API ~BattleSim();

		// Resolves given battle asynchronously. 
		// Simulation will run max given milliseconds forward until Simulate() needs to be called
		BATTLESIM_API void ResolveAsync(size_t milliseconds);
		
		// Resolves given battle. 
		BATTLESIM_API void Resolve();

		// Move simulation forward given number of milliseconds, only needed for manual mode
		BATTLESIM_API void Simulate(size_t milliSeconds);

		static BATTLESIM_API Battle Generate();

		BATTLESIM_API Field& GetField() { return myField; }

		BATTLESIM_API double GetTimePerUpdate() const;

		BATTLESIM_API bool IsComplete() const { return !myActiveFlag; }

		BATTLESIM_API void Cancel(); // Cancels simulation

	private:		
		void Run();

		void RunUntilComplete();

		Field myField;

		double myTotalTime = 0; // Total time simulated
		double myTimeAccu = 0; // Time requested to be simulated, but could not fit in time step

		Battle& myBattle;
		std::thread myThread;
		std::mutex myMutex;
		std::condition_variable myCV;
		std::atomic<bool> myActiveFlag;
		size_t myTimeToSimulate;
	};
}