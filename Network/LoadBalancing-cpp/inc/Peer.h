/* Exit Games Photon LoadBalancing - C++ Client Lib
 * Copyright (C) 2004-2016 by Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#pragma once

#include "Photon-cpp/inc/PhotonPeer.h"
#include "Photon-cpp/inc/Enums/EventCode.h"
#include "Photon-cpp/inc/Enums/EventCache.h"
#include "Photon-cpp/inc/Enums/EventKey.h"
#include "Photon-cpp/inc/Enums/OperationCode.h"
#include "Photon-cpp/inc/Enums/ParameterCode.h"
#include "Photon-cpp/inc/Enums/ReceiverGroup.h"
#include "Photon-cpp/inc/Enums/ConnectionProtocol.h"
#include "LoadBalancing-cpp/inc/Enums/ErrorCode.h"
#include "LoadBalancing-cpp/inc/Enums/PeerStates.h"
#include "LoadBalancing-cpp/inc/Enums/LobbyType.h"
#include "LoadBalancing-cpp/inc/Enums/MatchmakingMode.h"
#include "LoadBalancing-cpp/inc/AuthenticationValues.h"
#include "LoadBalancing-cpp/inc/LobbyStatsRequest.h"
#include "LoadBalancing-cpp/inc/LobbyStatsResponse.h"
#include "LoadBalancing-cpp/inc/RaiseEventOptions.h"
#include "LoadBalancing-cpp/inc/RoomOptions.h"

namespace ExitGames
{
	namespace LoadBalancing
	{
		class Peer : public Photon::PhotonPeer
		{
		public:
			Peer(Photon::PhotonListener& listener, nByte connectionProtocol=Photon::ConnectionProtocol::DEFAULT);
			virtual ~Peer(void);

			virtual bool opJoinLobby(const Common::JString& lobbyName=Common::JString(), nByte lobbyType=LobbyType::DEFAULT);
			virtual bool opLeaveLobby(void);
			virtual bool opCreateRoom(const Common::JString& gameID, const RoomOptions& options=RoomOptions(), const Common::Hashtable& customLocalPlayerProperties=Common::Hashtable());
			virtual bool opJoinRoom(const Common::JString& gameID, const RoomOptions& options=RoomOptions(), const Common::Hashtable& customLocalPlayerProperties=Common::Hashtable(), bool createIfNotExists=false, int playerNumber=0, int cacheSliceIndex=0);
			virtual bool opJoinRandomRoom(const Common::Hashtable& customRoomProperties=Common::Hashtable(), nByte maxPlayers=0, nByte matchmakingMode=MatchmakingMode::FILL_ROOM, const Common::JString& lobbyName=Common::JString(), nByte lobbyType=LobbyType::DEFAULT, const Common::JString& sqlLobbyFilter=Common::JString());
			virtual bool opLeaveRoom(bool willComeBack=false);
			template<typename Ftype> typename Common::Helpers::EnableIf<!Common::Helpers::ConfirmAllowed<Ftype>::dimensions, bool>::type opRaiseEvent(bool reliable, Ftype parameters, nByte eventCode, RaiseEventOptions options=RaiseEventOptions())
			{
				return opRaiseEvent(reliable, Common::Helpers::ValueToObject::get(parameters), eventCode, options);
			}
			template<typename Ftype> typename Common::Helpers::EnableIf<Common::Helpers::ConfirmAllowed<Ftype>::dimensions==1, bool>::type opRaiseEvent(bool reliable, Ftype pParameterArray, typename Common::Helpers::ArrayLengthType<Ftype>::type arrSize, nByte eventCode, RaiseEventOptions options=RaiseEventOptions())
			{
				return opRaiseEvent(reliable, Common::Helpers::ValueToObject::get(pParameterArray, arrSize), eventCode, options);
			}
			template<typename Ftype> typename Common::Helpers::EnableIf<(Common::Helpers::ConfirmAllowed<Ftype>::dimensions>1), bool>::type opRaiseEvent(bool reliable, Ftype pParameterArray, const short* pArrSizes, nByte eventCode, RaiseEventOptions options=RaiseEventOptions())
			{
				return opRaiseEvent(reliable, Common::Helpers::ValueToObject::get(pParameterArray, pArrSizes), eventCode, options);
			}
			virtual bool opAuthenticate(const Common::JString& appID, const Common::JString& appVersion, bool encrypted, const AuthenticationValues& authenticationValues=AuthenticationValues(), bool lobbyStats=false, const Common::JString& regionCode=Common::JString());
			virtual bool opFindFriends(const Common::JString* friendsToFind, short numFriendsToFind);
			virtual bool opLobbyStats(const Common::JVector<LoadBalancing::LobbyStatsRequest>& lobbiesToQuery=Common::JVector<LoadBalancing::LobbyStatsRequest>());
			virtual bool opChangeGroups(const Common::JVector<nByte>* pGroupsToRemove, const Common::JVector<nByte>* pGroupsToAdd);
			virtual bool opWebRpc(const Common::JString& uriPath, const Common::Object& parameters);
			virtual bool opGetRegions(bool encrypted, const Common::JString& appID);
			virtual bool opSetPropertiesOfPlayer(int playerNr, const Common::Hashtable& properties);
			virtual bool opSetPropertiesOfRoom(const Common::Hashtable& properties, bool webForward = false);
		private:
			Photon::OperationRequestParameters opCreateRoomImplementation(const Common::JString& gameID, const RoomOptions& options, const Common::Hashtable& customLocalPlayerProperties);
			Photon::OperationRequestParameters opJoinRoomImplementation(const Common::JString& gameID, const RoomOptions& options, const Common::Hashtable& customLocalPlayerProperties, bool createIfNotExists, int playerNumber, int cacheSliceIndex);
			Photon::OperationRequestParameters enterRoomImplementation(const RoomOptions* pOptions, const Common::Hashtable& customLocalPlayerProperties);
			virtual bool opRaiseEvent(bool reliable, const Common::Object& parameters, nByte eventCode, RaiseEventOptions options);

			typedef PhotonPeer super;

			friend class Client;
		};
	}
}