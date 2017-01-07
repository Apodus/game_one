
#pragma once

#include <cmath>
#include <utility>
#include <cstdint>

#ifdef max
#undef max
#endif

namespace sa {
namespace math {

	const float PI_float = 3.14159f;
	template<typename T> inline const T& PI() { return T::PI; }
	template<> inline const float& PI<float>() { return PI_float; }

	template<int N>
	struct Factorial {
		enum { value = N * Factorial<N-1>::value };
	};

	template<>
	struct Factorial<0> {
		enum { value = 1 };
	};

	// xorshift32 - feed the result as the next parameter to get next random number.
	//              must start with nonzero value.
	inline uint32_t rand(uint32_t x) {
		x ^= x << 13;
		x ^= x >> 17;
		x ^= x << 5;
		return x;
	}

	// xorshift64 - feed the result as the next parameter to get next random number.
	//              must start with nonzero value.
	inline uint64_t rand(uint64_t x) {
		x ^= x >> 12; // a
		x ^= x << 25; // b
		x ^= x >> 27; // c
		return x * 0x2545F4914F6CDD1D;
	}

	inline float sin(const float& f) {
		return ::sin(f);
	}

	inline float cos(float f) {
		return ::cos(f);
	}

	inline float abs(float f) {
		return std::abs(f);
	}

	template<class T>
	inline T sqrt(const T& val) {
		return ::sqrt(val);
	}

	template<class T>
	inline T& rotateXY(T& v, decltype(T().x) radians) {
		decltype(T().x) cosVal = sa::math::cos(radians);
		decltype(T().x) sinVal = sa::math::sin(radians);
		decltype(T().x) xNew = (v.x * cosVal - v.y * sinVal);
		decltype(T().x) yNew = (v.x * sinVal + v.y * cosVal);
		v.x = xNew;
		v.y = yNew;
		return v;
	}

	template<class T>
	inline T rotatedXY(const T& v, decltype(T().x) radians) {
		T result;
		decltype(T().x) cosVal = sa::math::cos(radians);
		decltype(T().x) sinVal = sa::math::sin(radians);
		result.x = (v.x * cosVal - v.y * sinVal);
		result.y = (v.x * sinVal + v.y * cosVal);
		return result;
	}

	// approximation, good enough
	template<>
	inline float sqrt<float>(const float& val) {
		int* x = (int*)(&val);
        *x = (1 << 29) + (*x >> 1) - (1 << 22) - 0x4C000;
        return *((float*)x);
	}

	template<class P>
	inline decltype(std::declval<P>().x) distanceSquared(const P& p1, const P& p2) {
		decltype(p1.x) x = (p1.x - p2.x);
		decltype(p1.x) y = (p1.y - p2.y);
		return x * x + y * y;
	}

	template<class P>
	inline bool pointInTriangle(const P& point, const P& t1, const P& t2, const P& t3) {
		typedef decltype(point.x) Scalar;
        P v0, v1, v2;
		v0.x = t3.x - t1.x;
        v0.y = t3.y - t1.y;

        v1.x = t2.x - t1.x;
        v1.y = t2.y - t1.y;

        v2.x = point.x - t1.x;
        v2.y = point.y - t1.y;

		Scalar dot00 = v0.x * v0.x + v0.y * v0.y;
		Scalar dot01 = v0.x * v1.x + v0.y * v1.y;
		Scalar dot02 = v0.x * v2.x + v0.y * v2.y;
		Scalar dot11 = v1.x * v1.x + v1.y * v1.y;
		Scalar dot12 = v1.x * v2.x + v1.y * v2.y;

        // Compute barycentric coordinates
		Scalar invDenom = Scalar(1) / (dot00 * dot11 - dot01 * dot01);
		Scalar u = (dot11 * dot02 - dot01 * dot12) * invDenom;
		Scalar v = (dot00 * dot12 - dot01 * dot02) * invDenom;

        // Check if point is in triangle
		return (u > Scalar(0)) && (v > Scalar(0)) && (u + v < Scalar(1));
    }
	
	template<class P>
	inline bool pointLeftOfLine(const P& point, const P& t1, const P& t2) {
		return (t2.x - t1.x) * (point.y - t1.y) - (t2.y - t1.y) * (point.x - t1.x) > (decltype(point.x))(0);
	}

	template<class T>
	inline decltype(std::declval<T>().x) crossProduct(const T& t1, const T& t2) {
        return (t1.x * t2.y - t1.y * t2.x);
    }
}
}
