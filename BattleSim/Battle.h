#pragma once

#include "Types.hpp"
#include "Unit.hpp"
#include "Vector.hpp"

namespace bs
{
	struct Battle
	{
		struct Army
		{
			Vector<Unit> units;
		};
		Vector<Army> armies;
	};
}