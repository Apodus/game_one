/* Exit Games Photon LoadBalancing - C++ Client Lib
 * Copyright (C) 2004-2016 by Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */
#include "NetPch.h"
#include "LoadBalancing-cpp/inc/Internal/PlayerPropertiesCacher.h"
#include "LoadBalancing-cpp/inc/Player.h"

namespace ExitGames
{
	namespace LoadBalancing
	{
		namespace Internal
		{
			void PlayerPropertiesCacher::cache(Player& player, const Common::Hashtable& properties)
			{
				player.cacheProperties(properties);
			}
			void PlayerPropertiesCacher::setIsInactive(Player& player, bool isInactive)
			{
				player.setIsInactive(isInactive);
			}
		}
	}
}