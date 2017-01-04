#pragma once

#include "util/fixedpoint.hpp"
#include "util/vec3.hpp"

namespace bs
{
	typedef ::FixedPoint<1024 * 64> Real;
	typedef ::sa::vec3<Real> Vec;
	typedef uint64_t U64;
	typedef uint32_t U32;
	typedef uint16_t U16;


}

namespace sa
{
	namespace math
	{
		// http://stackoverflow.com/questions/1100090/looking-for-an-efficient-integer-square-root-algorithm-for-arm-thumb2
		inline bs::Real sqrt(const bs::Real& val)
		{
#if 0
			int64_t op = val.getRawValue();
			int64_t res = 0;
			int64_t one = bs::Real::s_fpOne; // The second-to-top bit is set: use 1u << 14 for uint16_t type; use 1uL<<30 for uint32_t type
									  
			// "one" starts at the highest power of four <= than the argument.
			while (one > op)
			{
				one >>= 2;
			}

			while (one != 0)
			{
				if (op >= res + one)
				{
					op = op - (res + one);
					res = res + 2 * one;
				}
				res >>= 1;
				one >>= 2;
			}
			return bs::Real(static_cast<int64_t>(res), static_cast<int64_t>(1));
#else
			return squareRoot(val);
#endif
		}
	}
}