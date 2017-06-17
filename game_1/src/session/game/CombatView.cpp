#define NOMINMAX

#include "CombatView.hpp"

#include "graphics/graphics.hpp"
#include "BattleSim/BattleSimAsync.h"

#include <algorithm>

CombatView::CombatView()
{

}

CombatView::~CombatView()
{

}

void CombatView::draw(std::shared_ptr<sa::Graphics> pGraphics)
{
	// TODO: Need to provide delta time for draw method too! 
	// After that this can be removed:
	auto renderTime = std::chrono::high_resolution_clock::now();
	using ms = std::chrono::duration<float, std::milli>;
	auto deltaTime = static_cast<long long>(std::chrono::duration_cast<ms>(renderTime - m_renderTime).count());
	m_renderTime = renderTime;
	draw(pGraphics, deltaTime);
}

void CombatView::draw(std::shared_ptr<sa::Graphics> pGraphics, long long deltaTime)
{
	sa::TextureHandler::getSingleton().bindTexture(0, "Empty");

	const float offsetX = 30.0f;
	const float offsetY = 20.0f;
	const float offsetZ = 0.0f;
	const float scale = 1 / 4.0f;

	float unitHeight = 0.1f;

	m_simAccu += static_cast<double>(deltaTime) / 1000.0;
	while (m_simAccu >= m_sim->GetTimePerUpdate())
	{
		const auto* frame = m_sim->GetField().GetFrame();
		if (frame)
		{
			for (size_t i = 0; i < m_units.size(); i++)
			{
				m_units[i].current = m_units[i].next;
			}
			auto reader = frame->GetReader();

			// Starting units
			uint16_t numStartingUnits = reader.Read<uint16_t>();
			for (size_t i = 0; i < numStartingUnits; i++)
			{
				const auto& unitIn = reader.Read<bs::Visualization::Addition>();
				if (unitIn.id >= m_units.size())
				{
					m_units.resize(unitIn.id + 1);
				}
				auto& unit = m_units[unitIn.id];
				unit.team = unitIn.team;
				unit.size = unitIn.radius.toFloat() * scale;
			}

			// Moving units
			uint16_t numUpdates = reader.Read<uint16_t>();
			for (size_t i = 0; i < numUpdates; i++)
			{
				const auto& unitIn = reader.Read<bs::Visualization::Movement>();
				auto& unit = m_units[unitIn.id];
				unit.next.isValid = true;
				unit.next.angle = unitIn.angle.toFloat();
				unit.next.x = unitIn.pos.x.toFloat() * scale - offsetX;
				unit.next.y = unitIn.pos.y.toFloat() * scale - offsetY;
				unit.next.z = unitIn.pos.z.toFloat() * scale - offsetZ;
				unit.hitpoints = unitIn.hitpoints;
			}

			// Stopping units
			uint16_t numStopping = reader.Read<uint16_t>();
			for (size_t i = 0; i < numStopping; i++)
			{
				const auto& unitIn = reader.Read<bs::Visualization::Removal>();
				auto& unit = m_units[unitIn.id];
				unit.next.isValid = false;
			}

			m_sim->GetField().FreeFrame();
			m_simAccu -= m_sim->GetTimePerUpdate();
		}
		else
		{
			if (!m_sim->IsComplete())
			{
				LOG("Sim stalled;fraction=%f", static_cast<float>(m_simAccu / m_sim->GetTimePerUpdate()));
			}
			break;
		}
	}

	float frameFraction = std::min(1.0f, static_cast<float>(m_simAccu / m_sim->GetTimePerUpdate()));

	sa::vec3<float> minPos;
	sa::vec3<float> maxPos;
	bool found = false;
	for (size_t i = 0; i < m_units.size(); i++)
	{
		auto& unit = m_units[i];
		sa::Matrix4 model;
		sa::vec3<float> pos;
		float angle;
		if (unit.next.isValid)
		{
			if (unit.current.isValid)
			{
				pos.x = unit.current.x + ((unit.next.x - unit.current.x) * frameFraction);
				pos.y = unit.current.y + ((unit.next.y - unit.current.y) * frameFraction);
				pos.z = unit.current.z + ((unit.next.z - unit.current.z) * frameFraction);
				angle = unit.current.angle;
			}
			else
			{
				continue; // Not yet visible
			}
		}
		else if (frameFraction <= 0 && unit.current.isValid)
		{
			pos.x = unit.current.x;
			pos.y = unit.current.y;
			pos.z = unit.current.z;
			angle = unit.current.angle;
		}
		else
		{
			// Not visible anymore, remove from list
			continue;
		}

		if (!found)
		{
			minPos = pos;
			maxPos = pos;
			found = true;
		}
		else
		{
			minPos.x = std::min(minPos.x, pos.x);
			minPos.y = std::min(minPos.y, pos.y);
			minPos.z = std::min(minPos.z, pos.z);
			maxPos.x = std::max(maxPos.x, pos.x);
			maxPos.y = std::max(maxPos.y, pos.y);
			maxPos.z = std::max(maxPos.z, pos.z);
		}			

		model.makeTranslationMatrix(pos.x, pos.y, pos.z + (unit.hitpoints == 0 ? 0.0f : unitHeight * scale));
		model.rotate(-angle * 0.0174533f, 0, 0, 1);
		model.scale(unit.size, unit.size, 1);

		pGraphics->m_pRenderer->drawRectangle(model, "Hero",
			unit.hitpoints == 0 ? Color::RED :
			(unit.team == 1 ? Color::GREEN : Color::BLUE));
	}

	if (found)
	{
		m_directorCameraPosition = (minPos + maxPos) / 2;
		m_directorCameraPosition.z = 20;
		if (!m_directorReady)
		{
			m_directorReady = true;
			m_camera.pos = getDirectorTarget();
			m_camera.target = m_camera.pos;
		}
	}
}

void CombatView::tick(long long timeMs)
{
	if (m_sim)
	{
		auto delta = m_lastSimUpdate != 0 ? timeMs - m_lastSimUpdate : 0;
		m_sim->Simulate(static_cast<size_t>(delta));
		m_lastSimUpdate = timeMs;
	}

	if (isDirectorReady())
	{
		// This is bad, it does not take frame skipping into account?
		m_camera.target = m_camera.target * 0.99 + getDirectorTarget() * 0.01;
	}
}

void CombatView::start()
{
	m_directorReady = false;
	m_renderTime = std::chrono::high_resolution_clock::now();
	m_battle = bs::Battle();
	m_combat.setup(m_battle);
	if (m_battle.NumUnits() == 0)
	{
		m_battle = bs::BattleSim::Generate();
	}

	m_sim = std::make_unique<bs::BattleSimAsync>(m_battle);
	const size_t SimForwardMillis = 1000;
	m_sim->Resolve(SimForwardMillis);
	m_lastSimUpdate = 0;
	m_simAccu = 0;
	m_units.clear();
}

void CombatView::stop()
{
	m_sim = nullptr;
}

void CombatView::setCombat(Combat& combat)
{
	m_combat = combat;
}