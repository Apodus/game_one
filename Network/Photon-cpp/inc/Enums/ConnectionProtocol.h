/* Exit Games Photon - C++ Client Lib
 * Copyright (C) 2004-2016 by Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#pragma once

#include "Common-cpp/inc/Common.h"

namespace ExitGames
{
	namespace Photon
	{
		/** These are the options that can be used as underlying transport protocol.*/
		namespace ConnectionProtocol
		{
#ifdef _EG_EMSCRIPTEN_PLATFORM
			static const nByte WS      = 0; ///<Use websockets to connect to Photon.
			static const nByte WSS     = 1; ///<Use secure websockets to connect to Photon.
			static const nByte DEFAULT = WS;
#else
			static const nByte UDP     = 0; ///<Use UDP to connect to Photon, which allows you to send operations reliable or unreliable on demand.
			static const nByte TCP     = 1; ///<Use TCP to connect to Photon.
			static const nByte DEFAULT = UDP;
#endif
		}
		/** @file */
	}
}