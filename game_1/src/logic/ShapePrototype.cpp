
#include "logic/ShapePrototype.hpp"

ShapePrototype::ShapePrototype(const sa::Polygon<sa::vec3<float>>& givenPoly) {

	polygon = givenPoly;
	auto& poly = polygon;

	if (poly.triangles.empty()) {
		sa::PolygonTesselator<sa::vec3<float>>().makeTriangles(poly);
	}

	for (auto v : poly.triangles) {
		std::vector<b2Vec2> physicsVertices;
		physicsVertices.emplace_back(poly[v.a].x, poly[v.a].y);
		physicsVertices.emplace_back(poly[v.b].x, poly[v.b].y);
		physicsVertices.emplace_back(poly[v.c].x, poly[v.c].y);

		b2PolygonShape shape;
		shape.Set(physicsVertices.data(), physicsVertices.size());

		shapes.emplace_back(shape);
	}

	for (auto&& shape : shapes) {
		b2FixtureDef fixture;
		fixture.density = 1;
		fixture.shape = &shape;
		fixtureDefs.emplace_back(fixture);
	}
}

void ShapePrototype::attach(b2Body* body, float density) {
	for (auto&& fix : fixtureDefs)
	{
		fix.density = density;
		body->CreateFixture(&fix);
	}
}

ShapePrototype::operator const sa::Polygon<sa::vec3<float>>&() const
{
	return polygon;
}