/* Exit Games Photon LoadBalancing - C++ Client Lib
 * Copyright (C) 2004-2016 by Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */
#include "NetPch.h"
#include "LoadBalancing-cpp/inc/Internal/MutablePlayerFactory.h"
#include "LoadBalancing-cpp/inc/MutablePlayer.h"

namespace ExitGames
{
	namespace LoadBalancing
	{
		using namespace Common::MemoryManagement;

		MutablePlayerFactory::~MutablePlayerFactory(void)
		{
		}

		MutablePlayer* MutablePlayerFactory::create(int number, const Common::Hashtable& properties, const MutableRoom* const pRoom, Client* pClient)
		{
			MutablePlayer* p;
			return ALLOCATE(MutablePlayer, p, number, properties, pRoom, pClient);
		}

		void MutablePlayerFactory::destroy(const MutablePlayer* pPlayer)
		{
			deallocate(pPlayer);
		}
	}
}