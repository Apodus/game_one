#pragma once


#pragma once

#include "Field.hpp"
#include "BattleSim.hpp"

#include <mutex>
#include <atomic>

namespace bs
{
	class BattleSimAsync : public BattleSim
	{
	public:
		BATTLESIM_API BattleSimAsync(Battle& battle);

		BATTLESIM_API ~BattleSimAsync();

		// Resolves given battle asynchronously. 
		// Simulation will run max given milliseconds forward until Simulate() needs to be called
		BATTLESIM_API void Resolve(size_t milliseconds);

		// Move simulation forward given number of milliseconds, only needed for manual mode
		BATTLESIM_API void Simulate(size_t milliSeconds);

		BATTLESIM_API void Cancel(); // Cancels simulation

	private:
		void Run();

		std::thread myThread;
		std::mutex myMutex;
		std::condition_variable myCV;

		size_t myTimeToSimulate;
		double myTotalTime = 0; // Total time simulated
		double myTimeAccu = 0; // Time requested to be simulated, but could not fit in time step


	};
}

