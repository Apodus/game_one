#pragma once

#include "FixedPoint.h"
#include "Math.h"

namespace core
{
	class FixedMath
	{
		static const size_t NumSinEntries = 1024;
		static const int16_t ourSinValues[NumSinEntries];
	public:
		FixedMath() {}

		static const int32_t base = core::Fixed32::FractionBits;

		// https://gist.github.com/Madsy/1088393
		static inline Fixed32 exp(Fixed32 value)
		{
			int32_t val = value.m_value;
			int32_t x = val;
			x = x - (((int64_t)x*(fp_ln(x) - val)) >> base);
			x = x - (((int64_t)x*(fp_ln(x) - val)) >> base);
			x = x - (((int64_t)x*(fp_ln(x) - val)) >> base);
			x = x - (((int64_t)x*(fp_ln(x) - val)) >> base);
			value.m_value = x;
			return value;
		}

		template<typename T, size_t U>
		static inline FixedPoint<T,U> abs(FixedPoint<T, U> fp)
		{
			fp.m_value = std::abs(fp.m_value);
			return fp;
		}

		static inline Fixed32 log(Fixed32 f32)
		{
			f32.m_value = fp_ln(f32.m_value);
			return f32;
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
		static uint16_t fxpt_atan2(const int16_t y, const int16_t x)
		{
#if 1
			const int16_t correctionMulti = 2847;
			const int16_t unrotatedMulti = 11039;
#else // Original
			static inline int16_t q15_from_double(const double d)
			{
				return static_cast<int16_t>(lrint(d * 32768));
			}
			static const double M_1_PI = 0.31830988618379067154;
			const int16_t correctionMulti = q15_from_double(0.273 * M_1_PI);
			const int16_t unrotatedMulti = q15_from_double(0.25 + 0.273 * M_1_PI);
#endif

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
				const int16_t correction = q15_mul(correctionMulti, s16_nabs(y_over_x));
				const int16_t unrotated = q15_mul(unrotatedMulti + correction, y_over_x);
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
				const int16_t correction = q15_mul(correctionMulti, s16_nabs(x_over_y));
				const int16_t unrotated = q15_mul(unrotatedMulti + correction, x_over_y);
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

		static Fixed32 Cos(core::Fixed32 v)
		{
			v += Math::Pi() / 2;
			return Sin(v);
		}

		static Fixed32 Sin(core::Fixed32 v)
		{
			if (v < 0)
			{
				int diff = static_cast<int>(-v / (Math::Pi() * Fraction<int64_t>(2)));
				v += (Math::Pi() * Fraction<int64_t>(diff * 2));
			}
			v = v / (Math::Pi() * Fraction<int64_t>(2));
			int index = static_cast<int>(v * Fixed32(NumSinEntries)) % NumSinEntries;
			auto modOne = v % Fixed32(1);

			Fixed32 a;
			a.m_value = ourSinValues[index];
			Fixed32 b;
			b.m_value = ourSinValues[(index + 1) & (NumSinEntries - 1)];
			return a * (Fixed32(1) - modOne) + (b * modOne);
		}


	private:
		static constexpr int32_t clz(uint32_t x)
		{
			/* See "Hacker's Delight" book for more details */
			if (x == 0) return 32;
			int32_t n = 0;
			if (x <= 0x0000FFFF) { n = n + 16; x = x << 16; }
			if (x <= 0x00FFFFFF) { n = n + 8; x = x << 8; }
			if (x <= 0x0FFFFFFF) { n = n + 4; x = x << 4; }
			if (x <= 0x3FFFFFFF) { n = n + 2; x = x << 2; }
			if (x <= 0x7FFFFFFF) { n = n + 1; }

			return n;
		}

		// https://gist.github.com/Madsy/1088393
		static int32_t fp_ln(int32_t val)
		{
			int32_t fracv = 0, intv = 0, y = 0, ysq = 0, fracr = 0, bitpos = 0;
			/*
			fracv    -    initial fraction part from "val"
			intv    -    initial integer part from "val"
			y        -    (fracv-1)/(fracv+1)
			ysq        -    y*y
			fracr    -    ln(fracv)
			bitpos    -    integer part of log2(val)
			*/

			// const int32_t ILN2 = 94548;        /* 1/ln(2) with 2^16 as base*/
			// const int32_t ILOG2E = 45426;    /* 1/log2(e) with 2^16 as base */
			// double tmp = 1 / std::log2(2.71828);
			const int32_t ILOG2E = core::Fixed32(core::Fraction32(693148, 1000000)).m_value;
			const int32_t ln_denoms[] = {
				(1 << base) / 1,
				(1 << base) / 3,
				(1 << base) / 5,
				(1 << base) / 7,
				(1 << base) / 9,
				(1 << base) / 11,
				(1 << base) / 13,
				(1 << base) / 15,
				(1 << base) / 17,
				(1 << base) / 19,
				(1 << base) / 21,
			};

			/* compute fracv and intv */
			bitpos = (base - 1) - clz(val);
			if (bitpos >= 0) {
				++bitpos;
				fracv = val >> bitpos;
			}
			else if (bitpos < 0) {
				/* fracr = val / 2^-(bitpos) */
				++bitpos;
				fracv = val << (-bitpos);
			}

			// bitpos is the integer part of ln(val), but in log2, so we convert
			// ln(val) = log2(val) / log2(e)
			intv = bitpos * ILOG2E;

			// y = (ln_fraction_value−1)/(ln_fraction_value+1)
			y = ((int64_t)(fracv - (1 << base)) << base) / (fracv + (1 << base));

			ysq = (y*y) >> base;
			fracr = ln_denoms[10];
			fracr = (((int64_t)fracr * ysq) >> base) + ln_denoms[9];
			fracr = (((int64_t)fracr * ysq) >> base) + ln_denoms[8];
			fracr = (((int64_t)fracr * ysq) >> base) + ln_denoms[7];
			fracr = (((int64_t)fracr * ysq) >> base) + ln_denoms[6];
			fracr = (((int64_t)fracr * ysq) >> base) + ln_denoms[5];
			fracr = (((int64_t)fracr * ysq) >> base) + ln_denoms[4];
			fracr = (((int64_t)fracr * ysq) >> base) + ln_denoms[3];
			fracr = (((int64_t)fracr * ysq) >> base) + ln_denoms[2];
			fracr = (((int64_t)fracr * ysq) >> base) + ln_denoms[1];
			fracr = (((int64_t)fracr * ysq) >> base) + ln_denoms[0];
			fracr = ((int64_t)fracr * (y << 1)) >> base;

			return intv + fracr;
		}

		template<typename T>
		static inline T s16_nabs(const T j)
		{
#if (((int16_t)-1) >> 1) == ((int16_t)-1)
			// signed right shift sign-extends (arithmetic)
			constexpr size_t Shift = sizeof(T) * 8 - 1;
			const T negSign = ~(j >> Shift); // splat sign bit into all 16 and complement
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
	};

	inline core::Fixed32 abs(core::Fixed32 value) noexcept { return FixedMath::abs(value);}

	inline core::Fixed32 exp(core::Fixed32 value) { return FixedMath::exp(value); }

	inline core::Fixed32 cos(core::Fixed32 value) { return FixedMath::Cos(value); }

	inline core::Fixed32 sin(core::Fixed32 value) { return FixedMath::Sin(value); }

	inline core::Fixed32 log(core::Fixed32 value) { return FixedMath::log(value);}

	template<typename T, size_t U>
	inline core::FixedPoint<T, U> atan2(core::FixedPoint<T, U> y, core::FixedPoint<T, U> x)
	{
		T yi = static_cast<T>(y * INT16_MAX);
		T xi = static_cast<T>(x * INT16_MAX);
		yi = yi < INT16_MAX ? yi > -INT16_MAX ? yi : -INT16_MAX : INT16_MAX;
		xi = xi < INT16_MAX ? xi > -INT16_MAX ? xi : -INT16_MAX : INT16_MAX;
		int32_t at2 = FixedMath::fxpt_atan2(static_cast<int16_t>(yi), static_cast<int16_t>(xi));
		at2 -= INT16_MAX * 2;
		if (at2 < -INT16_MAX)
		{
			at2 += INT16_MAX * 2;
		}
		core::FixedPoint<T, U> result(core::Fraction<T>(static_cast<T>(at2), INT16_MAX));
		return result * core::Math::Pi();
	}

	inline core::Fixed32 round(core::Fixed32 value) noexcept
	{
		return core::Fixed32{ static_cast<int16_t>(value + (core::Fixed32(core::Fraction64(1, 2)))) };
	}

	inline core::Fixed32 nextafter(core::Fixed32 from, core::Fixed32 to) noexcept
	{
		if (from < to)
		{
			return from + std::numeric_limits<core::Fixed32>::min();
		}
		if (from > to)
		{
			return from - std::numeric_limits<core::Fixed32>::min();
		}
		return to;
	}

	inline bool isfinite(core::Fixed32 value) noexcept
	{
		return (value > core::Fixed32::GetNegativeInfinity()) && (value < core::Fixed32::GetInfinity());
	}

	constexpr inline bool isnan(core::Fixed32 value) noexcept
	{
		return value.Compare(0) == core::Fixed32::ComparatorResult::Incomparable;
	}

#ifndef _WIN32
	inline bool isfinite(core::Fixed64 value) noexcept
	{
		return (value > core::Fixed64::GetNegativeInfinity()) && (value < core::Fixed64::GetInfinity());
	}

	constexpr inline bool isnan(core::Fixed64 value) noexcept
	{
		return value.Compare(0) == core::Fixed64::ComparatorResult::Incomparable;
	}
#endif
}
