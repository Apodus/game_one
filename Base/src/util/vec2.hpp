
#pragma once

#include "math/2d/math.hpp"
#include <iostream>

namespace sa {

template <class T>
class vec2
{
public:
	vec2();
	vec2(const vec2<T>& other): x(other.x), y(other.y) {}
	vec2(const T& a, const T& b);
	
	T x, y;
	
	inline vec2<T>& normalize();
	vec2<T>& operator*=(const T& scalar);
	vec2<T>& operator/=(const T& scalar);
	vec2<T>& operator+=(const vec2<T>&);
	vec2<T>& operator-=(const vec2<T>&);
	vec2<T> operator+(const vec2<T>&) const;
	vec2<T> operator-(const vec2<T>&) const;
	vec2<T> operator*(const vec2<T>&) const;
	
	vec2<T> operator*(const T& scalar) const;
	vec2<T> operator/(const T& scalar) const;
	vec2<T>& set(const T&, const T&);
	
	T operator[](int i) const;
	
	T crossProduct(const vec2<T>&) const;
	T dotProduct(const vec2<T>& b) const;
	T length() const;
	T lengthSquared() const;
};

#include "util/vec2.icc"

template <class T> vec2<T> operator-(const vec2<T>& a)
{
	return vec2<T>(-a.x, -a.y);
}

template <class T> bool operator == (const vec2<T>& a, const vec2<T>& b)
{
	return a.x == b.x && a.y == b.y;
}

template <class T> bool operator != (const vec2<T>& a, const vec2<T>& b)
{
	return a.x != b.x || a.y != b.y;
}

template <class T> std::ostream& operator<<(std::ostream& out, const vec2<T>& loc)
{
	return out << "(" << loc.x << "," << loc.y << ")";
}

template <class T> vec2<T>& vec2<T>::normalize()
{
	T length = sa::math::sqrt(x*x + y*y+ T(1) / T(1024));
	x /= length;
	y /= length;
	return *this;
}

}