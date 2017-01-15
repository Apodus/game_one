
#include "session/game/menu/ResourceTab.hpp"
#include "session/game/game.hpp"

void ResourceTab::update(float dt)
{
	bg.update(0);
}

void ResourceTab::draw(std::shared_ptr<sa::Graphics> graphics) const
{
	bg.visualise(graphics);

	sa::Matrix4 model;
	sa::vec3<float> pos = getExteriorPosition(LEFT) + sa::vec3<float>(0.1f, 0, 0);
	pos.y /= m_pWindow->getAspectRatio();
	
	model.makeTranslationMatrix(pos);
	float iconScale = m_worldScale.y * 0.5f;
	model.scale(iconScale * 0.75f, iconScale * 0.75f, 0);
	graphics->m_pRenderer->drawRectangle(model, "Gold");

	int64_t gold = m_game.gold(m_localPlayer);

	graphics->m_pTextRenderer->drawText(
		std::to_string(gold),
		pos.x + 2 * iconScale,
		pos.y,
		iconScale,
		Color::WHITE,
		sa::TextRenderer::Align::LEFT,
		graphics->m_fontConsola
	);
}