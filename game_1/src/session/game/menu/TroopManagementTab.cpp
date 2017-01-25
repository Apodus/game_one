
#include "session/game/menu/TroopManagementTab.hpp"
#include "session/game/menu/CommanderTab.hpp"

TroopsTab::TroopsTab(sa::MenuComponent* parent, Game& game, ProvinceCommandersTab& commandersTab)
	: sa::MenuComponent(
			parent,
			"TroopsTab",
			[this, parent]() {
				return parent->getExteriorPosition(RIGHT);
			} , sa::vec3<float>(1.0f, 0.1f, 0)
		)
	, bg(this, "bg", "Frame")
	, commandersTab(commandersTab)
	, m_game(game)
{
	positionAlign = LEFT;
	setPositionUpdateType(true);
	bg.update(0);

	alpha = 0;
	targetAlpha = 0;
}

void TroopsTab::update(float dt)
{
	alpha += (targetAlpha - alpha) > 0 ? 0.05f : -0.05f;
	if ((targetAlpha - alpha) * (targetAlpha - alpha) < 0.05f * 0.05f)
		alpha = targetAlpha;
	bg.getColor().a = alpha;

	if (troopTabEnabled())
	{
		bg.update(0);

		// handle input
	}
}

void TroopsTab::toggle()
{
	targetAlpha = 1 - targetAlpha;
}

bool TroopsTab::troopTabEnabled() const
{
	return alpha > 0.001f;
}

void TroopsTab::draw(std::shared_ptr<sa::Graphics> graphics) const
{
	if (troopTabEnabled())
	{
		bg.visualise(graphics);
	}
}