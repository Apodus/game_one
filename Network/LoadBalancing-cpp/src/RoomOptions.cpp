/* Exit Games Photon LoadBalancing - C++ Client Lib
 * Copyright (C) 2004-2016 by Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */
#include "NetPch.h"
#include "LoadBalancing-cpp/inc/RoomOptions.h"

/** @file LoadBalancing-cpp/inc/RoomOptions.h */

namespace ExitGames
{
	using namespace Common;

	namespace LoadBalancing
	{
		RoomOptions::RoomOptions(bool isVisible, bool isOpen, nByte maxPlayers, const Hashtable& customRoomProperties, const JVector<JString>& propsListedInLobby, const JString& lobbyName, nByte lobbyType, int playerTtl, int emptyRoomTtl)
			: mIsVisible(isVisible)
			, mIsOpen(isOpen)
			, mMaxPlayers(maxPlayers)
			, mCustomRoomProperties(customRoomProperties)
			, mPropsListedInLobby(propsListedInLobby)
			, mLobbyName(lobbyName)
			, mLobbyType(lobbyType)
			, mPlayerTtl(playerTtl)
			, mEmptyRoomTtl(emptyRoomTtl)
		{
		}

		RoomOptions::~RoomOptions(void)
		{
		};

		RoomOptions::RoomOptions(const RoomOptions& toCopy)
		{
			*this = toCopy;
		}

		RoomOptions& RoomOptions::operator=(const RoomOptions& toCopy)
		{
			mIsVisible = toCopy.mIsVisible;
			mIsOpen = toCopy.mIsOpen;
			mMaxPlayers = toCopy.mMaxPlayers;
			mCustomRoomProperties = toCopy.mCustomRoomProperties;
			mPropsListedInLobby = toCopy.mPropsListedInLobby;
			mLobbyName = toCopy.mLobbyName;
			mLobbyType = toCopy.mLobbyType;
			mPlayerTtl = toCopy.mPlayerTtl;
			mEmptyRoomTtl = toCopy.mEmptyRoomTtl;

			return *this;
		}

		bool RoomOptions::getIsVisible(void) const
		{
			return mIsVisible;
		}

		RoomOptions& RoomOptions::setIsVisible(bool isVisible)
		{
			mIsVisible = isVisible;
			return *this;
		}

		bool RoomOptions::getIsOpen(void) const
		{
			return mIsOpen;
		}

		RoomOptions& RoomOptions::setIsOpen(bool isOpen)
		{
			mIsOpen = isOpen;
			return *this;
		}

		nByte RoomOptions::getMaxPlayers(void) const
		{
			return mMaxPlayers;
		}

		RoomOptions& RoomOptions::setMaxPlayers(nByte maxPlayers)
		{
			mMaxPlayers = maxPlayers;
			return *this;
		}

		const Hashtable& RoomOptions::getCustomRoomProperties(void) const
		{
			return mCustomRoomProperties;
		}

		RoomOptions& RoomOptions::setCustomRoomProperties(const Hashtable& customRoomProperties)
		{
			mCustomRoomProperties = customRoomProperties; 
			return *this;
		}

		const JVector<JString>& RoomOptions::getPropsListedInLobby(void) const
		{
			return mPropsListedInLobby;
		}

		RoomOptions& RoomOptions::setPropsListedInLobby(const JVector<JString>& propsListedInLobby)
		{
			mPropsListedInLobby = propsListedInLobby;
			return *this;
		}

		const JString& RoomOptions::getLobbyName(void) const
		{
			return mLobbyName;
		}

		RoomOptions& RoomOptions::setLobbyName(const JString& lobbyName)
		{
			mLobbyName = lobbyName;
			return *this;
		}

		nByte RoomOptions::getLobbyType(void) const
		{
			return mLobbyType;
		}

		RoomOptions& RoomOptions::setLobbyType(nByte lobbyType)
		{
			mLobbyType = lobbyType;
			return *this;
		}

		int RoomOptions::getPlayerTtl(void) const
		{
			return mPlayerTtl;
		}

		RoomOptions& RoomOptions::setPlayerTtl(int playerTtl)
		{
			mPlayerTtl = playerTtl;
			return *this;
		}

		int RoomOptions::getEmptyRoomTtl(void) const
		{
			return mEmptyRoomTtl;
		}

		RoomOptions& RoomOptions::setEmptyRoomTtl(int emptyRoomTtl)
		{
			mEmptyRoomTtl = emptyRoomTtl;
			return *this;
		}

		JString& RoomOptions::toString(JString& retStr, bool withTypes) const
		{
			return retStr += JString(L"{isVisible=") + getIsVisible() + L", isOpen=" + getIsOpen() + L", maxPlayers=" + getMaxPlayers() + L", customRoomProperties=" + getCustomRoomProperties().toString(withTypes) + L", propsListedInLobby=" + getPropsListedInLobby().toString() + L", lobbyName=" + getLobbyName().toString() + L", lobbyType" + getLobbyType() + L", playerTtl=" + getPlayerTtl() + L", emptyRoomTtl=" + getEmptyRoomTtl() + L"}";
		}
	}
}