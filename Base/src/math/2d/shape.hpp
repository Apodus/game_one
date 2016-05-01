
#pragma once

#include "math/2d/polygon.hpp"
#include "math/2d/polygonEditor.hpp"
#include "util/vec3.hpp"
#include "math/2d/math.hpp"

// Todo: Get rid of vec3 assumption

namespace Shape {
	template<class T>
	inline sa::Polygon<sa::vec3<T>> makeAAOval(const T& radius, int vertices, const T& x_scale, const T& y_scale) {
        sa::Polygon<sa::vec3<T>> shape;
        sa::PolygonEditor<sa::vec3<T>> editor(shape);
        for(int i=0; i<vertices; ++i) {
            T angle = T(-i * 2) * sa::math::PI<T>() / T(vertices);
            editor.insertVertex(
				sa::vec3<T>(
					x_scale * radius * sa::math::sin(angle),
					y_scale * radius * sa::math::cos(angle),
					T(0)
				)
			);
        }
        return shape;
    }

	template<class T>
    inline sa::Polygon<sa::vec3<T>> makeCircle(const T& radius, int vertices) {
        return makeAAOval(radius, vertices, T(1), T(1));
    }

	template<class T>
    inline sa::Polygon<sa::vec3<T>> makeTriangle(const T& radius) {
        return makeCircle(radius, 3);
    }

	template<class T>
    inline sa::Polygon<sa::vec3<T>> makeRectangle(const T& width, const T& height, int widthDivs = 1, int heightDivs = 1) {
        sa::Polygon<sa::vec3<T>> shape;
        sa::PolygonEditor<sa::vec3<T>> editor(shape);

		auto addLine = [&](const sa::vec3<T>& start, const sa::vec3<T>& end, int divs) {
			ASSERT(divs > 0, "zero segments is not an option");
			for (int i = 0; i < divs; ++i) {
				editor.insertVertex(start * T(divs - i) / T(divs) + end * T(i) / T(divs));
			}
		};

		auto topLeft = sa::vec3<T>(-width / T(2), height / T(2), T(0));
		auto botLeft = sa::vec3<T>(-width / T(2), -height / T(2), T(0));
		auto botRight = sa::vec3<T>(+width / T(2), -height / T(2), T(0));
		auto topRight = sa::vec3<T>(+width / T(2), +height / T(2), T(0));
		
		addLine(topLeft, botLeft, heightDivs);
		addLine(botLeft, botRight, widthDivs);
		addLine(botRight, topRight, heightDivs);
		addLine(topRight, topLeft, widthDivs);
        
		return shape;
    }

	template<class T>
	inline sa::Polygon<sa::vec3<T>> makeBox(const T& edgeLength) {
        return makeRectangle(edgeLength, edgeLength);
    }
}

