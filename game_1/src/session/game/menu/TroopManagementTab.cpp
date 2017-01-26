
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

		sa::vec3<float> battleAreaPos = getPosition() + getScale() * sa::vec3<float>(0, 0.45f * 0.5f, 0);
		sa::vec3<float> battleAreaScale = getScale() * sa::vec3<float>(0.975f, 0.50f, 1) * 0.5f;

		{
			// draw "battle area"
			sa::Matrix4 model;
			model.makeTranslationMatrix(battleAreaPos);
			model.scale(battleAreaScale);
			graphics->m_pRenderer->drawRectangle(model, "Empty", sa::vec4<float>(1, 1, 1, m_alpha));
		}

		// draw commanders and squads.
		for (size_t i = 0; i < m_province.commanders.size(); ++i)
		{
			const auto& commander = m_province.commanders[i];
			sa::vec2<int> commanderPosInt = commander.combatOrder.startPos;
			sa::vec3<float> commanderPos(
				battleAreaScale.x * (commanderPosInt.x / 100.0f),
				battleAreaScale.y * (commanderPosInt.y / 100.0f),
				0
			);
			
			sa::Matrix4 commanderModel;
			commanderModel.makeTranslationMatrix(battleAreaPos + commanderPos);
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
