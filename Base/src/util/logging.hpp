
#pragma once

#include "platform.hpp"
#include <cstdio>

#ifndef FINAL_BUILD
#define DEBUG_LEVEL 2
#else
#define DEBUG_LEVEL 1
#endif

#if DEBUG_LEVEL > 0

#if PLATFORM_WIN
#ifdef APIENTRY // glfw definition collides with winapi definition. they both define it as __stdcall
#undef APIENTRY
#endif
#include <Windows.h>
#endif

#define LOG_ACTUAL_DONT_USE__(x, ...) printf("%s::%s (%d): " x "\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_SIMPLE_ACTUAL_DONT_USE__(x, ...) printf(x, ##__VA_ARGS__)
#define LOG_SIMPLE_NEWLINE_ACTUAL_DONT_USE__(x, ...) printf(x "\n", ##__VA_ARGS__)

#if PLATFORM_WIN
#define ASSERT(x, msg, ...) {if(!(##x)){LOG_ACTUAL_DONT_USE__("ASSERT(%s) failed: " msg, #x, ##__VA_ARGS__); char assertBuffer[1024]; sprintf_s(assertBuffer, 1024,  "%s(%d): " #x "\n" ##msg "\n", __FILE__, __LINE__, ##__VA_ARGS__); OutputDebugString(assertBuffer); __debugbreak(); } }
#else
#define ASSERT(x, msg, ...) {if(!(x)){LOG_ACTUAL_DONT_USE__("ASSERT(%s) failed: " msg, #x, ##__VA_ARGS__); __builtin_trap(); } }
#endif

#if DEBUG_LEVEL > 1
#if PLATFORM_WIN
#define LOG(x, ...) {                                 \
	LOG_ACTUAL_DONT_USE__(x, ##__VA_ARGS__);          \
	char logBuffer[10240];                             \
	sprintf_s(logBuffer, 10240,  "%s(%d): " x "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
	OutputDebugString(logBuffer); \
}
#define LOGS(x, ...) {                                 \
	LOG_SIMPLE_ACTUAL_DONT_USE__(x, ##__VA_ARGS__);          \
	char logBuffer[10240];                             \
	sprintf_s(logBuffer, 10240,  x, ##__VA_ARGS__); \
	OutputDebugString(logBuffer); \
}
#define LOGSNL(x, ...) {                                 \
	LOG_SIMPLE_NEWLINE_ACTUAL_DONT_USE__(x, ##__VA_ARGS__);          \
	char logBuffer[10240];                             \
	sprintf_s(logBuffer, 10240,  x "\n", ##__VA_ARGS__); \
	OutputDebugString(logBuffer); \
}

#else
#define LOG(x, ...) LOG_ACTUAL_DONT_USE__(x, ##__VA_ARGS__)
#define LOGS(x, ...) LOG_SIMPLE_ACTUAL_DONT_USE__(x, ##__VA_ARGS__)
#define LOGSNL(x, ...) LOG_SIMPLE_NEWLINE_ACTUAL_DONT_USE__(x, ##__VA_ARGS__)
#endif
#else
#define LOG(x, ...)
#define LOGS(x, ...)
#define LOGSNL(x, ...)
#endif

#else // no debug

#define LOG(x, ...)
#define ASSERT(x, msg, ...)
#define LOGS(x, ...)
#define LOGSNL(x, ...)

#endif
