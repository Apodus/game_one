/* Exit Games Common - C++ Client Lib
 * Copyright (C) 2004-2016 by Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#pragma once

#include "Common-cpp/inc/defines.h"

#if defined _EG_WINDOWS_PLATFORM || defined _EG_WINDOWSSTORE_PLATFORM || defined _EG_XB1_PLATFORM
#	include <process.h>
#	include <mbstring.h>
#	if !defined _EG_XB1_PLATFORM
#		include <tchar.h>
#	endif
#	include <assert.h>
#	include <winsock2.h>
#	include <windows.h>
#	include <time.h>
#	include <ws2tcpip.h>
#	if defined _EG_WINDOWS_PLATFORM
#		include <wininet.h>
#	endif
#	include <urlmon.h>
#	include <string.h>
#elif defined _EG_UNIX_PLATFORM
#	include <pthread.h>
#	if defined _EG_APPLE_PLATFORM
#		include <AvailabilityMacros.h>
#		import <CoreFoundation/CFString.h>
#		import <CoreFoundation/CoreFoundation.h>
#		include <sys/time.h>
#	elif defined _EG_BLACKBERRY_PLATFORM
#		include <sys/time.h>
#		include <iconv.h>
#		include <limits.h>
#	elif defined _EG_MARMALADE_PLATFORM
#		include <limits.h>
#	elif defined _EG_ANDROID_PLATFORM
#		include <sys/time.h>
#		include <sys/limits.h>
#	elif defined _EG_PS3_PLATFORM
#		include <limits.h>
#		include <time.h>
#		include <assert.h>
#	endif
#endif