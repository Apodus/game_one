/* Exit Games Photon LoadBalancing - C++ Client Lib
 * Copyright (C) 2004-2016 by Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */
#include "NetPch.h"
#include "LoadBalancing-cpp/inc/RaiseEventOptions.h"

/** @file LoadBalancing-cpp/inc/RaiseEventOptions.h */

namespace ExitGames
{
	using namespace Common;

	namespace LoadBalancing
	{
		RaiseEventOptions::RaiseEventOptions(nByte channelID, nByte eventCaching, const int* targetPlayers, short numTargetPlayers, nByte receiverGroup, nByte interestGroup, bool forwardToWebhook, int cacheSliceIndex)
			: mChannelID(channelID)
			, mEventCaching(eventCaching)
			, mTargetPlayers(targetPlayers)
			, mNumTargetPlayers(numTargetPlayers)
			, mReceiverGroup(receiverGroup)
			, mInterestGroup(interestGroup)
			, mForwardToWebhook(forwardToWebhook)
			, mCacheSliceIndex(cacheSliceIndex)
		{
		}

		RaiseEventOptions::~RaiseEventOptions(void)
		{
		};

		RaiseEventOptions::RaiseEventOptions(const RaiseEventOptions& toCopy)
		{
			*this = toCopy;
		}

		RaiseEventOptions& RaiseEventOptions::operator=(const RaiseEventOptions& toCopy)
		{
			mChannelID = toCopy.mChannelID;
			mEventCaching = toCopy.mEventCaching;
			mTargetPlayers = toCopy.mTargetPlayers;
			mNumTargetPlayers = toCopy.mNumTargetPlayers;
			mReceiverGroup = toCopy.mReceiverGroup;
			mInterestGroup = toCopy.mInterestGroup;
			mForwardToWebhook = toCopy.mForwardToWebhook;
			mCacheSliceIndex = toCopy.mCacheSliceIndex;

			return *this;
		}

		nByte RaiseEventOptions::getChannelID(void) const
		{
			return mChannelID;
		}

		RaiseEventOptions& RaiseEventOptions::setChannelID(nByte channelID)
		{
			mChannelID = channelID;
			return *this;
		}

		nByte RaiseEventOptions::getEventCaching(void) const
		{
			return mEventCaching;
		}

		RaiseEventOptions& RaiseEventOptions::setEventCaching(nByte eventCaching)
		{
			mEventCaching = eventCaching;
			return *this;
		}

		const int* RaiseEventOptions::getTargetPlayers(void) const
		{
			return mTargetPlayers;
		}

		RaiseEventOptions& RaiseEventOptions::setTargetPlayers(const int* targetPlayers)
		{
			mTargetPlayers = targetPlayers;
			return *this;
		}

		short RaiseEventOptions::getNumTargetPlayers(void) const
		{
			return mNumTargetPlayers;
		}

		RaiseEventOptions& RaiseEventOptions::setNumTargetPlayers(short numTargetPlayers)
		{
			mNumTargetPlayers = numTargetPlayers;
			return *this;
		}

		nByte RaiseEventOptions::getReceiverGroup(void) const
		{
			return mReceiverGroup;
		}

		RaiseEventOptions& RaiseEventOptions::setReceiverGroup(nByte receiverGroup)
		{
			mReceiverGroup = receiverGroup;
			return *this;
		}

		nByte RaiseEventOptions::getInterestGroup(void) const
		{
			return mInterestGroup;
		}

		RaiseEventOptions& RaiseEventOptions::setInterestGroup(nByte interestGroup)
		{
			mInterestGroup = interestGroup;
			return *this;
		}

		bool RaiseEventOptions::getForwardToWebhook(void) const
		{
			return mForwardToWebhook;
		}

		RaiseEventOptions& RaiseEventOptions::setForwardToWebhook(bool forwardToWebhook)
		{
			mForwardToWebhook = forwardToWebhook;
			return *this;
		}

		int RaiseEventOptions::getCacheSliceIndex(void) const 
		{
			return mCacheSliceIndex;
		}

		RaiseEventOptions& RaiseEventOptions::setCacheSliceIndex(int cacheSliceIndex)
		{
			mCacheSliceIndex = cacheSliceIndex;
			return *this;
		}

		JString& RaiseEventOptions::toString(JString& retStr, bool withTypes) const
		{
			return retStr += JString(L"{channelID=") + getChannelID() + L", eventCaching=" + getEventCaching() + L", targetPlayers=" + JVector<int>(getTargetPlayers(), static_cast<unsigned int>(getNumTargetPlayers())).toString() + L", receiverGroup=" + getReceiverGroup()+ L", interestGroup" + getInterestGroup()+ L", forwardToWebhook=" + getForwardToWebhook() + L", cacheSliceIndex" + getCacheSliceIndex() + L"}";
		}
	}
}