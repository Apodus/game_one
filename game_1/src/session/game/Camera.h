#pragma once

class Camera
{
public:
	sa::vec3<float> pos;
	sa::vec3<float> target;

	void tick(long long timeMs)
	{
		pos += (target - pos) * 200.0f / 1000.0f;
	}
};

