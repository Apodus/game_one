#pragma once

#include "Types.hpp"
#include "Vector.hpp"

namespace bs
{
	struct Visualization
	{
		struct Addition
		{
			Real radius;
			Unit::Id id;
			uint8_t team;
		};

		struct Movement
		{
			Vec pos;
			Real angle;
			U32 hitpoints;
			Unit::Id id;
		};

		struct Removal
		{
			Unit::Id id;
		};

		struct Hitpoints
		{
			Unit::Id id;
			uint8_t newValue;
		};
	};
}