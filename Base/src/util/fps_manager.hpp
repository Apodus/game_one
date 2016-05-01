
#pragma once

#include <cstdint>

class FPS_Manager
{
	int64_t m_frameCount;
	int64_t m_startTime;
	int64_t m_desiredFps;
	int64_t m_prevTick;

public:
	FPS_Manager(long long time, long long fps = 50);

	void    setStartTime(const long long& time);
	int64_t resetTime(const long long& time);
	
	float	frameLength() const;
	float	timeSince(const long long& time) const;
	float	tick(const long long& time);
	int     canTick(const long long& time);
};
