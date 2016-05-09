#pragma once

#include "logic/Transform.hpp"

class ObjectController
{
public:
	ObjectController(b2Body* body)
		:
		m_transform(body),
		turnSpeed(0),
		forwardSpeed(0)
	{
	}

	ObjectController(ObjectController&& other)
		: m_transform(std::move(other.m_transform))
	{
		forwardSpeed = other.forwardSpeed;
		turnSpeed = other.turnSpeed;
	}

	ObjectController& operator = (ObjectController&& other)
	{
		m_transform = std::move(other.m_transform);
		m_transform.update();
		forwardSpeed = other.forwardSpeed;
		turnSpeed = other.turnSpeed;
		return *this;
	}

	const Transform& ObjectController::getTransform() const
	{
		return m_transform;
	}

	void update()
	{
		m_transform.update();
	}

	void setTurning(float force)
	{
		turnSpeed = force;
	}

	void setForwardSpeed(float force)
	{
		forwardSpeed = force;
	}

	void tick()
	{
		if (turnSpeed != 0)
		{
			m_transform.m_body->ApplyTorque(10.0f * turnSpeed, true);
		}

		if (forwardSpeed > 0)
		{
			m_transform.m_body->ApplyForceToCenter({
				+sa::math::sin(m_transform.direction) * 100.1f * forwardSpeed,
				-sa::math::cos(m_transform.direction) * 100.1f * forwardSpeed
			},
				true
			);
		}
		else if (forwardSpeed < 0)
		{
			m_transform.m_body->ApplyForceToCenter({
				+sa::math::sin(m_transform.direction) * 10.0f * forwardSpeed,
				-sa::math::cos(m_transform.direction) * 10.0f * forwardSpeed
			},
				true
			);
		}
	}

	ObjectController& teleportTo(const sa::vec2<float>& pos)
	{
		m_transform.m_body->SetTransform({ pos.x, pos.y }, m_transform.direction);
		return *this;
	}

	ObjectController& setVelocity(const sa::vec2<float>& vel)
	{
		m_transform.m_body->SetLinearVelocity({ vel.x, vel.y });
		return *this;
	}

private:
	Transform m_transform;

	float turnSpeed;
	float forwardSpeed;


};