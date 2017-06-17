#pragma once

#include "Types.hpp"

namespace bs
{
	// http://turner.faculty.swau.edu/mathematics/materialslibrary/pi/pirat.html
	const static bs::Real PI = bs::Real(1146408, 364913);

	class AngleUtil
	{
	public:
		static const size_t Granularity = 50;

		AngleUtil()
		{
			int32_t mid = Granularity / 2;
			for (int32_t x = 0; x <= Granularity; x++)
			{
				for (int32_t y = 0; y <= Granularity; y++)
				{
					Vec pos;
					pos.x = Real(x - mid, mid);
					pos.y = Real(y - mid, mid);

				}
			}
		}

		bs::Real GetAngle(const Vec& dir) const
		{
			return Atan2(dir.y, dir.x);// *Real(180) / PI;
		}

		Real Atan2(Real x, Real y) const
		{
			int xi = x * Real(INT16_MAX);
			int yi = y * Real(INT16_MAX);
			int16_t at2 = fxpt_atan2(static_cast<int16_t>(xi), static_cast<int16_t>(yi));
			return Real(at2, 32768);
		}

		static inline int16_t s16_nabs(const int16_t j)
		{
#if (((int16_t)-1) >> 1) == ((int16_t)-1)
			// signed right shift sign-extends (arithmetic)
			const int16_t negSign = ~(j >> 15); // splat sign bit into all 16 and complement
												// if j is positive (negSign is -1), xor will invert j and sub will add 1
												// otherwise j is unchanged
			return (j ^ negSign) - negSign;
#else
			return (j < 0 ? j : -j);
#endif
		}

		static inline int16_t q15_div(const int16_t numer, const int16_t denom) {
			return ((int32_t)numer << 15) / denom;
		}

		static inline int16_t q15_mul(const int16_t j, const int16_t k) {
			const int32_t intermediate = j * (int32_t)k;
#if 0 // don't round
			return intermediate >> 15;
#elif 0 // biased rounding
			return (intermediate + 0x4000) >> 15;
#else // unbiased rounding
			return static_cast<int16_t>((intermediate + ((intermediate & 0x7FFF) == 0x4000 ? 0 : 0x4000)) >> 15);
#endif
		}

		// TODO: Get rid of doubles here...
		static inline int16_t q15_from_double(const double d) {
			return static_cast<int16_t>(lrint(d * 32768));
		}

		// https://geekshavefeelings.com/posts/fixed-point-atan2
		/**
		 * 16-bit fixed point four-quadrant arctangent. Given some Cartesian vector
		 * (x, y), find the angle subtended by the vector and the positive x-axis.
		 *
		 * The value returned is in units of 1/65536ths of one turn. This allows the use
		 * of the full 16-bit unsigned range to represent a turn. e.g. 0x0000 is 0
		 * radians, 0x8000 is pi radians, and 0xFFFF is (65535 / 32768) * pi radians.
		 *
		 * Because the magnitude of the input vector does not change the angle it
		 * represents, the inputs can be in any signed 16-bit fixed-point format.
		 *
		 * @param y y-coordinate in signed 16-bit
		 * @param x x-coordinate in signed 16-bit
		 * @return angle in (val / 32768) * pi radian increments from 0x0000 to 0xFFFF
		 */

		 // TODO: Get rid of this thing
#define M_1_PI 0.31830988618379067154

		static uint16_t fxpt_atan2(const int16_t y, const int16_t x)
		{
			if (x == y)
			{ // x/y or y/x would return -1 since 1 isn't representable
				if (y > 0)
				{ // 1/8
					return 8192;
				}
				else if (y < 0)
				{ // 5/8
					return 40960;
				}
				else
				{ // x = y = 0
					return 0;
				}
			}
			const int16_t nabs_y = s16_nabs(y), nabs_x = s16_nabs(x);
			if (nabs_x < nabs_y)
			{ // octants 1, 4, 5, 8
				const int16_t y_over_x = q15_div(y, x);
				const int16_t correction = q15_mul(q15_from_double(0.273 * M_1_PI), s16_nabs(y_over_x));
				const int16_t unrotated = q15_mul(q15_from_double(0.25 + 0.273 * M_1_PI) + correction, y_over_x);
				if (x < 0)
				{ // octants 1, 8
					return unrotated;
				}
				else { // octants 4, 5
					return 32768 + unrotated;
				}
			}
			else
			{ // octants 2, 3, 6, 7
				const int16_t x_over_y = q15_div(x, y);
				const int16_t correction = q15_mul(q15_from_double(0.273 * M_1_PI), s16_nabs(x_over_y));
				const int16_t unrotated = q15_mul(q15_from_double(0.25 + 0.273 * M_1_PI) + correction, x_over_y);
				if (y > 0)
				{ // octants 2, 3
					return 16384 - unrotated;
				}
				else
				{ // octants 6, 7
					return 49152 - unrotated;
				}
			}
		}

	private:
		Array<Array<bs::Real, Granularity>, Granularity> myVectorToAngle;
	};

}