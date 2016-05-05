#pragma once

#include "Photon-cpp/inc/PhotonPeer.h"
#include "LoadBalancing-cpp/inc/Client.h"

#include <chrono>
#include <array>

namespace net
{
	class Photon : private ExitGames::LoadBalancing::Listener
	{
	public:
		Photon();

		enum State
		{
			STATE_INITIALIZED = 0,
			STATE_CONNECTING,
			STATE_CONNECTED,
			STATE_JOINING,
			STATE_JOINED,
			STATE_LEAVING,
			STATE_LEFT,
			STATE_DISCONNECTING,
			STATE_DISCONNECTED
		};

		enum Input
		{
			INPUT_NON = 0,
			INPUT_1,
			INPUT_2,
			INPUT_EXIT
		};

		class Listener : public ExitGames::Common::ToString
		{
		public:




			using ExitGames::Common::ToString::toString;
			virtual void stateUpdate(State newState) = 0;
			virtual ExitGames::Common::JString& toString(ExitGames::Common::JString& retStr, bool withTypes = false) const;
		};

		class StateAccessor
		{
		public:
			State getState(void) const { return mState; }
			void setState(State newState)
			{
				mState = newState;
				for (unsigned int i = 0; i<mStateUpdateListeners.getSize(); i++)
					mStateUpdateListeners[i]->stateUpdate(newState);
			}

			void registerForStateUpdates(Listener* listener)
			{
				mStateUpdateListeners.addElement(listener);
			}

		private:
			State mState;
			ExitGames::Common::JVector<Listener*> mStateUpdateListeners;
		};

		void registerForStateUpdates(Listener* listener);
		void run(void);
		void connect(const ExitGames::LoadBalancing::AuthenticationValues& authenticationValues = ExitGames::LoadBalancing::AuthenticationValues());
		void opCreateRoom(void);
		void opJoinRandomRoom(void);
		void opJoinOrCreateRoom(void);
		void disconnect(void);
		void Service(void);

		// Input getLastInput(void) const;
		// void setLastInput(Input newInput);
		// State getState(void) const;


	private:
		// receive and print out debug out here
		virtual void debugReturn(int debugLevel, const ExitGames::Common::JString& string);

		// implement your error-handling here
		virtual void connectionErrorReturn(int errorCode);
		virtual void clientErrorReturn(int errorCode);
		virtual void warningReturn(int warningCode);
		virtual void serverErrorReturn(int errorCode);

		// events, triggered by certain operations of all players in the same room
		virtual void joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& playernrs, const ExitGames::LoadBalancing::Player& player);
		virtual void leaveRoomEventAction(int playerNr, bool isInactive);
		virtual void customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent);

		virtual void onLobbyStatsResponse(const ExitGames::Common::JVector<ExitGames::LoadBalancing::LobbyStatsResponse>& lobbyStats);
		virtual void onLobbyStatsUpdate(const ExitGames::Common::JVector<ExitGames::LoadBalancing::LobbyStatsResponse>& lobbyStats);
		virtual void onAvailableRegions(const ExitGames::Common::JVector<ExitGames::Common::JString>& availableRegions, const ExitGames::Common::JVector<ExitGames::Common::JString>& availableRegionServers);

		// callbacks for operations on PhotonLoadBalancing server
		virtual void connectReturn(int errorCode, const ExitGames::Common::JString& errorString);
		virtual void disconnectReturn(void);
		virtual void createRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString);
		virtual void joinOrCreateRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString);
		virtual void joinRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString);
		virtual void joinRandomRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& gameProperties, const ExitGames::Common::Hashtable& playerProperties, int errorCode, const ExitGames::Common::JString& errorString);
		virtual void leaveRoomReturn(int errorCode, const ExitGames::Common::JString& errorString);
		virtual void joinLobbyReturn(void);
		virtual void leaveLobbyReturn(void);


		void LogMeasurements();

		ExitGames::LoadBalancing::Client myLoadBalancingClient;
		ExitGames::Common::JString mLastJoinedRoom;
		int mLastPlayerNr;
		ExitGames::Common::Logger mLogger;
		StateAccessor mStateAccessor;
		Input mLastInput;
		// OutputListener* mpOutputListener;
		bool mAutoJoinRoom;

		ExitGames::Common::JString myClientName;

		std::chrono::time_point<std::chrono::high_resolution_clock> myLastUpdateSentTime;

		struct Measurement
		{
			std::chrono::time_point<std::chrono::high_resolution_clock> myUpdateTime;
			uint32_t myUpdateSentCount = 0;
			int myMeasuredBytesIn = 0;
			int myPayloadBytesIn = 0;
			int myMeasuredBytesOut = 0;
			int myPayloadBytesOut = 0;
		};

		std::array<Measurement, 3> myMeasurements;

		int myPayloadBytesIn = 0;
		int myPayloadBytesOut = 0;
		uint32_t myUpdateSendCount = 0;
	};
}
