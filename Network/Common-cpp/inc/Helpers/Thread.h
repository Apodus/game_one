/* Exit Games Common - C++ Client Lib
 * Copyright (C) 2004-2016 by Exit Games. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#pragma once

#include "Common-cpp/inc/platformLayer.h"

namespace ExitGames
{
	namespace Common
	{
		namespace Helpers
		{
			namespace Thread
			{
				typedef void (*callbackSignature)(void* pArg);

				void create(callbackSignature onEnterThread, void* pArg);
			}
		}
	}
}