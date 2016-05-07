#pragma once

#include "logic/Transform.hpp"

class ObjectController
{
public:
	ObjectController(b2Body* body)
		:
		transform(body),
		turnSpeed(0),
		forwardSpeed(0)
	{
	}

	ObjectController(ObjectController&& other)
		: transform(std::move(other.transform))
	{
		forwardSpeed = other.forwardSpeed;
		turnSpeed = other.turnSpeed;
	}

	ObjectController& operator = (ObjectController&& other)
	{
		transform = std::move(other.transform);
		transform.update();
		forwardSpeed = other.forwardSpeed;
		turnSpeed = other.turnSpeed;
		return *this;
	}

	Transform& ObjectController::getTransform()
	{
		return transform;
	}

	void update()
	{
		transform.update();
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
			transform.m_body->ApplyTorque(10.0f * turnSpeed, true);
		}

		if (forwardSpeed > 0)
		{
			transform.m_body->ApplyForceToCenter({
				+sa::math::sin(transform.direction) * 100.1f * forwardSpeed,
				-sa::math::cos(transform.direction) * 100.1f * forwardSpeed
			},
				true
			);
		}
		else if (forwardSpeed < 0)
		{
			transform.m_body->ApplyForceToCenter({
				+sa::math::sin(transform.direction) * 10.0f * forwardSpeed,
				-sa::math::cos(transform.direction) * 10.0f * forwardSpeed
			},
				true
			);
		}
	}

private:
	Transform transform;

	float turnSpeed;
	float forwardSpeed;


};