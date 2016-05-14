#pragma once

#include <vector>
#include "NetSession.h"

namespace net
{

	class DataAdapter;
	class Photon;

	class Engine
	{
		friend class Session;
	public:
		Engine();
		~Engine();

		void PreTick();

		void PostTick();

	private:
		void AddSession(Session* aSession);

		void RemoveSession(Session* aSession);

		std::vector<Session*> mySessions;
	};
}