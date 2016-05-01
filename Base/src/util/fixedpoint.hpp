
#pragma once

#include "util/logging.hpp"

#include <ostream>
#include <istream>
#include <iostream>
#include <cassert>
#include <istream>

template<int FIXED_POINT_ONE = 1024 * 128>
class FixedPoint
{
	long long number;

public:
	static const FixedPoint ZERO;
	static const FixedPoint PI;
	static const int s_fpOne = FIXED_POINT_ONE;

	template<int T>
	explicit FixedPoint(const FixedPoint<T>& a) :
		number(a.getRawValue() * FIXED_POINT_ONE / T)
	{
	}

	explicit FixedPoint(int a, int b = 1) :
		number(a * FIXED_POINT_ONE / b)
	{
	}

	explicit FixedPoint(long long a, long long b = 1) :
		number( a * FIXED_POINT_ONE / b )
	{
	}

	FixedPoint():
		number(0)
	{
	}

	long long getRawValue() const {
		return number;
	}

	FixedPoint& setRawValue(long long val) {
		number = val;
		return *this;
	}

	template<int T>
	operator FixedPoint<T>() {
		FixedPoint<T> val;
		val.setRawValue(number * val.s_fpOne / FIXED_POINT_ONE);
		return val;
	}

	FixedPoint& operator +() {
		return *this;
	}

	const FixedPoint& operator +() const {
		return *this;
	}

	template<int T>
	void operator = (const FixedPoint<T>& other) {
		number = other.getRawValue() * FIXED_POINT_ONE / T;
	}

	template<int T>
	FixedPoint<FIXED_POINT_ONE> operator % (const FixedPoint<T>& v) const {
		ASSERT(v > FixedPoint<T>(0), "Modulo for negative numbers not defined");
		FixedPoint<T> copy(*this);
		while (copy < v) {
			copy += v;
		}

		return FixedPoint<FIXED_POINT_ONE>(copy.getRawValue() % v.getRawValue(), static_cast<long long>(T));
	}

	operator float() const {
		return number / float(FIXED_POINT_ONE);
	}
	
	operator int() const
	{
		return static_cast<int>(number / FIXED_POINT_ONE);
	}
	
	int getDesimal() const
	{
		return number % FIXED_POINT_ONE;
	}
	
	FixedPoint abs()
	{
		FixedPoint tmp(*this);
		tmp.number *= (tmp.number < 0)?-1:1;
		return tmp;
	}

	/*
	FixedPoint& operator += (int a) {
		number += a * FIXED_POINT_ONE;
		return *this;
	}

	FixedPoint operator + (int a) {
		FixedPoint tmp(*this);
		tmp += a;
		return tmp;
	}

	FixedPoint& operator -= (int a) {
		number -= a * FIXED_POINT_ONE;
		return *this;
	}

	FixedPoint operator - (int a) {
		FixedPoint tmp(*this);
		tmp -= a;
		return tmp;
	}

	FixedPoint& operator *= (int a) {
		number *= a;
		return *this;
	}

	FixedPoint operator * (int a) {
		FixedPoint tmp(*this);
		tmp *= a;
		return tmp;
	}

	FixedPoint& operator /= (int a) {
		number /= a;
		return *this;
	}

	FixedPoint operator / (int a) {
		FixedPoint tmp(*this);
		tmp /= a;
		return tmp;
	}
	*/

	template<int T>
	FixedPoint& operator += (const FixedPoint<T>& a)
	{
		number += a.getRawValue() * FIXED_POINT_ONE / T;
		return *this;
	}

	template<int T>
	FixedPoint operator + (const FixedPoint<T>& a) const
	{
		return FixedPoint(*this) += a;
	}

	FixedPoint& operator += (const FixedPoint& a)
	{
		number += a.number;
		return *this;
	}
	
	FixedPoint operator + (const FixedPoint& a) const
	{
		return FixedPoint(*this) += a;
	}
	
	template<int T>
	FixedPoint& operator -= (const FixedPoint<T>& a)
	{
		number -= a.getRawValue() * FIXED_POINT_ONE / T;
		return *this;
	}

	template<int T>
	FixedPoint operator - (const FixedPoint<T>& a) const
	{
		return FixedPoint(*this) -= a;
	}

	FixedPoint& operator -= (const FixedPoint& a)
	{
		number -= a.number;
		return *this;
	}
	
	FixedPoint operator - (const FixedPoint& a) const
	{
		return FixedPoint(*this) -= a;
	}

	FixedPoint operator -() const
	{
		FixedPoint tmp(*this);
		tmp.number = -tmp.number;
		return tmp;
	}

	// comparison against other fixed point types
	template<int T>
	bool operator > (const FixedPoint<T>& a) const
	{
		return number * T > a.getRawValue() * FIXED_POINT_ONE;
	}
	template<int T>
	bool operator < (const FixedPoint<T>& a) const
	{
		return number * T < a.getRawValue() * FIXED_POINT_ONE;
	}
	template<int T>
	bool operator >= (const FixedPoint<T>& a) const
	{
		return number * T >= a.getRawValue() * FIXED_POINT_ONE;
	}
	template<int T>
	bool operator <= (const FixedPoint<T>& a) const
	{
		return number * T <= a.getRawValue() * FIXED_POINT_ONE;
	}
	template<int T>
	bool operator==(const FixedPoint<T>& a) const
	{
		return number * T == a.getRawValue() * FIXED_POINT_ONE;
	}
	template<int T>
	bool operator!=(const FixedPoint& a) const
	{
		return number * T != a.getRawValue() * FIXED_POINT_ONE;
	}

	// comparison against same type as self
	bool operator > (const FixedPoint& a) const
	{
		return number > a.number;
	}
	bool operator < (const FixedPoint& a) const
	{
		return number < a.number;
	}
	bool operator >= (const FixedPoint& a) const
	{
		return number >= a.number;
	}
	bool operator <= (const FixedPoint& a) const
	{
		return number <= a.number;
	}
	
	template<int T>
	FixedPoint operator * (const FixedPoint<T>& a) const
	{
		return FixedPoint(*this) *= a;
	}
	bool operator==(const FixedPoint& a) const
	{
		return number == a.number;
	}
	bool operator!=(const FixedPoint& a) const
	{
		return number != a.number;
	}

	template<int T>
	FixedPoint& operator *= (const FixedPoint<T>& a)
	{
		number *= a.getRawValue();
		number /= T;
		return *this;
	}
	
	template<int T>
	FixedPoint& operator /= (const FixedPoint<T>& a)
	{
		ASSERT(a.getRawValue() != 0, "division by zero");
		number *= T;
		number /= a.getRawValue();
		return *this;
	}
	
	template<int T>
	FixedPoint operator / (const FixedPoint<T>& a) const
	{
		return FixedPoint(*this) /= a;
	}
	
	FixedPoint squared() const
	{
		return (*this) * (*this);
	}
	
	/*
	friend inline std::istream& operator>>(std::istream& in, FixedPoint& point)
	{
		long long tmp;
		in >> tmp;
		if(in)
		{
			point.number = tmp;
		}
		return in;
	}
	*/
};

template <class T>
T squareRoot(const T& t)
{
	if(t * T(1000) < T(1))
	{
		return T(0);
	}
	
	// approximates the square root quite nicely
	T currentVal(t);
	currentVal /= T(2);
	for(int i = 0; i < 10; ++i)
	{
		currentVal += t / currentVal;
		currentVal /= T(2);
	}
	
	return currentVal;
}

namespace sa {
	namespace math {
		FixedPoint<> sqrt(const FixedPoint<>& val);
		template<int T> FixedPoint<T> abs(const FixedPoint<T>& val) { return val > FixedPoint<T>(0) ? val : -val; }
	}
}

template<int T>
std::ostream& operator<<(std::ostream& out, const FixedPoint<T>& point)
{
	return out << point.getDesimal();
}

// Trigonometric functions for fixed points
namespace sa {

	template<typename T>
	struct is_fixed_point {
		static const bool value = false;
	};

	template<int T>
	struct is_fixed_point<FixedPoint<T>> {
		static const bool value = true;
	};

	namespace math {
		const FixedPoint<>& sin(const FixedPoint<>&);
		const FixedPoint<>& cos(const FixedPoint<>&);

		namespace fixedpoint {
			void initialize();
		}
	}
}

template <int T>
std::istream& operator >> (std::istream& stream, FixedPoint<T>& fp) {
	int a, b;
	stream >> a >> b;
	fp = FixedPoint<T>(a, b);
	return stream;
}