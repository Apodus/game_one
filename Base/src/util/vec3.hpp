
#pragma once

#include "math/2d/math.hpp"
#include "util/vec2.hpp"
#include <iostream>

namespace sa {

template <class T>
class vec3
{
public:
	vec3();
	vec3(const vec3<T>& other): x(other.x), y(other.y), z(other.z) {}
	vec3(const T& a, const T& b, const T& c);
	vec3(const vec2<T>& other): x(other.x), y(other.y), z(0) {}

	template<typename T>
	explicit operator vec3<T>() const {
		return vec3<T>(x, y, z);
	}

	T x, y, z;
	
	inline vec3<T>& normalize();
	vec3<T>& operator*=(const T& scalar);
	vec3<T>& operator/=(const T& scalar);
	vec3<T>& operator+=(const vec3<T>&);
	vec3<T>& operator-=(const vec3<T>&);
	vec3<T> operator+(const vec3<T>&) const;
	vec3<T> operator-(const vec3<T>&) const;
	vec3<T> operator*(const vec3<T>&) const;
	
	vec3<T> operator*(const T& scalar) const;
	vec3<T> operator/(const T& scalar) const;
	vec3<T>& set(const T&, const T&, const T&);
	
	T operator[](int i) const;
	
	vec3 crossProduct(const vec3<T>&) const;
	T dotProduct(const vec3<T>& b) const;
	T length() const;
	T lengthSquared() const;
};

#include "util/vec3.icc"

template <class T> vec3<T> operator-(const vec3<T>& a)
{
	return vec3<T>(-a.x, -a.y, -a.z);
}

template <class T> bool operator == (const vec3<T>& a, const vec3<T>& b)
{
	return a.x == b.x && a.y == b.y && a.z == b.z;
}

template <class T> bool operator != (const vec3<T>& a, const vec3<T>& b)
{
	return a.x != b.x || a.y != b.y || a.z != b.z;
}

template <class T> std::ostream& operator<<(std::ostream& out, const vec3<T>& loc)
{
	return out << "(" << loc.x << "," << loc.y << "," << loc.z << ")";
}

template <class T> vec3<T>& vec3<T>::normalize()
{
	T length = sa::math::sqrt(x*x + y*y + z*z + T(1) / T(1024));
	x /= length;
	y /= length;
	z /= length;
	return *this;
}

}