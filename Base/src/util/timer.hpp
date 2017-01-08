#ifndef TIMER_H
#define TIMER_H

#include <cstdint>

class Timer
{
public:
	static int64_t time_now();
	static void sleep(int milliseconds);
};

#endif

