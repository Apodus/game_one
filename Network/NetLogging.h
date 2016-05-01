#pragma once

#include <stdio.h>
#include <wchar.h>

#define NET_LOG(x, ...) \
{ \
	wchar_t logBuffer[10240];                             \
	swprintf(logBuffer, 10240, L"%hs(%d): Net: " x "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
	OutputDebugString(logBuffer); \
}

