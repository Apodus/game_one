#pragma once

#include "util/fixedpoint.hpp"

namespace bs
{
	typedef ::FixedPoint<1024 * 128> Real;

	struct Vec
	{
		Real x;
		Real y;
		Real z;
	};

	typedef uint64_t U64;
	typedef uint32_t U32;
	typedef uint16_t U16;

}