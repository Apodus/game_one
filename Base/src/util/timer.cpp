#include "timer.hpp"
#include "util/platform.hpp"

#if !PLATFORM_WIN
#include <sys/time.h>
#else
#include <windows.h>
#endif

#include <ctime>

long long Timer::time_now()
{
#if PLATFORM_WIN
	return clock();
#else
	timeval t;
	gettimeofday(&t,NULL);
	return t.tv_sec * 1000 + t.tv_usec / 1000;
#endif
}

void Timer::sleep(int milliseconds)
{
#if PLATFORM_WIN
	Sleep(milliseconds);
#else
	timespec duration = { 0, 1000000 * milliseconds };
	nanosleep(&duration, 0);
#endif
}

