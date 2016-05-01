
#include "logic/ShapePrototype.hpp"

ShapePrototype::ShapePrototype(sa::Polygon<sa::vec3<float>>& poly) {
	if (poly.triangles.empty()) {
		sa::PolygonTesselator<sa::vec3<float>>().makeTriangles(poly);
	}

	for (auto v : poly.triangles) {
		std::vector<b2Vec2> physicsVertices;
		physicsVertices.emplace_back(poly[v.a].x, poly[v.a].y);
		physicsVertices.emplace_back(poly[v.a].x, poly[v.a].y);
		physicsVertices.emplace_back(poly[v.a].x, poly[v.a].y);

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

void ShapePrototype::attach(b2Body* body) {
	for (auto&& fix : fixtureDefs)
		body->CreateFixture(&fix);
}