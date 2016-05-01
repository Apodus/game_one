/* Exit Games Photon - C++ Client Lib
 * Copyright (C) 2004-2016 by Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#pragma once

#include "Photon-cpp/inc/PhotonListener.h"
#include "Photon-cpp/inc/OperationRequest.h"
#include "Photon-cpp/inc/TrafficStats.h"
#include "Photon-cpp/inc/TrafficStatsGameLevel.h"
#include "Photon-cpp/inc/Enums/ConnectionProtocol.h"
#include "Photon-cpp/inc/Enums/ErrorCode.h"
#include "Photon-cpp/inc/Enums/NetworkPort.h"
#include "Photon-cpp/inc/Enums/PeerState.h"
#include "Photon-cpp/inc/Enums/StatusCode.h"

class EGObjCPhotonPeer;

namespace ExitGames
{
	namespace Photon
	{
		namespace Internal
		{
			class EnetPeer;
			class TPeer;
		}

		class PhotonPeer
		{
		private:
			static const unsigned int MAX_APP_ID_LENGTH = 32;
		public:
			PhotonPeer(PhotonListener& listener, nByte connectionProtocol=ConnectionProtocol::DEFAULT);
			virtual ~PhotonPeer(void);

			virtual bool connect(const Common::JString& ipAddr, const Common::JString& appID=Common::JString());
			virtual void disconnect(void);
			virtual void service(bool dispatchIncomingCommands=true);
			virtual void serviceBasic(void);
			virtual bool opCustom(const OperationRequest& operationRequest, bool sendReliable, nByte channelID=0, bool encrypt=false);
			virtual bool sendOutgoingCommands(void);
			virtual bool sendAcksOnly(void);
			virtual bool dispatchIncomingCommands(void);
			virtual bool establishEncryption(void);
			virtual void fetchServerTimestamp(void);
			virtual void resetTrafficStats(void);
			virtual void resetTrafficStatsMaximumCounters(void);
			virtual Common::JString vitalStatsToString(bool all) const;

			const PhotonListener* getListener(void) const;
			int getServerTimeOffset(void) const;
			int getServerTime(void) const;
			int getBytesOut(void) const;
			int getBytesIn(void) const;
			int getByteCountCurrentDispatch(void) const;
			int getByteCountLastOperation(void) const;
			int getPeerState(void) const;
			int getSentCountAllowance(void) const;
			void setSentCountAllowance(int setSentCountAllowance);
			int getTimePingInterval(void) const;
			void setTimePingInterval(int setTimePingInterval);
			int getRoundTripTime(void) const;
			int getRoundTripTimeVariance(void) const;
			int getTimestampOfLastSocketReceive(void) const;
			int getDebugOutputLevel(void) const;
			bool setDebugOutputLevel(int debugLevel);
			int getIncomingReliableCommandsCount(void) const;
			short getPeerId(void) const;
			int getDisconnectTimeout(void) const;
			void setDisconnectTimeout(int disconnectTimeout);
			int getQueuedIncomingCommands(void) const;
			int getQueuedOutgoingCommands(void) const;
			Common::JString getServerAddress(void) const;
			bool getIsEncryptionAvailable(void) const;
			int getResentReliableCommands(void) const;
			int getLimitOfUnreliableCommands(void) const;
			void setLimitOfUnreliableCommands(int value);
			bool getCrcEnabled(void) const;
			void setCrcEnabled(bool crcEnabled);
			int getPacketLossByCrc(void) const;
			bool getTrafficStatsEnabled(void) const;
			void setTrafficStatsEnabled(bool trafficStasEnabled);
			int getTrafficStatsElapsedMs(void) const;
			const TrafficStats& getTrafficStatsIncoming(void) const;
			const TrafficStats& getTrafficStatsOutgoing(void) const;
			const TrafficStatsGameLevel& getTrafficStatsGameLevel(void) const;
			nByte getQuickResendAttempts(void);
			void setQuickResendAttempts(nByte quickResendAttempts);

			static short getPeerCount(void);
			static unsigned int getMaxAppIDLength(void);
		protected:
			PhotonPeer(PhotonListener& listener, bool usingObjC, nByte connectionProtocol=ConnectionProtocol::DEFAULT);

			Common::Logger mLogger;
		private:
			void init(PhotonListener& listener, bool usingObjC, nByte connectionProtocol=ConnectionProtocol::DEFAULT);

			Internal::PeerBase* mpPhotonPeer;
#ifdef _EG_EMSCRIPTEN_PLATFORM
			const nByte M_CONNECTION_PROTOCOL;
#endif

			friend class Internal::PeerBase;
			friend class Internal::EnetPeer;
			friend class Internal::TPeer;
			friend class ::EGObjCPhotonPeer;
		};
	}
}