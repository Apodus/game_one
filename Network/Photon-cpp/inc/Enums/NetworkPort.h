/* Exit Games Photon - C++ Client Lib
 * Copyright (C) 2004-2016 by Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#pragma once

namespace ExitGames
{
	namespace Photon
	{
		namespace NetworkPort
		{
#ifdef _EG_EMSCRIPTEN_PLATFORM
			static const int WS  =  9090;
			static const int WSS = 19090;
#else
			static const int UDP =  5055;
			static const int TCP =  4530;
#endif
		}
		/** @file */
	}
}