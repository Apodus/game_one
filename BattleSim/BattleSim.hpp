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
		BATTLESIM_API BattleSim(Battle& battle, Field::StreamingMode mode = Field::StreamingMode::Disabled);

		BATTLESIM_API ~BattleSim();
	
		// Resolves given battle. 
		BATTLESIM_API void Resolve();

		static BATTLESIM_API Battle Generate();

		BATTLESIM_API Field& GetField() { return myField; }

		BATTLESIM_API double GetTimePerUpdate() const;

		BATTLESIM_API bool IsComplete() const { return !myActiveFlag; }

		BATTLESIM_API void Cancel(); // Cancels simulation

	protected:		
		void RunUntilComplete();

		Field myField;
		Battle& myBattle;
		volatile std::atomic<bool> myActiveFlag;
	};
}