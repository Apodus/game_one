
#pragma once

#include "math/2d/math.hpp"

namespace sa {

template<class T> 
class LineSegment {
	typedef decltype(T().x) Scalar;

public:
	T p1;
	T p2;
	T normal;

	LineSegment() {
		p1 = T();
		p2 = T();
		normal = T();
	}

	LineSegment(const T& a, const T& b) {
		p1 = a;
		p2 = b;
	}

	LineSegment(const LineSegment& other) {
		p1 = other.p1;
		p2 = other.p2;
		normal = other.normal;
	}

	// assumes 2d
	Scalar lengthApprox() {
		Scalar x = p1.x - p2.x;
		Scalar y = p1.y - p2.y;
		return sa::math::sqrt(x * x + y * y);
	}

	// assumes 2d
    T& computeNormal() {
        normal.x = p2.y - p1.y;
        normal.y = p1.x - p2.x;
        return normal;
    }

	T computeNormal() const {
        T normal;
		normal.x = p2.y - p1.y;
        normal.y = p1.x - p2.x;
        return normal;
    }

	// assumes 2d
    const T& getNormal() const {
        return normal;
    }

	bool intersects(const LineSegment<T>& other) const {		
		const T& p = other.p1;
		const T& r = other.p2 - other.p1;

		const T& q = p1;
		const T& s = p2 - p1;

		T qp = (q - p);
		Scalar rxs = sa::math::crossProduct(r, s);
		Scalar qps = sa::math::crossProduct(qp, s);
		Scalar qpr = sa::math::crossProduct(qp, r);

		if (rxs == Scalar(0)) {
			if (qpr == Scalar(0)) {
				// lines are colinear.
				Scalar val_r = qp.x * r.x + qp.y * r.y;
				if (val_r > Scalar(0) && val_r < r.x * r.x + r.y * r.y)
					return true;

				Scalar val_s = qp.x * s.x + qp.y * s.y;
				if (val_s > Scalar(0) && val_s < s.x * s.x + s.y * s.y)
					return true;

			}

			// parallel but disjoint
			return false;
		}

		Scalar t = qps / rxs;
		Scalar u = qpr / rxs;

		bool t_ok = t > Scalar(0) && t < Scalar(1);
		bool u_ok = u > Scalar(0) && u < Scalar(1);
		return t_ok && u_ok;
	}

	bool approxEquals(const LineSegment& lineSegment) {
		return sa::math::distanceSquared(p1, lineSegment.p1) + sa::math::distanceSquared(p2, lineSegment.p2) < 0.01f;
	}
};

}
