
#pragma once

#include "math/2d/math.hpp"
#include "math/2d/mesh.hpp"
#include "math/2d/polygon.hpp"
#include "math/2d/polygonEditor.hpp"
#include "util/attributes.hpp"
#include "util/vec4.hpp"

#include <vector>
#include <stdexcept>

namespace sa {

	template<class T>
	class PolygonTesselator {

		struct TriangulationException {};

		sa::Polygon<T>* polygon;
		std::vector<int> unhandled;

		std::vector<typename Polygon<T>::Triangle>& getTriangles() {
			return polygon->triangles;
		}

		void resetUnhandledMarkers() {
			unhandled.clear();
			for (unsigned i = 0; i < polygon->vertices.size(); ++i)
				unhandled.emplace_back(i);
		}

		void resetForPostProcess() {
			polygon->triangles.clear();
			resetUnhandledMarkers();
		}

		INLINE T& getVertexUnhandled(int i) {
			return polygon->vertices[unhandled[i]];
		}

		void addTriangle(int earNode, int t1, int t2) {
			polygon->triangles.push_back(typename Polygon<T>::Triangle(unhandled[t1], unhandled[earNode], unhandled[t2]));
			for (unsigned i = earNode; i < unhandled.size() - 1; ++i)
				unhandled[i] = unhandled[i + 1];
			unhandled.pop_back();
		}

		std::unique_ptr<Mesh> buildMeshData(vec4<float> uvLimits) {
			std::unique_ptr<Mesh> polyMesh = std::make_unique<sa::Mesh>(polygon->vertices.size(), polygon->triangles.size() * 3);

			// build vertex buffer
			for (T& v : polygon->vertices) {
				polyMesh->putVertex(float(v.x), float(v.y), 0.0f);
				float uvX = uvLimits.data[0] + static_cast<float>(v.x) * uvLimits.data[2]; // assumes [-1, +1] meshes
				float uvY = uvLimits.data[1] + static_cast<float>(v.y) * uvLimits.data[3]; // assumes [-1, +1] meshes
				polyMesh->putUVCoord(uvX, uvY);
			}

			// build index buffer
			for (const typename Polygon<T>::Triangle& t : getTriangles()) {
				polyMesh->putTriangleIndices(t.a, t.b, t.c);
			}

			return std::move(polyMesh);
		}

		bool isEar(int t1, int t2, int t3) {
			for (unsigned i = 0; i < unhandled.size(); ++i) {
				if (i == t1 || i == t2 || i == t3)
					continue;
				if (sa::math::pointInTriangle<T>(getVertexUnhandled(i), getVertexUnhandled(t1), getVertexUnhandled(t2), getVertexUnhandled(t3))) {
					return false;
				}
			}
			return true;
		}

		bool triangulateOneStep() {

			// Note: If there is enough love in the polygon, we are done.
			if (unhandled.size() < 3) {
				return true;
			}

			for (unsigned i = 0; i < unhandled.size() - 2; ++i) {
				if (sa::math::pointLeftOfLine(getVertexUnhandled(i + 1), getVertexUnhandled(i), getVertexUnhandled(i + 2))) {
				}
				else {
					if (isEar(i + 1, i, i + 2)) {
						addTriangle(i + 1, i, i + 2);
						return false;
					}
				}
			}

			int k = unhandled.size();
			if (sa::math::pointLeftOfLine(getVertexUnhandled(k - 1), getVertexUnhandled(k - 2), getVertexUnhandled(0))) {

			}
			else {
				if (isEar(k - 1, k - 2, 0)) {
					addTriangle(k - 1, k - 2, 0);
					return false;
				}
			}

			if (sa::math::pointLeftOfLine(getVertexUnhandled(0), getVertexUnhandled(k - 1), getVertexUnhandled(1))) {

			}
			else {
				if (isEar(0, k - 1, 1)) {
					addTriangle(0, k - 1, 1);
					return false;
				}
			}

			throw new TriangulationException();
		}

		void triangulate() {
			while (!triangulateOneStep());
		}


	public:
		PolygonTesselator() {
		}

		std::unique_ptr<Mesh> tesselate(Polygon<T>& polygon, vec4<float> uvLimits = vec4<float>(0.5f, 0.5f, 1.0f, 1.0f)) {
			makeTriangles(polygon);
			return buildMeshData(uvLimits);
		}

		void makeTriangles(Polygon<T>& polygon) {
			this->polygon = &polygon;
			resetForPostProcess();

			try {
				triangulate();
			}
			catch (TriangulationException) {
				PolygonEditor<T> editor(polygon);
				editor.reverse();
				try {
					triangulate();
				}
				catch (TriangulationException) {
					throw new std::runtime_error("Broken polygon");
				}
			}
		}
	};

}
