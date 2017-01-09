
#include "RecruitmentTab.hpp"
#include "session/game/game.hpp"

ProvinceRecruitmentTab::ProvinceRecruitmentTab(sa::MenuComponent* parent, const ProvinceGraph::Province& province, const Game& game)
	: sa::MenuComponent(parent, "CommandersTab", []() {return sa::vec3<float>(0, -0.9f, 0); }, sa::vec3<float>(1, 0.3f, 0))
	, bg(this, "BG", "ButtonBase", sa::vec4<float>(1, 1, 1, 0.4f))
{
	this->positionAlign = sa::MenuComponent::PositionAlign::TOP;
	this->m_focus = true;

	for (const auto& troopName : province.troopsToRecruit)
	{
		const TroopReference* troopReference = game.troopReference(troopName);
		if (troopReference == nullptr)
		{
			LOG("Oh no, troop: '%s' not found!", troopName.c_str());
			continue;
		}


		auto icon = std::make_shared<RecruitmentIcon>(this, troopName, *troopReference);
		
		if (icons.empty())
		{
			icon->setTargetPosition([this]() {
				return this->getExteriorPositionForChild(sa::MenuComponent::TOP | sa::MenuComponent::LEFT) + sa::vec3<float>(0.03f, -0.1f, 0);
			});
			icon->positionAlign = sa::MenuComponent::TOP | sa::MenuComponent::LEFT;
		}
		else
		{
			if ((icons.size() % iconsPerRow) == 0)
			{
				int index = icons.size() - iconsPerRow;
				icon->setTargetPosition([this, index]() {
					auto pos = icons[index]->getLocalExteriorPosition(sa::MenuComponent::BOTTOM);
					return pos + sa::vec3<float>(0, -0.1f, 0);
				});
				icon->positionAlign = sa::MenuComponent::TOP;
			}
			else
			{
				int index = icons.size() - 1;
				icon->setTargetPosition([this, index]() {
					auto pos = icons[index]->getLocalExteriorPosition(sa::MenuComponent::RIGHT);
					return pos + sa::vec3<float>(0.03f, 0, 0);
				});
				icon->positionAlign = sa::MenuComponent::LEFT;
			}
		}
		
		this->addChild(icon);
		icons.emplace_back(icon);
	}

	open();
}


void ProvinceRecruitmentTab::RecruitmentIcon::draw(std::shared_ptr<sa::Graphics> graphics) const
{
	sa::Matrix4 model;
	auto pos = recursivePosition();
	model.makeTranslationMatrix(pos.x, pos.y, 0);
	model.rotate(0, 0, 0, 1);
	model.scale(m_worldScale.x * 0.5f, m_worldScale.y * 0.5f, 0);
	graphics->m_pRenderer->drawRectangle(model, "Hero", color); // TODO: Choose texture

	graphics->m_pTextRenderer->drawText(
		className,
		pos.x,
		(pos.y - 0.035f),
		0.04f,
		Color::GOLDEN,
		sa::TextRenderer::Align::CENTER,
		graphics->m_fontConsola
	);
}