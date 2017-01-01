#pragma once


#ifdef BATTLESIM_DLL
#define BATTLESIM_API __declspec(dllimport)
#else
#define BATTLESIM_API __declspec(dllexport)
#endif

namespace bs
{
	class BattleSim
	{
	public:
		BATTLESIM_API BattleSim();

		void BATTLESIM_API Tick();

	private:
	};
}