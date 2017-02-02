#pragma once

namespace sa { class Graphics; }
namespace bs { class BattleSimAsync; }

#include "Combat.hpp"
#include <chrono>
#include <memory>

#include "BattleSim/BattleSimAsync.h"


class CombatView
{
public:
	void draw(std::shared_ptr<sa::Graphics> pGraphics);

	void tick(long long timeMs);

	void start();
	void stop();

	void setCombat(Combat& combat);

private:
	void draw(std::shared_ptr<sa::Graphics> pGraphics, long long deltaTime);

	std::unique_ptr<bs::BattleSimAsync> m_sim;
	uint64_t m_lastSimUpdate;
	std::chrono::time_point<std::chrono::steady_clock> m_renderTime;
	double m_simAccu;

	Combat m_combat;
	bs::Battle m_battle;


	struct Unit
	{
		struct Status
		{
			Status() : isValid(false) {}
			float x;
			float y;
			float z;
			bool isValid;
		};
		Status current;
		Status next;
		float size;
		int hitpoints;
		int team;
	};
	std::vector<Unit> m_units;

};