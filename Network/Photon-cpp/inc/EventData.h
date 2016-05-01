/* Exit Games Photon - C++ Client Lib
* Copyright (C) 2004-2016 by Exit Games GmbH. All rights reserved.
* http://www.photonengine.com
* mailto:developer@photonengine.com
*/

#pragma once

#include "Common-cpp/inc/Common.h"

namespace ExitGames
{
	namespace Photon
	{
		namespace Internal
		{
			class PeerBase;
		}

		class EventData
		{
		public:
			~EventData(void);

			EventData(const EventData& toCopy);
			EventData& operator=(const EventData& toCopy);


			const Common::Object& operator[](unsigned int index) const;

			Common::JString toString(bool withParameters=false, bool withParameterTypes=false) const;
			Common::Object getParameterForCode(nByte parameterCode) const;

			nByte getCode(void) const;
			const Common::Hashtable& getParameters(void) const;
		private:
			friend class Internal::PeerBase;

			EventData(nByte code, const Common::Hashtable& params);

			nByte mCode;
			Common::Hashtable mParameters;
		};
	}
}