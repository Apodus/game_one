/* Exit Games Photon LoadBalancing - C++ Client Lib
 * Copyright (C) 2004-2016 by Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */
#include "NetPch.h"
#include "LoadBalancing-cpp/inc/Internal/PlayerFactory.h"
#include "LoadBalancing-cpp/inc/Player.h"

namespace ExitGames
{
	namespace LoadBalancing
	{
		namespace Internal
		{
			using namespace Common::MemoryManagement;

			Player* PlayerFactory::create(int number, const Common::Hashtable& properties, const MutableRoom* const pRoom)
			{
				Player* p;
				return ALLOCATE(Player, p, number, properties, pRoom);
			}

			void PlayerFactory::destroy(const Player* pPlayer)
			{
				deallocate(pPlayer);
			}
		}
	}
}