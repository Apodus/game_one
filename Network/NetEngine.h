#pragma once

#include <vector>

namespace net
{

	class DataAdapter;
	class Photon;

	class Engine
	{
	public:
		Engine();
		~Engine();

		void AddAdapter(uint8_t eventId, uint8_t keyId, net::DataAdapter& adapter);

		void PreTick();
		void PostTick();

	private:
		std::unique_ptr<net::Photon> myPhoton;

	};
}