/* Exit Games Photon LoadBalancing - C++ Client Lib
 * Copyright (C) 2004-2016 by Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */
#include "NetPch.h"
#include "LoadBalancing-cpp/inc/Client.h"
#include "LoadBalancing-cpp/inc/MutablePlayer.h"
#include "LoadBalancing-cpp/inc/MutableRoom.h"
#include "LoadBalancing-cpp/inc/Internal/Utils.h"
#include "LoadBalancing-cpp/inc/Internal/Enums/Properties/Player.h"

/** @file inc/MutablePlayer.h */

namespace ExitGames
{
	namespace LoadBalancing
	{
		using namespace Common;
		using namespace Internal;

		MutablePlayer::MutablePlayer(int number, const Hashtable& properties, const MutableRoom* const pRoom, Client* pClient)
			: Player(number, properties, pRoom)
			, mLoadBalancingClient(pClient)
		{
		}

		MutablePlayer::~MutablePlayer(void)
		{
		}

		MutablePlayer::MutablePlayer(const MutablePlayer& toCopy) : Player(toCopy)
		{
			*this = toCopy;
		}

		MutablePlayer& MutablePlayer::operator=(const Player& toCopy)
		{
			return assign(toCopy);
		}

		bool MutablePlayer::getIsMutable(void) const
		{
			return true;
		}

		MutablePlayer& MutablePlayer::assign(const Player& toCopy)
		{
			super::assign(toCopy);
			const MutablePlayer& temp = static_cast<const MutablePlayer&>(toCopy);
			if(temp.getIsMutable())
				mLoadBalancingClient = temp.mLoadBalancingClient;
			return *this;
		}

		void MutablePlayer::mergeCustomProperties(const Hashtable& customProperties)
		{
			Hashtable stripDict = Utils::stripToCustomProperties(customProperties);
			if(!stripDict.getSize())
				return;
			Hashtable oldDict = mCustomProperties;
			mCustomProperties.put(stripDict);
			mCustomProperties = Utils::stripKeysWithNullValues(mCustomProperties);
			if(mCustomProperties != oldDict)
				mLoadBalancingClient->opSetPropertiesOfPlayer(mNumber, stripDict);
		}

		void MutablePlayer::addCustomProperties(const Hashtable& customProperties)
		{
			mergeCustomProperties(Utils::stripKeysWithNullValues(customProperties));
		}

		void MutablePlayer::setName(const JString& name)
		{
			if(mName != name)
			{
				Hashtable properties;
				properties.put(static_cast<nByte>(Properties::Player::PLAYERNAME), mName=name);
				mLoadBalancingClient->opSetPropertiesOfPlayer(mNumber, properties);
			}
		}
	}
}