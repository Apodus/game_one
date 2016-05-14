#include "NetPch.h"
#include "NetPhoton.h"
#include "NetLogging.h"
#include "NetDataAdapter.h"
#include "NetOutputStream.h"
#include "NetInputStream.h"
#include "NetSession.h"

#include <vector>

static const ExitGames::Common::JString appId = L"357c978b-3bfc-4f60-a2f0-143560e2bba8"; // set your app id here
static const ExitGames::Common::JString appVersion = L"1.0";

static const bool autoLobbbyStats = true;
static const bool useDefaultRegion = false;

// How often send outgoing data
// Minimum period should be less than logic tick period to allow always scheduling after logic update.
static const float MinUpdatePeriod = 0.015f; // seconds
static const float MaxUpdatePeriod = 0.2f; // seconds

using namespace ExitGames::Common;

net::Photon::Photon(Session& session)
	: 
	mySession(session),
	myLoadBalancingClient(*this, appId, appVersion, ExitGames::Photon::ConnectionProtocol::DEFAULT, autoLobbbyStats, useDefaultRegion),
	myLastUpdateSentTime(std::chrono::high_resolution_clock::now())
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

void net::Photon::Update(void)
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
	LogMeasurements();
}

bool net::Photon::IsSendScheduled() const
{
	auto now = std::chrono::high_resolution_clock::now();
	const std::chrono::duration<double> duration = now - myLastUpdateSentTime;
	return (duration.count() >= MinUpdatePeriod);
}

void net::Photon::Receive()
{
	myLoadBalancingClient.service(false);
}

void net::Photon::Send()
{
	const size_t OverheadBytesPerUpdateApprox = 40;

	auto now = std::chrono::high_resolution_clock::now();
	const std::chrono::duration<double> duration = now - myLastUpdateSentTime;

#if 0
	NET_LOG("In: %d Out: %d  Rtt: %d byteCount:%d lastOp:%d state:%d delta:%f",
		myLoadBalancingClient.getBytesIn(), myLoadBalancingClient.getBytesOut(),
		myLoadBalancingClient.getRoundTripTime(), myLoadBalancingClient.getByteCountCurrentDispatch(), myLoadBalancingClient.getByteCountLastOperation(),
		mStateAccessor.getState(), duration.count());
#endif

	size_t totalPayload = 0;
	if (mStateAccessor.getState() == STATE_JOINED)
	{
		bool hasDataLeft;
		do
		{
			hasDataLeft = false;
			for (size_t i = 0; i < myAdapterGroups.size(); i++)
			{
				net::DataAdapter::OutputStream outStream(64);
				ExitGames::Common::Hashtable table;
				for (size_t j = 0; j < myAdapterGroups[i].adapters.size(); j++)
				{
					auto& adapter = myAdapterGroups[i].adapters[j];
					if (!adapter.instance->Serialize(outStream))
					{
						assert(!outStream.CanConfigure());
						hasDataLeft = true;
					}
					int dataLen = static_cast<int>(outStream.GetSize());
					if (dataLen > 0)
					{
						table.put(adapter.keyId, outStream.GetData(), dataLen);
						totalPayload += dataLen + 1 /* KeyId */;
						outStream.Clear();
					}
				}

				if (totalPayload > 0)
				{
					// TODO: Process receiver list
					myLoadBalancingClient.opRaiseEvent(outStream.IsReliable(), table, myAdapterGroups[i].groupId);
					myPayloadBytesOut += totalPayload + 1 /*Event code*/;
				}
			}
		} while (hasDataLeft);
	}

	if (totalPayload > 0 || duration.count() > MaxUpdatePeriod)
	{
		myLoadBalancingClient.service();
		myLastUpdateSentTime = now;
		myUpdateSendCount++;
	}
}

void net::Photon::LogMeasurements()
{
#if 0
	auto now = std::chrono::high_resolution_clock::now();
	const double MeasurementPeriod[3] = { 0.50, 1.00, 3.0 };
	for (int i = 0; i < 3; i++)
	{
		const std::chrono::duration<double> duration = now - myMeasurements[i].myUpdateTime;
		if (duration.count() >= MeasurementPeriod[i])
		{
			myMeasurements[i].myUpdateTime = now;
			auto bytesInNow = myLoadBalancingClient.getBytesIn();
			auto bytesOutNow = myLoadBalancingClient.getBytesOut();

			double measuredOut = static_cast<double>(static_cast<uint32_t>(
				bytesOutNow - myMeasurements[i].myMeasuredBytesOut)) / duration.count() / 1000.0;
			double payloadOut = static_cast<double>(static_cast<uint32_t>(
				myPayloadBytesOut - myMeasurements[i].myPayloadBytesOut)) / duration.count() / 1000.0;
			double updatesPerSec = static_cast<double>(static_cast<uint32_t>(
				myUpdateSendCount - myMeasurements[i].myUpdateSentCount)) / duration.count();

			uint32_t overhead = static_cast<uint32_t>((measuredOut - payloadOut) / updatesPerSec * 1000.0);

			NET_LOG("In: %f KB/s Out: %f KB/s PaylodOut: %f KB/s %f Updates/s Overhead %d bytes (%fs period)",
				static_cast<double>(static_cast<uint32_t>(
				bytesInNow - myMeasurements[i].myMeasuredBytesIn)) / duration.count() / 1000.0,
				measuredOut,
				payloadOut,
				updatesPerSec,
				overhead,
				duration.count());

			myMeasurements[i].myMeasuredBytesIn = bytesInNow;
			myMeasurements[i].myMeasuredBytesOut = bytesOutNow;
			myMeasurements[i].myPayloadBytesOut = myPayloadBytesOut;
			myMeasurements[i].myUpdateSentCount = myUpdateSendCount;

			// NET_LOG("Time %d", myLoadBalancingClient.getServerTime());
			// myLoadBalancingClient.fetchServerTimestamp();
		}
	}
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

void net::Photon::joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& playernrs, const ExitGames::LoadBalancing::Player& player)
{
	NET_LOG("%ls(%d) joined the game (playernrs:%d) (%d players in game %d online)", 
		player.getName().cstr(),
		playerNr,
		playernrs.getSize(),
		myLoadBalancingClient.getCountPlayersIngame(), 
		myLoadBalancingClient.getCountPlayersOnline());
	mySession.AddMember(playerNr);
}

void net::Photon::leaveRoomEventAction(int playerNr, bool isInactive)
{
	NET_LOG("Player %d left the game", playerNr);
	mySession.RemoveMember(playerNr);	
}

net::DataAdapter* net::Photon::FindAdapter(uint8_t eventId, uint8_t keyId)
{
	for (size_t i = 0; i < myAdapterGroups.size(); i++)
	{
		if (myAdapterGroups[i].groupId == eventId)
		{
			for (size_t j = 0; j < myAdapterGroups[i].adapters.size(); j++)
			{
				if (myAdapterGroups[i].adapters[j].keyId == keyId)
				{
					return myAdapterGroups[i].adapters[j].instance;
				}
			}
		}
	}
	return nullptr;
}

void net::Photon::customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent)
{
	const auto hash = ExitGames::Common::ValueObject<ExitGames::Common::Hashtable>(eventContent);
	if (hash.getDataAddress() != nullptr)
	{
		const auto evData = *hash.getDataAddress();
		for (size_t i = 0; i < evData.getKeys().getSize(); i++)
		{
			const auto keyId = ExitGames::Common::ValueObject<nByte>(evData.getKeys().getElementAt(i)).getDataCopy();
			const auto obj = evData.getValue(keyId);
			if (obj && obj->getDimensions() == 1)
			{
				const auto size = obj->getSizes()[0];
				if (size > 0)
				{
 					const nByte* data = *((ValueObject<nByte*>*)obj)->getDataAddress();
#if 0
					NET_LOG("Data player %d->%d code %dKey=%d Size=%d", playerNr, 
						myLoadBalancingClient.getLocalPlayer().getNumber(), eventCode, keyId, size);
#endif
					DataAdapter* adapter = FindAdapter(eventCode, keyId);
					if (adapter)
					{
						InputStream stream(data, size);
						adapter->Deserialize(stream);
					}
				}
				else
				{
					NET_LOG("Zero data in key %d", keyId);
				}
			}
			else
			{
				NET_LOG("Invalid data in key %d", keyId);
			}
		}
	}
	else
	{
		NET_LOG("Received invalid event content");
	}
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
	NET_LOG("Lobby stats: %s", lobbyStats.toString().cstr());
}

void net::Photon::onAvailableRegions(const ExitGames::Common::JVector<ExitGames::Common::JString>& availableRegions, const ExitGames::Common::JVector<ExitGames::Common::JString>& availableRegionServers)
{
	NET_LOG("Available regions: %s / %s", 
		availableRegions.toString().cstr(), availableRegionServers.toString().cstr());
	// select first region from list
	myLoadBalancingClient.selectRegion(availableRegions[0]);
}