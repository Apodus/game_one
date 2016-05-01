#pragma once

namespace net
{
	class Photon;

	class Engine
	{
	public:
		Engine();
		~Engine();

		void Tick();

	private:
		std::unique_ptr<net::Photon> myPhoton;
	};
}