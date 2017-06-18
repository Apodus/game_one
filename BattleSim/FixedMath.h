#pragma once

#include "FixedPoint.h"

namespace core
{
	class FixedMath
	{
	public:
		FixedMath() {}

		static const int32_t base = core::Fixed32::FractionBits;

		// https://gist.github.com/Madsy/1088393
		static Fixed32 exp(Fixed32 value)
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

		static Fixed32 log(Fixed32 f32)
		{
			f32.m_value = fp_ln(f32.m_value);
			return f32;
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
			double tmp = 1 / std::log2(2.71828);
			const int32_t ILOG2E = core::Fixed32(tmp).m_value;

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
	};

	inline core::Fixed32 abs(core::Fixed32 value) noexcept
	{
		return (value < core::Fixed32(0u)) ? -value : value;
	}

	inline core::Fixed32 exp(core::Fixed32 value)
	{
		return FixedMath::exp(value);
	}

	inline core::Fixed32 cos(core::Fixed32 value)
	{
		return static_cast<float>(std::cos(static_cast<double>(value)));
	}

	inline core::Fixed32 sin(core::Fixed32 value)
	{
		return static_cast<float>(std::sin(static_cast<double>(value)));
	}

	inline core::Fixed32 log(core::Fixed32 value)
	{
		return FixedMath::log(value);
	}

	inline core::Fixed32 atan2(core::Fixed32 y, core::Fixed32 x)
	{
		return std::atan2(static_cast<float>(y), static_cast<float>(x));
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
}
