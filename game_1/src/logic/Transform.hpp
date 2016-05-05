#pragma once

#include "Box2D/Box2D.h"
#include "util/vec2.hpp"

class Transform
{
public:
	~Transform()
	{
		if (m_body) {
			m_body->GetWorld()->DestroyBody(m_body);
			m_body = nullptr;
		}
	}

	sa::vec2<float> position;
	float direction;

	b2Body* m_body;

	Transform(b2Body* body) : direction(0)
	{
		m_body = body;
	}

	Transform(Transform&& other)
	{
		operator = (std::move(other));
	}

	Transform& operator = (Transform&& other)
	{
		m_body = other.m_body;
		other.m_body = nullptr;

		position = other.position;
		direction = other.direction;
		return *this;
	}

	void update()
	{
		if (m_body)
		{
			auto pos = m_body->GetWorldCenter();
			position.x = pos.x;
			position.y = pos.y;

			direction = m_body->GetAngle();
		}
	}

	sa::vec2<float> directionForward() const
	{
		return sa::vec2<float>(
			sa::math::sin(direction),
			-sa::math::cos(direction)
		);
	}

	sa::vec2<float> directionLeft() const
	{
		return sa::vec2<float>(
			sa::math::cos(direction),
			sa::math::sin(direction)
		);
	}
};