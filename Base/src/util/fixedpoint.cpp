
#include "fixedpoint.hpp"
#include "math/2d/math.hpp"
#include <array>

template<int64_t I> const FixedPoint<I> FixedPoint<I>::ZERO = FixedPoint<I>(0);
template<int64_t I> const FixedPoint<I> FixedPoint<I>::PI = FixedPoint<I>(314159265359ll, 100000000000ll);

namespace sa {
	namespace math {
		FixedPoint<> sqrt(const FixedPoint<>& val) {
			return squareRoot(val);
		}
	}
}

namespace {

	FixedPoint<> factorial(int n) {
		FixedPoint<> v(1);
		for (int i = 1; i <= n; ++i) {
			v *= FixedPoint<>(i);
		}
		return v;
	}

	FixedPoint<1024 * 128> power(const FixedPoint<1024 * 128>& v, int p) {
		FixedPoint<1024 * 128> tmp(1);
		FixedPoint<1024 * 128> copy;
		copy = v;
		for (int i = 0; i < p; ++i) {
			tmp *= copy;
		}
		return tmp;
	}


	/*
	sin x   =   x − x3 / 3! + x5 / 5! − x7 / 7! + ...
	cos x   =   1 − x2 / 2! + x4 / 4! − x6 / 6! + ...
	*/

	FixedPoint<> sin_fp_slow(FixedPoint<1024 * 128> p) {
		FixedPoint<1024 * 128> val;
		for (int i = 0; i < 3; ++i) {
			val += power(p, i * 4 + 1) / factorial(i * 4 + 1);
			val -= power(p, i * 4 + 3) / factorial(i * 4 + 3);
		}
		return val;
	}

	FixedPoint<> cos_fp_slow(FixedPoint<1024 * 128> p) {
		FixedPoint<1024 * 128> val;
		for (int i = 0; i < 3; ++i) {
			val += power(p, i * 4 + 0) / factorial(i * 4 + 0);
			val -= power(p, i * 4 + 2) / factorial(i * 4 + 2);
		}
		return val;
	}

#define NUM_SINCOS_ENTRIES 1024
	std::array<FixedPoint<>, NUM_SINCOS_ENTRIES> g_FixedPoint_SinValues;
	std::array<FixedPoint<>, NUM_SINCOS_ENTRIES> g_FixedPoint_CosValues;

}

void sa::math::fixedpoint::initialize() {
	for (int i = 0; i < NUM_SINCOS_ENTRIES; ++i) {
		g_FixedPoint_SinValues[i] = sin_fp_slow(FixedPoint<1024 * 128>(i) * FixedPoint<1024 * 1024>::PI * FixedPoint<1024 * 1024>(2) / FixedPoint<1024 * 1024>(NUM_SINCOS_ENTRIES) - FixedPoint<1024 * 128>::PI);
		g_FixedPoint_CosValues[i] = cos_fp_slow(FixedPoint<1024 * 128>(i) * FixedPoint<1024 * 1024>::PI * FixedPoint<1024 * 1024>(2) / FixedPoint<1024 * 1024>(NUM_SINCOS_ENTRIES) - FixedPoint<1024 * 128>::PI);
	}
}

FixedPoint<> sa::math::sin(const FixedPoint<>& v) {
	FixedPoint<> tmp = (v + FixedPoint<>::PI) % (FixedPoint<1024 * 128>::PI * FixedPoint<1024 * 128>(2));
	tmp *= FixedPoint<>(NUM_SINCOS_ENTRIES);
	tmp /= (FixedPoint<>::PI * FixedPoint<>(2));

	int index = static_cast<int>(tmp);
	auto modOne = tmp.getDesimal();
	return g_FixedPoint_SinValues[index] * (FixedPoint<>(1) - modOne) + g_FixedPoint_SinValues[(index + 1) & (NUM_SINCOS_ENTRIES - 1)] * modOne;
}

FixedPoint<> sa::math::cos(const FixedPoint<>& v) {
	FixedPoint<> tmp = (v + FixedPoint<>::PI) % (FixedPoint<1024 * 128>::PI * FixedPoint<1024 * 128>(2));
	tmp *= FixedPoint<>(NUM_SINCOS_ENTRIES);
	tmp /= (FixedPoint<>::PI * FixedPoint<>(2));

	int index = static_cast<int>(tmp);
	auto modOne = tmp.getDesimal();
	return g_FixedPoint_CosValues[index] * (FixedPoint<>(1) - modOne) + g_FixedPoint_CosValues[(index + 1) & (NUM_SINCOS_ENTRIES - 1)] * modOne;
}

