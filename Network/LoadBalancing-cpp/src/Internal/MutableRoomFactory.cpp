/* Exit Games Photon LoadBalancing - C++ Client Lib
 * Copyright (C) 2004-2016 by Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */
#include "NetPch.h"
#include "LoadBalancing-cpp/inc/Internal/MutableRoomFactory.h"
#include "LoadBalancing-cpp/inc/MutableRoom.h"

namespace ExitGames
{
	namespace LoadBalancing
	{
		using namespace Common::MemoryManagement;

		MutableRoomFactory::~MutableRoomFactory(void)
		{
		}

		MutableRoom* MutableRoomFactory::create(const Common::JString& name, const Common::Hashtable& properties, Client* pClient, const Common::JVector<Common::JString>& propsListedInLobby, int playerTtl, int emptyRoomTtl)
		{
			MutableRoom* p;
			return ALLOCATE(MutableRoom, p, name, properties, pClient, propsListedInLobby, playerTtl, emptyRoomTtl);
		}

		void MutableRoomFactory::destroy(const MutableRoom* pRoom)
		{
			deallocate(pRoom);
		}
	}
}