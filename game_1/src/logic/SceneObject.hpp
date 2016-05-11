#pragma once


#include "logic/ObjectController.hpp"
#include "logic/GraphicsPolygon.hpp"
#include "math/matrix/matrix4.hpp"
#include "util/vec3.hpp"

#include "input/userio.hpp"
#include "graphics/graphics.hpp"

#include <memory>
#include <functional>

class OnTick
{
	std::function<void(ObjectController&, float)> m_onTick;
public:
	template<typename F>
	OnTick(F&& f) : m_onTick(std::move(f))
	{}

	void tick(ObjectController& oc, float dt)
	{
		m_onTick(oc, dt);
	}
};

class OnInput
{
	std::function<void(ObjectController&, sa::UserIO&)> action;
public:
	template<typename F>
	OnInput(F&& f) : action(std::move(f))
	{}

	void update(ObjectController& t, sa::UserIO& io)
	{
		action(t, io);
	}
};

class SceneObject
{
	ObjectController controller;
	GraphicsPolygon shape;

	std::unique_ptr<OnTick> m_onTick;
	std::unique_ptr<OnInput> m_onInput;
	uint16_t m_id;

	mutable sa::Matrix4 m_modelMatrix;

public:
	SceneObject(SceneObject&& other)
		: controller(std::move(other.controller))
		, shape(std::move(other.shape))
	{
		m_onTick = std::move(other.m_onTick);
		m_onInput = std::move(other.m_onInput);
		m_modelMatrix = other.m_modelMatrix;
	}

	SceneObject& operator = (SceneObject&& other)
	{
		controller = std::move(other.controller);
		shape = std::move(other.shape);

		m_onTick = std::move(other.m_onTick);
		m_onInput = std::move(other.m_onInput);
		m_modelMatrix = other.m_modelMatrix;
		return *this;
	}

	SceneObject(
		b2Body* body,
		const sa::Polygon<sa::vec3<float>>& poly,
		const std::string& texture,
		uint16_t id
	) : controller(body)
		, shape(poly, texture)
	{
		m_onTick = std::make_unique<OnTick>([](ObjectController& o, float dt) {
		});

		m_onInput = std::make_unique<OnInput>([](ObjectController& o, sa::UserIO& userio) {});
		controller.update();
	}

	SceneObject& teleportTo(const sa::vec2<float>& pos)
	{
		this->controller.teleportTo(pos);
		return *this;
	}

	SceneObject& setVelocity(const sa::vec2<float>& vel)
	{
		this->controller.setVelocity(vel);
		return *this;
	}

	const Transform& getTransform() const
	{
		return controller.getTransform();
	}

	void inputHandler(std::unique_ptr<OnInput>&& input)
	{
		m_onInput = std::move(input);
	}

	void update(sa::UserIO& io)
	{
		controller.update();
		m_onInput->update(controller, io);
	}

	void tick(float dt)
	{
		m_onTick->tick(controller, dt);
		controller.tick();		
	}

	void invalidate()
	{
		controller.invalidate();
	}

	void draw(std::shared_ptr<sa::Graphics> pGraphics) const
	{
		const auto& transform = controller.getTransform();
		m_modelMatrix.makeTranslationMatrix(transform.position.x, transform.position.y, 0);
		m_modelMatrix.scale(1.0f, 1.0f, 0.0f);
		m_modelMatrix.rotate(controller.getTransform().direction, 0, 0, 1);
		pGraphics->m_pRenderer->drawMesh(shape.mesh(), m_modelMatrix, shape.texture(), Color::WHITE);
	}

	uint16_t getId() const { return m_id; }
};