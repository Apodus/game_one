#pragma once

#include "Combat.hpp"
#include <chrono>
#include <memory>

namespace sa { class Graphics; }
namespace bs { class BattleSimAsync; }

class CombatView
{
public:
	CombatView();
	~CombatView();

	void draw(std::shared_ptr<sa::Graphics> pGraphics);

	void tick(long long timeMs);

	void start();
	void stop();

	void setCombat(Combat& combat);

	const sa::vec3<float> getDirectorTarget() const { return m_directorCameraPosition; }

	bool isDirectorReady() const { return m_directorReady; }

private:
	void draw(std::shared_ptr<sa::Graphics> pGraphics, long long deltaTime);

	sa::vec3<float> m_directorCameraPosition;

	std::unique_ptr<bs::BattleSimAsync> m_sim;
	uint64_t m_lastSimUpdate;
	std::chrono::time_point<std::chrono::steady_clock> m_renderTime;
	double m_simAccu;
	bool m_directorReady;

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