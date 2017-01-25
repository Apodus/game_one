
#include "session/game/menu/TroopManagementTab.hpp"
#include "session/game/menu/CommanderTab.hpp"

TroopsTab::TroopsTab(sa::MenuComponent* parent, Game& game, ProvinceCommandersTab& commandersTab)
	: sa::MenuComponent(
			parent,
			"TroopsTab",
			[this, parent]() {
				return sa::vec3<float>(0, 1, 0);
			} , sa::vec3<float>(1.0f, 0.1f, 0)
		)
	, bg(this, "bg", "Frame")
	, commandersTab(commandersTab)
	, m_game(game)
{
	positionAlign = TOP;
	setPositionUpdateType(true);
	bg.update(0);
}

void TroopsTab::update(float dt)
{

}

void TroopsTab::draw(std::shared_ptr<sa::Graphics> graphics) const
{

}