
#include "RecruitmentTab.hpp"
#include "session/game/game.hpp"

ProvinceRecruitmentTab::ProvinceRecruitmentTab(sa::MenuComponent* parent, const ProvinceGraph::Province& province, const Game& game)
	: sa::MenuComponent(parent, "CommandersTab", []() {return sa::vec3<float>(0, -0.95f, 0); }, sa::vec3<float>(1, 0.3f, 0))
	, bg(this, "BG", "ButtonBase", sa::vec4<float>(1, 1, 1, 0.4f))
{
	this->positionAlign = sa::MenuComponent::PositionAlign::BOTTOM;
	this->m_focus = true;

	openClose = std::make_shared<sa::MenuButton>(
		this,
		"RecruitmentOpenClose",
		[this](){ return getExteriorPosition(TOP); },
		sa::vec3<float>(0.3f, 0.1f, 0),
		"Frame",
		"Recruitment",
		PositionAlign::BOTTOM,
		sa::TextRenderer::Align::CENTER,
		Color::WHITE,
		Color::WHITE
	);
	openClose->setPositionUpdateType(true);

	this->addChild(openClose);

	for (const auto& troopName : province.troopsToRecruit)
	{
		const TroopReference* troopReference = game.troopReference(troopName);
		if (troopReference == nullptr)
		{
			LOG("Oh no, troop: '%s' not found!", troopName.c_str());
			continue;
		}


		auto icon = std::make_shared<RecruitmentIcon>(this, troopName, *troopReference);
		icon->myIndex = icons.size();

		if (icons.empty())
		{
			icon->setTargetPosition([this]() {
				return this->getExteriorPosition(sa::MenuComponent::TOP | sa::MenuComponent::LEFT) + sa::vec3<float>(0.03f, -0.03f, 0);
			});
			icon->positionAlign = sa::MenuComponent::TOP | sa::MenuComponent::LEFT;
		}
		else
		{
			if ((icons.size() % iconsPerRow) == 0)
			{
				int index = icons.size() - iconsPerRow;
				icon->setTargetPosition([this, index]() {
					auto pos = icons[index]->getExteriorPosition(sa::MenuComponent::BOTTOM);
					return pos + sa::vec3<float>(0, -0.9f, 0);
				});
				icon->positionAlign = sa::MenuComponent::TOP;
			}
			else
			{
				int index = icons.size() - 1;
				icon->setTargetPosition([this, index]() {
					auto pos = icons[index]->getExteriorPosition(sa::MenuComponent::RIGHT);
					return pos + sa::vec3<float>(0.03f, 0, 0);
				});
				icon->positionAlign = sa::MenuComponent::LEFT;
			}
		}
		
		this->addChild(icon);
		icons.emplace_back(icon);
	}

	close();
}