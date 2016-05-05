#pragma once

#include "logic/Transform.hpp"
#include "logic/GraphicsPolygon.hpp"
#include "math/matrix/matrix4.hpp"
#include "util/vec3.hpp"

#include "input/userio.hpp"
#include "graphics/graphics.hpp"

#include <memory>
#include <functional>

class OnTick
{
	std::function<void(Transform&, float)> m_onTick;
public:
	template<typename F>
	OnTick(F&& f) : m_onTick(std::move(f))
	{}

	void tick(Transform& transform, float dt)
	{
		m_onTick(transform, dt);
	}
};

class OnInput
{
	std::function<void(Transform&, sa::UserIO&)> action;
public:
	template<typename F>
	OnInput(F&& f) : action(std::move(f))
	{}

	void update(Transform& t, sa::UserIO& io)
	{
		action(t, io);
	}
};

class SceneObject
{
	Transform transform;
	GraphicsPolygon shape;

	std::unique_ptr<OnTick> m_onTick;
	std::unique_ptr<OnInput> m_onInput;

	sa::Matrix4 m_modelMatrix;

public:
	SceneObject(SceneObject&& other)
		: transform(std::move(other.transform))
		, shape(std::move(other.shape))
	{
		m_onTick = std::move(other.m_onTick);
		m_onInput = std::move(other.m_onInput);
		m_modelMatrix = other.m_modelMatrix;
		transform.update();
	}

	SceneObject& operator = (SceneObject&& other)
	{
		transform = std::move(other.transform);
		shape = std::move(other.shape);

		m_onTick = std::move(other.m_onTick);
		m_onInput = std::move(other.m_onInput);
		m_modelMatrix = other.m_modelMatrix;
		return *this;
	}

	SceneObject(
		b2Body* body,
		const sa::Polygon<sa::vec3<float>>& poly,
		const std::string& texture
	) : transform(body)
		, shape(poly, texture)
	{
		m_onTick = std::make_unique<OnTick>([](Transform& t, float dt) {
		});

		m_onInput = std::make_unique<OnInput>([](Transform& t, sa::UserIO& userio) {});
		transform.update();
	}

	void inputHandler(std::unique_ptr<OnInput>&& input)
	{
		m_onInput = std::move(input);
	}

	void update(sa::UserIO& io)
	{
		transform.update();
		m_onInput->update(transform, io);
	}

	void tick(float dt)
	{
		m_onTick->tick(transform, dt);
	}

	void draw(std::shared_ptr<sa::Graphics> pGraphics)
	{
		m_modelMatrix.makeTranslationMatrix(transform.position.x, transform.position.y, 0);
		m_modelMatrix.scale(1.0f, 1.0f, 0.0f);
		m_modelMatrix.rotate(transform.direction, 0, 0, 1);
		pGraphics->m_pRenderer->drawMesh(shape.mesh(), m_modelMatrix, shape.texture(), Color::WHITE);
	}
};