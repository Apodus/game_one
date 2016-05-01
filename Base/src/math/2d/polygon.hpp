
#pragma once

#include "math/2d/LineSegment.hpp"
#include <vector>

namespace sa {

	template<class T>
	class Polygon {

	public:

		class Segment : public LineSegment<T> {
		public:
			bool convex1;
			bool convex2;

			Segment() : LineSegment<T>(), convex1(true), convex2(true) {

			}

			Segment(const LineSegment<T>& lineSegment) {
				*this = lineSegment;
			}

			Segment(const T& a, const T& b, bool c1, bool c2) {
				this->p1 = a;
				this->p2 = b;
				convex1 = c1;
				convex2 = c2;
			}
		};

		struct Triangle {
			int a, b, c;
			Triangle(int a, int b, int c) {
				this->a = a;
				this->b = b;
				this->c = c;
			}
		};

		std::vector<T> vertices;
		std::vector<Triangle> triangles;

		T& operator [](int index) {
			return vertices[index];
		}

		const T& operator [](int index) const {
			return vertices[index];
		}

		bool isConvex(int vertex) const {
			T left;
			T right;
			if (vertex == 0)
				left = vertices[vertices.size() - 1];
			else
				left = vertices[vertex - 1];

			left -= vertices[vertex];

			if (vertex == vertices.size() - 1)
				right = (*this)[0];
			else
				right = vertices[vertex + 1];
			right -= (*this)[vertex];
			return sa::math::crossProduct(left, right) < decltype(T().x)(0);
		}


		std::vector<Segment> generateBoundary() const {
			std::vector<Segment> boundary;
			if (vertices.size() < 3) {
				return boundary;
			}

			for (size_t i = 0; i < vertices.size() - 1; ++i) {
				boundary.emplace_back(Segment(vertices[i], vertices[i + 1], isConvex(i), isConvex(i + 1)));
			}
			boundary.emplace_back(Segment(vertices[vertices.size() - 1], vertices[0], isConvex(vertices.size() - 1), isConvex(0)));
			return boundary;
		}
	};

}

#include "math/2d/math.hpp"

namespace sa {
	namespace math {
		template<class P>
		bool pointInPolygon(const P& point, const sa::Polygon<P>& poly) {
			bool ans = false;
			for (const typename Polygon<P>::Triangle& t : poly.triangles) {
				ans |= pointInTriangle(point, poly[t.a], poly[t.b], poly[t.c]);
			}
			return ans;
		}
	}
}
