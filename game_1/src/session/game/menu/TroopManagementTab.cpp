
#include "session/game/menu/TroopManagementTab.hpp"
#include "session/game/menu/CommanderTab.hpp"

CommanderEntry::CommanderEntry(
	TroopsTab* parent,
	sa::MenuComponent* alignUnder
)
	: sa::MenuComponent(
		parent,
		"CommanderEntry",
		[alignUnder]() { return alignUnder->getExteriorPosition(BOTTOM); },
		sa::vec3<float>(1.0f, 0.1f, 0)
	)
	, bg(this, "bg", "Frame")
{
	positionAlign = TOP;
	bg.update(0);
}


void CommanderEntry::draw(std::shared_ptr<sa::Graphics> graphics) const
{
	bg.visualise(graphics);
	// 1. draw commander icon to top left corner
	// 2. draw list of squads to left side (visualise squad members towards the right)
	// 
	// TODO: List of CommanderEntries should scroll + out of bounds entries should alpha blend.
}

void CommanderEntry::update(float dt)
{
	// TODO: Editing squads?? How?
}

TroopsTab::TroopsTab(
	sa::MenuComponent* parent,
	ProvinceGraph::Province& province,
	ProvinceCommandersTab& commandersTab
)
	: sa::MenuComponent(
			parent,
			"TroopsTab",
			[this, parent]() {
				return parent->getExteriorPosition(RIGHT | TOP);
			},
			[this, parent]() {
				return sa::vec3<float>(1.0f, 0.9f, 0);
			}
		)
	, bg(this, "bg", "Frame")
	, m_commandersTab(commandersTab)
	, m_province(province)
{
	positionAlign = LEFT | TOP;
	setPositionUpdateType(true);
	bg.update(0);

	m_alpha = 0;
	m_targetAlpha = 0;
}

sa::vec3<float> TroopsTab::startPosToUI(sa::vec2<int> posInt) const {
	auto areaScale = battleAreaScale();
	return sa::vec3<float>(
		areaScale.x * (posInt.x / 100.0f),
		areaScale.y * (posInt.y / 100.0f),
		0
	);
}

sa::vec2<int> TroopsTab::uiPosToStart(sa::vec3<float> posUI) const {
	auto areaScale = battleAreaScale();
	return sa::vec2<int>(
		static_cast<int>(100.0f * posUI.x / areaScale.x),
		static_cast<int>(100.0f * posUI.y / areaScale.y)
	);
}

CommanderOrSquad TroopsTab::findNearest(float x, float y) const {
	float bestValue = 1000000;
	CommanderOrSquad bestItem;

	sa::vec3<float> requestedPos(x, y, 0);
	for (size_t i = 0; i < m_province.commanders.size(); ++i)
	{
		const auto& commander = m_province.commanders[i];
		auto pos = uiPosOf(commander.combatOrder.startPos);
		float lengthSquared = (requestedPos - pos).lengthSquared();

		if (lengthSquared < bestValue)
		{
			bestValue = lengthSquared;
			bestItem.commanderIndex = commander.id;
			bestItem.squadIndex = -1;
			bestItem.distanceSquared = lengthSquared;
		}

		for (size_t k = 0; k < commander.squads.size(); ++k) {
			auto& squad = commander.squads[k];
			sa::vec3<float> uiPos = uiPosOf(squad.startPosition);
			lengthSquared = (uiPos - requestedPos).lengthSquared();
			if (lengthSquared < bestValue) {
				bestValue = lengthSquared;
				bestItem.squadIndex = static_cast<int>(k);
				bestItem.distanceSquared = lengthSquared;
			}
		}
	}

	return bestItem;
}

void TroopsTab::update(float dt)
{
	m_alpha += (m_targetAlpha - m_alpha) > 0 ? 0.05f : -0.05f;
	if ((m_targetAlpha - m_alpha) * (m_targetAlpha - m_alpha) < 0.05f * 0.05f)
		m_alpha = m_targetAlpha;
	bg.getColor().a = m_alpha;

	int mouseCode = m_pUserIO->getMouseKeyCode(0);

	if (m_pUserIO->isKeyReleased(mouseCode))
	{
		m_dragActive = false;
	}

	if (troopTabEnabled())
	{
		bg.update(0);

		if(hasFocus() && isMouseOver())
		{
			if (m_pUserIO->isKeyPressed(mouseCode))
			{
				m_dragActive = true;
				mousePos = m_pUserIO->getCursorPosition();
				mousePos.y /= m_pWindow->getAspectRatio();
				auto result = findNearest(mousePos.x, mousePos.y);

				if (result.distanceSquared < 0.05f * 0.05f) {
					// select (if already selected, drag).
					if (result.squadIndex == -1) {
						if (!m_commandersTab.isSelected(result.commanderIndex)) {
							m_commandersTab.unselectAll();
							m_commandersTab.select(result.commanderIndex);
						}

						auto selected = m_commandersTab.selectedCommanders();
						commanderOffsets.resize(selected.size());

						// Note: This could be optimized.
						for (uint32_t i = 0; i < selected.size(); ++i) {
							uint32_t commanderId = selected[i];
							for (uint32_t k = i; k < m_province.commanders.size(); ++k) {
								auto& commander = m_province.commanders[k];
								if (commander.id == commanderId) {
									commanderOffsets[i] = commander.combatOrder.startPos - uiPosToStart(mousePos);
									break;
								}
							}
						}
					}
					else {
						// squad selection?
						// TODO:
						// Plan: Squads can not be multiselected.
						//       Squads always move in relation with their commander.
					}
				}
				else {
					// unselect
					m_commandersTab.unselectAll();
					commanderOffsets.clear();
				}
			}
			else if (m_dragActive && m_pUserIO->isKeyDown(mouseCode)) {
				// continue drag.
				auto nextPos = m_pUserIO->getCursorPosition();
				nextPos.y /= m_pWindow->getAspectRatio();

				auto selected = m_commandersTab.selectedCommanders();
				for (size_t i = 0; i < selected.size(); ++i)
				{
					for (size_t k = i; k < m_province.commanders.size(); ++k)
					{
						if (m_province.commanders[k].id == selected[i])
						{
							m_province.commanders[k].combatOrder.startPos = wrapToBattleArea(
								commanderOffsets[i] + uiPosToStart(nextPos));
							break;
						}
					}
				}
			}

			// handle input
		}
	}
}

void TroopsTab::toggle()
{
	m_targetAlpha = 1 - m_targetAlpha;
}

bool TroopsTab::troopTabEnabled() const
{
	return m_alpha > 0.001f;
}

void TroopsTab::draw(std::shared_ptr<sa::Graphics> graphics) const
{
	if (troopTabEnabled())
	{
		bg.visualise(graphics);

		{
			// draw "battle area"
			sa::Matrix4 model;
			model.makeTranslationMatrix(battleAreaPos());
			model.scale(battleAreaScale());
			graphics->m_pRenderer->drawRectangle(model, "Empty", sa::vec4<float>(1, 1, 1, m_alpha));
		}

		sa::vec3<float> areaScale = battleAreaScale();
		sa::vec3<float> areaPos = battleAreaPos();

		auto selectedCommanders = m_commandersTab.selectedCommanders();
		size_t index = 0;

		// draw commanders and squads.
		for (size_t i = 0; i < m_province.commanders.size(); ++i)
		{
			const auto& commander = m_province.commanders[i];
			sa::vec2<int> commanderPosInt = commander.combatOrder.startPos;
			sa::vec3<float> commanderPos(
				areaScale.x * (commanderPosInt.x / 100.0f),
				areaScale.y * (commanderPosInt.y / 100.0f),
				0
			);
			
			float commanderAlpha = 0.7f;
			float commanderScale = 0.02f;
			if (index < selectedCommanders.size() && selectedCommanders[index] == commander.id)
			{
				++index;
				commanderAlpha = 1.0f;
				commanderScale = 0.022f;
			}

			sa::Matrix4 commanderModel;
			commanderModel.makeTranslationMatrix(areaPos + commanderPos);
			commanderModel.scale(commanderScale, commanderScale, 0);
			graphics->m_pRenderer->drawRectangle(
				commanderModel,
				commander.reference->icon,
				sa::vec4<float>(1, 1, 1, m_alpha * commanderAlpha)
			);
		}

	}
}

void TroopsTab::hide()
{
	m_targetAlpha = 0;
}
