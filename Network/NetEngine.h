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

		// TODO: Add message Id -> Proxy mapping
		void SetProxy(net::DataProxy& proxy);

		void PreTick();
		void PostTick();

	private:
		std::unique_ptr<net::Photon> myPhoton;

	};
}