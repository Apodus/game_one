/* Exit Games Photon LoadBalancing - C++ Client Lib
 * Copyright (C) 2004-2016 by Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */
#include "NetPch.h"
#include "LoadBalancing-cpp/inc/Client.h"
#include "LoadBalancing-cpp/inc/MutableRoom.h"
#include "LoadBalancing-cpp/inc/Internal/Utils.h"
#include "LoadBalancing-cpp/inc/Internal/Enums/Properties/Room.h"

/** @file inc/MutableRoom.h */

namespace ExitGames
{
	namespace LoadBalancing
	{
		using namespace Common;
		using namespace Common::MemoryManagement;
		using namespace Internal;

		MutableRoom::MutableRoom(const JString& name, const Hashtable& properties, Client* pClient, const JVector<JString>& propsListedInLobby, int playerTtl, int emptyRoomTtl)
			: super(name, properties)
			, mLoadBalancingClient(pClient)
			, mIsVisible(true)
			, mPlayers(JVector<Player*>())
			, mMasterClientID(0)
			, mPropsListedInLobby(propsListedInLobby)
			, mPlayerTtl(playerTtl)
			, mEmptyRoomTtl(emptyRoomTtl)
		{
			cacheProperties(properties);
		}

		MutableRoom::~MutableRoom(void)
		{
			destroyAllPlayers();
		}

		MutableRoom::MutableRoom(const MutableRoom& toCopy)
			: super(toCopy)
		{
			*this = toCopy;
		}

		MutableRoom& MutableRoom::operator=(const Room& toCopy)
		{
			return assign(toCopy);
		}

		bool MutableRoom::getIsMutable(void) const
		{
			return true;
		}

		MutableRoom& MutableRoom::assign(const Room& toCopy)
		{
			super::assign(toCopy);
			const MutableRoom& temp = static_cast<const MutableRoom&>(toCopy);
			if(!temp.getIsMutable())
				return *this;
			mLoadBalancingClient = temp.mLoadBalancingClient;
			mIsVisible = temp.mIsVisible;
			mPlayers = temp.mPlayers;
			mMasterClientID = temp.mMasterClientID;
			mPropsListedInLobby = temp.mPropsListedInLobby;
			mLoadBalancingClient = temp.mLoadBalancingClient;
			return *this;
		}

		void MutableRoom::mergeCustomProperties(const Hashtable& customProperties, bool webForward)
		{
			Hashtable stripDict = Utils::stripToCustomProperties(customProperties);
			if(!stripDict.getSize())
				return;
			Hashtable oldDict = mCustomProperties;
			mCustomProperties.put(stripDict);
			mCustomProperties = Utils::stripKeysWithNullValues(mCustomProperties);
			if(mCustomProperties != oldDict)
				mLoadBalancingClient->opSetPropertiesOfRoom(stripDict, webForward);
		}

		void MutableRoom::addCustomProperties(const Hashtable& customProperties, bool webForward)
		{
			mergeCustomProperties(Utils::stripKeysWithNullValues(customProperties), webForward);
		}

		void MutableRoom::cacheProperties(const Hashtable& properties)
		{
			if(properties.contains(static_cast<nByte>(Properties::Room::IS_VISIBLE)))
				mIsVisible = ValueObject<bool>(properties.getValue(static_cast<nByte>(Properties::Room::IS_VISIBLE))).getDataCopy();
			super::cacheProperties(properties);
		}

		JString MutableRoom::toString(bool withTypes, bool withCustomProperties, bool withPlayers) const
		{
			return mName + L"={" + payloadToString(withTypes, withCustomProperties, withPlayers) + L"}";
		}

		JString MutableRoom::payloadToString(bool withTypes, bool withCustomProperties, bool withPlayers) const
		{
			JString res = JString(L"masterClient: ") + mMasterClientID + L" " + super::payloadToString() + L" visible: " + mIsVisible;
			if(mPropsListedInLobby.getSize())
				res += L" propsListedInLobby: " + mPropsListedInLobby.toString(withTypes);
			if(withCustomProperties && mCustomProperties.getSize())
				res += L" props: " + mCustomProperties.toString(withTypes);
			if(withPlayers && mPlayers.getSize())
			{
				res += L" players: ";
				for(unsigned int i=0; i<mPlayers.getSize(); ++i)
					res += mPlayers[i]->toString(withTypes, withCustomProperties) + (i<mPlayers.getSize()-1?L", ":L"");
			}
			return res;
		}

		nByte MutableRoom::getPlayerCount(void) const
		{
			return mPlayers.getSize();
		}

		void MutableRoom::setMaxPlayers(nByte maxPlayers, bool webForward)
		{
			if(mMaxPlayers != maxPlayers)
				setRoomProperty(static_cast<nByte>(Properties::Room::MAX_PLAYERS), mMaxPlayers=maxPlayers);
		}

		void MutableRoom::setIsOpen(bool isOpen, bool webForward)
		{
			if(mIsOpen != isOpen)
				setRoomProperty(static_cast<nByte>(Properties::Room::IS_OPEN), mIsOpen=isOpen);
		}

		bool MutableRoom::getIsVisible(void) const
		{
			return mIsVisible;
		}

		void MutableRoom::setIsVisible(bool isVisible, bool webForward)
		{
			if(mIsVisible != isVisible)
				setRoomProperty(static_cast<nByte>(Properties::Room::IS_VISIBLE), mIsVisible=isVisible);
		}

		template<typename Etype> void MutableRoom::setRoomProperty(nByte key, Etype val, bool webForward)
		{
			Hashtable properties;
			properties.put(key, val);
			mLoadBalancingClient->opSetPropertiesOfRoom(properties, webForward);
		}

		const JVector<Player*>& MutableRoom::getPlayers(void) const
		{
			return mPlayers;
		}
		
		const Player* MutableRoom::getPlayerForNumber(int playerNumber) const
		{
			for(unsigned i=0; i<mPlayers.getSize(); i++)
				if(mPlayers[i]->getNumber() == playerNumber)
					return mPlayers[i];
			return NULL;
		}

		int MutableRoom::getMasterClientID(void) const
		{
			return mMasterClientID;
		}

		const JVector<JString>& MutableRoom::getPropsListedInLobby(void) const
		{
			return mPropsListedInLobby;
		}

		void MutableRoom::setPropsListedInLobby(const JVector<JString>& propsListedInLobby, bool webForward)
		{
			if(mPropsListedInLobby != propsListedInLobby)
			{
				mPropsListedInLobby = propsListedInLobby;
				JString* propsListedInLobbyArr = allocateArray<JString>(propsListedInLobby.getSize());
				for(unsigned int i=0; i<propsListedInLobby.getSize(); ++i)
					propsListedInLobbyArr[i] = propsListedInLobby[i];
				Hashtable properties;
				properties.put(static_cast<nByte>(Properties::Room::PROPS_LISTED_IN_LOBBY), propsListedInLobbyArr, propsListedInLobby.getSize());
				deallocateArray(propsListedInLobbyArr);
				mLoadBalancingClient->opSetPropertiesOfRoom(properties, webForward);
			}
		}

		int MutableRoom::getPlayerTtl(void) const
		{
			return mPlayerTtl;
		}

		int MutableRoom::getEmptyRoomTtl(void) const
		{
			return mEmptyRoomTtl;
		}

		Player* MutableRoom::createPlayer(int number, const Hashtable& properties) const
		{
			return PlayerFactory::create(number, properties, this);
		}

		void MutableRoom::destroyPlayer(const Player* pPlayer) const
		{
			PlayerFactory::destroy(pPlayer);
		}

		void MutableRoom::setPlayers(const JVector<Player*>& players)
		{
			mPlayers = players;
		}

		void MutableRoom::removeAllPlayers(void)
		{
			destroyAllPlayers();
			mPlayers.removeAllElements();
		}

		void MutableRoom::destroyAllPlayers(void)
		{
			for(unsigned int i=0; i<mPlayers.getSize(); ++i)
				if(mPlayers[i]->getNumber() != mLocalPlayerNumber)
					destroyPlayer(mPlayers[i]);
		}
		
		void MutableRoom::addPlayer(Player& player)
		{
			mPlayers.addElement(&player);
			if(!mMasterClientID || player.getNumber() < mMasterClientID)
				mMasterClientID = player.getNumber();
		}
		
		void MutableRoom::addLocalPlayer(Player& player)
		{
			mLocalPlayerNumber = player.getNumber();
			addPlayer(player);
		}
		
		void MutableRoom::addPlayer(int number, const Common::Hashtable& properties)
		{
			addPlayer(*createPlayer(number, properties));
		}
		
		bool MutableRoom::removePlayer(int number)
		{
			bool retVal = false;
			for(unsigned int i=0; i<mPlayers.getSize(); ++i)
			{
				if(mPlayers[i]->getNumber() == number)
				{
					destroyPlayer(mPlayers[i]);
					mPlayers.removeElementAt(i);
					retVal = true;
				}
			}

			if(number == mMasterClientID)
			{
				mMasterClientID = mPlayers.getSize()?mPlayers[0]->getNumber():0;
				for(unsigned int i=1; i<mPlayers.getSize(); ++i)
					if(mPlayers[i]->getNumber() < mMasterClientID)
						mMasterClientID = mPlayers[i]->getNumber();
			}
			return retVal;
		}

		JVector<Player*>& MutableRoom::getNonConstPlayers(void)
		{
			return mPlayers;
		}
	}
}