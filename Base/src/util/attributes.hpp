
#pragma once

#include "util/platform.hpp"

#if PLATFORM_WIN
	#define INLINE						__inline
	#define THREAD_LOCAL				__declspec(thread)
	#define UNIQUE						__declspec(restrict)
	#define PREFETCH_READ(x)			_mm_prefetch(reinterpret_cast<const char *>(x), _MM_HINT_T1)
	#define PREFETCH_READ_NTA(x)		_mm_prefetch(reinterpret_cast<const char *>(x), _MM_HINT_NTA)
	#define PREFETCH_WRITE(x)			PREFETCH_READ(x)
	#define PREFETCH_WRITE_NTA(x)		PREFETCH_READ_NTA(x)
#elif PLATFORM_LINUX
	#define INLINE						inline
	#define THREAD_LOCAL				__thread
	#define UNIQUE						__restrict__
	#define PREFETCH_READ(x)			__builtin_prefetch(x, 0, 3)
	#define PREFETCH_READ_NTA(x)		__builtin_prefetch(x, 0, 0)
	#define PREFETCH_WRITE(x)			__builtin_prefetch(x, 1, 3)
	#define PREFETCH_WRITE_NTA(x)		__builtin_prefetch(x, 1, 0)
#else
	#error Platform defines not handled for this platform!
#endif