
#include "fps_manager.hpp"
#include "util/logging.hpp"

FPS_Manager::FPS_Manager(long long time, long long fps)
{
	m_frameCount = 0;
	setStartTime(time);
	m_desiredFps = fps;
}

int64_t FPS_Manager::resetTime(const long long& time)
{
	//LOG("Warning, FPS timer reset!");
	setStartTime(time);
	m_frameCount = 0;
	return 0;
}

float FPS_Manager::frameLength() const
{
	return 1.0f / m_desiredFps;
}

float FPS_Manager::timeSince(const long long& time) const
{
	return static_cast<float>(time - m_prevTick) / 1000.0f;
}

float FPS_Manager::tick(const long long& time)
{
	float retVal = static_cast<float>(time - m_prevTick);
	m_prevTick = time;
	++m_frameCount;
	return retVal / 1000.0f;
}

void FPS_Manager::setStartTime(const long long& time)
{
	m_startTime = time;
	m_prevTick = time;
}

int FPS_Manager::canTick(const long long& time)
{
	long long real_time = time - m_startTime;
	
	long long missing_frames = m_desiredFps * real_time / 1000 - m_frameCount;
	if(missing_frames > 25)
	{
		resetTime(time);
		return 1;
	}
	
	if(1000 * m_frameCount / (1 + real_time) < m_desiredFps)
		return 1;
	return 0;
}
