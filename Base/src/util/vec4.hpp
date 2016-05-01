
#pragma once

#include "util/logging.hpp"

namespace sa {
template<class T>
class vec4 {
public:
	union {
		struct {
			T x, y, z, w;
		};
		struct {
			T r, g, b, a;
		};
		struct {
			T left, right, top, bottom;
		};
		T data[4];
	};

	vec4() {
		x = T(0);
		y = T(0);
		z = T(0);
		w = T(0);
	}

	vec4(const T& x, const T& y, const T& z, const T& w) {
		this->x = T(x);
		this->y = T(y);
		this->z = T(z);
		this->w = T(w);
	}

	const T& operator [](int index) const {
		ASSERT(index >= 0 && index < 4, "index out of bounds");
		return data[index];
	}

	T& operator [](int index) {
		ASSERT(index >= 0 && index < 4, "index out of bounds");
		return data[index];
	}

	vec4<T>& operator +=(const vec4<T>& other) {
		x += other.x;
		y += other.y;
		z += other.z;
		w += other.w;
		return *this;
	}

	vec4<T> operator -(const vec4<T>& other) {
		return vec4<T>(x - other.x, y - other.y, z - other.z, w - other.w);
	}

	vec4<T> operator *(const T& val) {
		return vec4<T>(x * val, y * val, z * val, w * val);
	}
};

}
