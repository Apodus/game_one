
#include "session/game/menu/TroopManagementTab.hpp"
#include "session/game/menu/CommanderTab.hpp"

TroopsTab::TroopsTab(
	sa::MenuComponent* parent,
	ProvinceGraph::Province& province,
	ProvinceCommandersTab& commandersTab
)
	: sa::MenuComponent(
			parent,
			"TroopsTab",
			[this, parent]() {
				return parent->getExteriorPosition(RIGHT);
			} , sa::vec3<float>(1.0f, 1.0f, 0)
		)
	, bg(this, "bg", "Frame")
	, m_commandersTab(commandersTab)
	, m_province(province)
{
	positionAlign = LEFT;
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
		auto pos = startPosToUI(commander.combatOrder.startPos) + battleAreaPos();
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
			sa::vec3<float> uiPos = startPosToUI(squad.startPosition) + battleAreaPos();
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
									// TODO
									break;
								}
							}
						}
					}
					else {
						// squad selection?
					}
				}
				else {
					// unselect
					m_commandersTab.unselectAll();
				}
			}
			else if (m_dragActive && m_pUserIO->isKeyDown(mouseCode)) {
				// continue drag.
				auto nextPos = m_pUserIO->getCursorPosition();
				sa::vec2<int> theStartPos = uiPosToStart(nextPos);

				auto selected = m_commandersTab.selectedCommanders();
				for (auto& commander : m_province.commanders) {
					// TODO
					// commander.combatOrder.startPos += diff;
					// m_faction.turn->setCombatPosition();
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
			
			sa::Matrix4 commanderModel;
			commanderModel.makeTranslationMatrix(areaPos + commanderPos);
			commanderModel.scale(0.02f, 0.02f, 0);
			graphics->m_pRenderer->drawRectangle(
				commanderModel,
				commander.reference->icon,
				sa::vec4<float>(1, 1, 1, m_alpha)
			);
		}

	}
}

void TroopsTab::hide()
{
	m_targetAlpha = 0;
}
