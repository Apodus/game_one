#include "NetPch.h"
#include "NetPhoton.h"
#include "NetLogging.h"


static const ExitGames::Common::JString appId = L"357c978b-3bfc-4f60-a2f0-143560e2bba8"; // set your app id here
static const ExitGames::Common::JString appVersion = L"1.0";

static const bool autoLobbbyStats = true;
static const bool useDefaultRegion = false;


net::Photon::Photon()
	: myLoadBalancingClient(*this, appId, appVersion, ExitGames::Photon::ConnectionProtocol::DEFAULT, autoLobbbyStats, useDefaultRegion)
{
	NET_LOG("Photon started");
	char buffer[1024];
	sprintf_s(buffer, 1024, "%s%d", "Player", rand());
	myClientName = buffer;
}

void net::Photon::registerForStateUpdates(Listener* listener)
{
	mStateAccessor.registerForStateUpdates(listener);
}

void net::Photon::connect(const ExitGames::LoadBalancing::AuthenticationValues& authenticationValues)
{
	// mpOutputListener->writeLine(ExitGames::Common::JString(L"connecting to Photon"));
	myLoadBalancingClient.connect(authenticationValues, myClientName);
	mStateAccessor.setState(STATE_CONNECTING);
}

void net::Photon::disconnect(void)
{
	myLoadBalancingClient.disconnect();
}

void net::Photon::opCreateRoom(void)
{
	ExitGames::Common::JString name;
	name += GETTIMEMS();
	myLoadBalancingClient.opCreateRoom(name, ExitGames::LoadBalancing::RoomOptions().setMaxPlayers(4).setPlayerTtl(INT_MAX / 2).setEmptyRoomTtl(10000));
	mStateAccessor.setState(STATE_JOINING);
	// mpOutputListener->writeLine(ExitGames::Common::JString(L"creating room ") + name + L"...");
}

void net::Photon::opJoinRandomRoom(void)
{
	myLoadBalancingClient.opJoinRandomRoom();
}

void net::Photon::opJoinOrCreateRoom(void)
{
	ExitGames::Common::JString name("GameOne");
	myLoadBalancingClient.opJoinOrCreateRoom(name);
	mStateAccessor.setState(STATE_JOINING);
	// mpOutputListener->writeLine(ExitGames::Common::JString(L"joining or creating room ") + name + L"...");
}

void net::Photon::run(void)
{
	State state = mStateAccessor.getState();
	if (mLastInput == INPUT_EXIT && state != STATE_DISCONNECTING && state != STATE_DISCONNECTED)
	{
		disconnect();
		mStateAccessor.setState(STATE_DISCONNECTING);
		// mpOutputListener->writeLine(L"terminating application");
	}
	else
	{
		switch (state)
		{
		case STATE_INITIALIZED:
			connect();
			break;
		case STATE_CONNECTING:
			break; // wait for callback
		case STATE_CONNECTED:
#if 0
			switch (mLastInput)
			{
			case INPUT_1: // create Game
				// mpOutputListener->writeLine(L"\n=========================");
				opCreateRoom();
				break;
			case INPUT_2: // join Game
#endif
					// mpOutputListener->writeLine(L"\n=========================");
					// remove false to enable rejoin
					if (false && mLastJoinedRoom.length())
					{
						// mpOutputListener->writeLine(ExitGames::Common::JString(L"rejoining ") + mLastJoinedRoom + " with actorNr = " + mLastPlayerNr + "...");
						myLoadBalancingClient.opJoinRoom(mLastJoinedRoom, mLastPlayerNr);
					}
					else
					{
						// mpOutputListener->writeLine(ExitGames::Common::JString(L"joining random room..."));
						/*NET_LOG("Joining random room ")
							opJoinRandomRoom();*/
						opJoinOrCreateRoom();
					}
					mStateAccessor.setState(STATE_JOINING);
				
#if 0
				break;
			default: // no or illegal input -> stay waiting for legal input
				break;
			}
#endif
			break;
		case STATE_JOINING:
			break; // wait for callback
		case STATE_JOINED:
			sendEvent();
			switch (mLastInput)
			{
			case INPUT_1: // leave Game
				myLoadBalancingClient.opLeaveRoom();
				// mpOutputListener->writeLine(L"");
				// mpOutputListener->writeLine(L"leaving room");
				NET_LOG("Leaving room ")
				mStateAccessor.setState(STATE_LEAVING);
				break;
			case INPUT_2: // leave Game
				myLoadBalancingClient.opLeaveRoom(true);
				// mpOutputListener->writeLine(L"");
				// mpOutputListener->writeLine(L"leaving room (will come back)");
				mStateAccessor.setState(STATE_LEAVING);
				NET_LOG("Leaving room ")
				break;
			default: // no or illegal input -> stay waiting for legal input
				break;
			}
			break;
		case STATE_LEAVING:
			break; // wait for callback
		case STATE_LEFT:
			mStateAccessor.setState(STATE_CONNECTED);
			break;
		case STATE_DISCONNECTING:
			break; // wait for callback
		default:
			break;
		}
	}
	mLastInput = INPUT_NON;
	myLoadBalancingClient.service();
}

void net::Photon::sendEvent(void)
{
	static int64 count = 0;
	myLoadBalancingClient.opRaiseEvent(false, ++count, 0);
	// mpOutputListener->write(ExitGames::Common::JString(L"s") + count + L" ");
#if defined _EG_EMSCRIPTEN_PLATFORM
	mpOutputListener->writeLine(L"");
#endif
}

// protocol implementations

void net::Photon::debugReturn(int debugLevel, const ExitGames::Common::JString& string)
{
	// mpOutputListener->debugReturn(debugLevel, string);
}

void net::Photon::connectionErrorReturn(int errorCode)
{
	EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"code: %d", errorCode);
	// mpOutputListener->writeLine(ExitGames::Common::JString(L"received connection error ") + errorCode);
	mStateAccessor.setState(STATE_DISCONNECTED);
}

void net::Photon::clientErrorReturn(int errorCode)
{
	NET_LOG("Client received error %d", errorCode);
}

void net::Photon::warningReturn(int warningCode)
{
	NET_LOG("Client received warning %d", warningCode);
}

void net::Photon::serverErrorReturn(int errorCode)
{
	EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"code: %d", errorCode);
	// mpOutputListener->writeLine(ExitGames::Common::JString(L"received error ") + errorCode + " from server");
}

void net::Photon::joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& /*playernrs*/, const ExitGames::LoadBalancing::Player& player)
{
	NET_LOG("%ls joined the game", player.getName().cstr());
}

void net::Photon::leaveRoomEventAction(int playerNr, bool isInactive)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	// mpOutputListener->writeLine(L"");
	// mpOutputListener->writeLine(ExitGames::Common::JString(L"player ") + playerNr + L" has left the game");
}

void net::Photon::customEventAction(int /*playerNr*/, nByte /*eventCode*/, const ExitGames::Common::Object& eventContent)
{
	// you do not receive your own events, unless you specify yourself as one of the receivers explicitly, so you must start 2 clients, to receive the events, which you have sent, as sendEvent() uses the default receivers of opRaiseEvent() (all players in same room like the sender, except the sender itself)
	EGLOG(ExitGames::Common::DebugLevel::ALL, L"");
	// mpOutputListener->write(ExitGames::Common::JString(L"r") + ExitGames::Common::ValueObject<long long>(eventContent).getDataCopy() + L" ");
#if defined _EG_EMSCRIPTEN_PLATFORM
	mpOutputListener->writeLine(L"");
#endif
}

void net::Photon::connectReturn(int errorCode, const ExitGames::Common::JString& errorString)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	if (errorCode)
	{
		EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		mStateAccessor.setState(STATE_DISCONNECTING);
		return;
	}
	// mpOutputListener->writeLine(L"connected");
	mStateAccessor.setState(STATE_CONNECTED);
	if (mAutoJoinRoom)
		opJoinOrCreateRoom();
}

void net::Photon::disconnectReturn(void)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	// mpOutputListener->writeLine(L"disconnected");
	mStateAccessor.setState(STATE_DISCONNECTED);
}

void net::Photon::createRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& /*gameProperties*/, const ExitGames::Common::Hashtable& /*playerProperties*/, int errorCode, const ExitGames::Common::JString& errorString)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	if (errorCode)
	{
		EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		// mpOutputListener->writeLine(L"opCreateRoom() failed: " + errorString);
		mStateAccessor.setState(STATE_CONNECTED);
		return;
	}
	mLastJoinedRoom = myLoadBalancingClient.getCurrentlyJoinedRoom().getName();
	mLastPlayerNr = localPlayerNr;

	EGLOG(ExitGames::Common::DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);
	// mpOutputListener->writeLine(L"... room " + myLoadBalancingClient.getCurrentlyJoinedRoom().getName() + " has been created");
	// mpOutputListener->writeLine(L"regularly sending dummy events now");
	mStateAccessor.setState(STATE_JOINED);
}

void net::Photon::joinOrCreateRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& /*gameProperties*/, const ExitGames::Common::Hashtable& /*playerProperties*/, int errorCode, const ExitGames::Common::JString& errorString)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	if (errorCode)
	{
		EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		// mpOutputListener->writeLine(L"opJoinOrCreateRoom() failed: " + errorString);
		mStateAccessor.setState(STATE_CONNECTED);
		return;
	}
	mLastJoinedRoom = myLoadBalancingClient.getCurrentlyJoinedRoom().getName();
	mLastPlayerNr = localPlayerNr;

	EGLOG(ExitGames::Common::DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);
	// mpOutputListener->writeLine(L"... room " + myLoadBalancingClient.getCurrentlyJoinedRoom().getName() + " has been entered");
	// mpOutputListener->writeLine(L"regularly sending dummy events now");
	mStateAccessor.setState(STATE_JOINED);
}

void net::Photon::joinRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& /*gameProperties*/, const ExitGames::Common::Hashtable& /*playerProperties*/, int errorCode, const ExitGames::Common::JString& errorString)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	if (errorCode)
	{
		mLastJoinedRoom = "";
		mLastPlayerNr = 0;
		EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		// mpOutputListener->writeLine(L"opJoinRoom() failed: " + errorString);
		mStateAccessor.setState(STATE_CONNECTED);
		return;
	}
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);
	// mpOutputListener->writeLine(L"... room " + myLoadBalancingClient.getCurrentlyJoinedRoom().getName() + " has been successfully joined");
	// mpOutputListener->writeLine(L"regularly sending dummy events now");
	mStateAccessor.setState(STATE_JOINED);
}

void net::Photon::joinRandomRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& /*gameProperties*/, const ExitGames::Common::Hashtable& /*playerProperties*/, int errorCode, const ExitGames::Common::JString& errorString)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	if (errorCode)
	{
		EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		// mpOutputListener->writeLine(L"opJoinRandomRoom() failed: " + errorString);
		mStateAccessor.setState(STATE_CONNECTED);
		return;
	}

	mLastJoinedRoom = myLoadBalancingClient.getCurrentlyJoinedRoom().getName();
	mLastPlayerNr = localPlayerNr;

	EGLOG(ExitGames::Common::DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);
	// mpOutputListener->writeLine(L"... room " + myLoadBalancingClient.getCurrentlyJoinedRoom().getName() + " has been successfully joined");
	// mpOutputListener->writeLine(L"regularly sending dummy events now");
	mStateAccessor.setState(STATE_JOINED);
}

void net::Photon::leaveRoomReturn(int errorCode, const ExitGames::Common::JString& errorString)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	if (errorCode)
	{
		EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		// mpOutputListener->writeLine(L"opLeaveRoom() failed: " + errorString);
		mStateAccessor.setState(STATE_DISCONNECTING);
		return;
	}
	mStateAccessor.setState(STATE_LEFT);
	// mpOutputListener->writeLine(L"room has been successfully left");
}

void net::Photon::joinLobbyReturn(void)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	// mpOutputListener->writeLine(L"joined lobby");
}

void net::Photon::leaveLobbyReturn(void)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	// mpOutputListener->writeLine(L"left lobby");
}

void net::Photon::onLobbyStatsResponse(const ExitGames::Common::JVector<ExitGames::LoadBalancing::LobbyStatsResponse>& lobbyStats)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"%ls", lobbyStats.toString().cstr());
	// mpOutputListener->writeLine(L"LobbyStats: " + lobbyStats.toString());
}

void net::Photon::onLobbyStatsUpdate(const ExitGames::Common::JVector<ExitGames::LoadBalancing::LobbyStatsResponse>& lobbyStats)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"%ls", lobbyStats.toString().cstr());
	// mpOutputListener->writeLine(L"LobbyStats: " + lobbyStats.toString());
}

void net::Photon::onAvailableRegions(const ExitGames::Common::JVector<ExitGames::Common::JString>& availableRegions, const ExitGames::Common::JVector<ExitGames::Common::JString>& availableRegionServers)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"%ls / %ls", availableRegions.toString().cstr(), availableRegionServers.toString().cstr());
	// mpOutputListener->writeLine(L"onAvailableRegions: " + availableRegions.toString() + L" / " + availableRegionServers.toString());
	// select first region from list
	// mpOutputListener->writeLine(L"selecting region: " + availableRegions[0]);
	myLoadBalancingClient.selectRegion(availableRegions[0]);
}