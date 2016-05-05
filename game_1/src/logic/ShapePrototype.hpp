#pragma once

#include "Box2D/Box2D.h"
#include "util/vec3.hpp"
#include "math/2d/polygonTesselator.hpp"
#include <vector>

class ShapePrototype
{
	sa::Polygon<sa::vec3<float>> polygon;
	std::vector<b2PolygonShape> shapes;
	std::vector<b2FixtureDef> fixtureDefs;

public:
	ShapePrototype(const sa::Polygon<sa::vec3<float>>& poly);
	void attach(b2Body* body, float density = 1.0f);
	operator const sa::Polygon<sa::vec3<float>>&() const;
};