
#pragma once

#define PLATFORM_WIN 0
#define PLATFORM_MAC 0
#define PLATFORM_LINUX 0

#define VS2012 0
#define VS2013 0

#if defined(_WIN32) || defined(_WIN64)
#undef PLATFORM_WIN
#define PLATFORM_WIN 1

// detect visual studio version
#if _MSC_VER == 1800
#undef VS2013
#define VS2013 1
#elif _MSC_VER == 1700
#undef VS2012
#define VS2012 1
#elif _MSC_VER < 1700
#error Please use visual studio 2012 or later
#endif

#elif defined(__APPLE__)
#undef PLATFORM_MAX
#define PLATFORM_MAX 1
#elif defined(__linux) || defined(__unix)
#undef PLATFORM_LINUX
#define PLATFORM_LINUX 1
#else
#error platform not detected!
#endif
