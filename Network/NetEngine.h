#pragma once

#include <vector>

namespace net
{

	class DataProxy;
	class Photon;

	class Engine
	{
	public:
		Engine();
		~Engine();

		void PreTick();
		void PostTick(net::DataProxy& proxy);

	private:
		std::unique_ptr<net::Photon> myPhoton;

	};
}