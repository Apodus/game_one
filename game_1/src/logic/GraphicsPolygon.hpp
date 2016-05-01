#pragma once

#include "math/2d/polygon.hpp"
#include "math/2d/polygonTesselator.hpp"
#include "util/vec3.hpp"
#include "util/vec4.hpp"

#include "graphics/texture/texturehandler.hpp"

#include <string>
#include <memory>

class GraphicsPolygon
{
public:
	GraphicsPolygon(const sa::Polygon<sa::vec3<float>>& poly, const std::string& texture) : m_shape(poly), m_texture(texture)
	{
		m_mesh = sa::PolygonTesselator<sa::vec3<float>>().tesselate(
			m_shape,
			sa::TextureHandler::getSingleton().textureLimits(
				m_texture,
				sa::vec4<float>(0.5f, 0.5f, 1.0f, 1.0f)
			)
		);
		m_mesh->build();
	}

	GraphicsPolygon(GraphicsPolygon&& other) {
		operator = (std::move(other));
	}

	GraphicsPolygon& operator = (GraphicsPolygon&& other) {
		m_mesh = std::move(other.m_mesh);
		m_texture = std::move(other.m_texture);
		m_shape = other.m_shape;
		return *this;
	}

	const sa::Mesh& mesh() const
	{
		return *m_mesh;
	}

	const sa::Polygon<sa::vec3<float>>& shape() const
	{
		return m_shape;
	}

	const std::string& texture() const
	{
		return m_texture;
	}

private:
	sa::Polygon<sa::vec3<float>> m_shape;
	std::unique_ptr<sa::Mesh> m_mesh;
	std::string m_texture;
};