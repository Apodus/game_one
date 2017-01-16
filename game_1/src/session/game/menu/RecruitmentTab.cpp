
#include "RecruitmentTab.hpp"
#include "session/game/game.hpp"

ProvinceRecruitmentTab::ProvinceRecruitmentTab(
	sa::MenuComponent* parent,
	const ProvinceGraph::Province& province,
	const Game& game,
	Faction& faction
)
	: sa::MenuComponent(parent, "RecruitmentTab", []() {return sa::vec3<float>(0, -0.95f, 0); }, sa::vec3<float>(1, 0.3f, 0))
	, bg(this, "BG", "ButtonBase", sa::vec4<float>(1, 1, 1, 0.4f))
	, m_faction(faction)
{
	this->positionAlign = sa::MenuComponent::PositionAlign::BOTTOM;
	this->m_focus = true;

	openClose = std::make_shared<sa::MenuButton>(
		this,
		"RecruitmentOpenClose",
		[this](){ return getExteriorPosition(TOP); },
		sa::vec3<float>(0.5f, 0.1f, 0),
		"Frame",
		"Recruitment",
		PositionAlign::BOTTOM,
		sa::TextRenderer::Align::CENTER,
		Color::WHITE,
		Color::WHITE
	);
	openClose->setPositionUpdateType(true);

	this->addChild(openClose);

	// fill recruitment options
	for (const auto& troopName : province.troopsToRecruit)
	{
		const TroopReference* troopReference = game.troopReference(troopName);
		if (troopReference == nullptr)
		{
			LOG("Oh no, troop: '%s' not found!", troopName.c_str());
			continue;
		}


		auto icon = std::make_shared<RecruitmentIcon>(this, troopName, *troopReference, "RecruitmentIcon");
		icon->myIndex = icons.size();

		if (icons.empty())
		{
			icon->setTargetPosition([this]() {
				return this->getExteriorPosition(sa::MenuComponent::TOP | sa::MenuComponent::LEFT) + sa::vec3<float>(0.03f, -0.2f * m_pWindow->getAspectRatio(), 0);
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
					return pos + sa::vec3<float>(0, -0.03f, 0);
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
		setTargetScale(sa::vec3<float>(1, height(), 0));
	}

	// fill current recruitment orders to recruitment order icons
	for (const auto* troopReference : province.inspectRecruitOrders()) {
		addRecruitmentOrderIcon(troopReference);
	}

	close();
}



void ProvinceRecruitmentTab::addRecruitmentOrderIcon(const TroopReference* troopReference) {
	// remove cost of troop from player's bank account.
	if (m_faction.currency < troopReference->goldCost)
	{
		// TODO: inform player of insufficient gold funds.
		return;
	}

	m_faction.currency -= troopReference->goldCost;

	auto recruitmentOrderIcon = std::make_shared<RecruitmentIcon>(this, troopReference->name, *troopReference, "CancelRecruitment");
	for (size_t i = 0; i<icons.size(); ++i)
	{
		if (&icons[i]->troopReference == troopReference)
		{
			recruitmentOrderIcon->myIndex = i;
			break;
		}
	}

	float elementWidth = 0.06f * 1.01f;
	int elementsPerRow = static_cast<int>(m_targetScale.x / elementWidth);
	int myX = recruitmentOrders.size() % elementsPerRow;
	int myY = recruitmentOrders.size() / elementsPerRow;
	recruitmentOrderIcon->setTargetPosition([this, elementWidth, myX, myY]() {
		return getExteriorPosition(sa::MenuComponent::LEFT | sa::MenuComponent::TOP) +
			sa::vec3<float>(myX * elementWidth, myY * -elementWidth * m_pWindow->getAspectRatio(), 0);
	});
	recruitmentOrderIcon->positionAlign = sa::MenuComponent::LEFT | sa::MenuComponent::TOP;
	recruitmentOrderIcon->tick(0.5f);
	recruitmentOrderIcon->setTargetScale(sa::vec3<float>(elementWidth / 1.01f, elementWidth / 1.01f, 0));
	recruitmentOrderIcon->noText();
	recruitmentOrders.emplace_back(recruitmentOrderIcon);
	setTargetScale(sa::vec3<float>(1, height(), 0));
}


void ProvinceRecruitmentTab::childComponentCall(const std::string& who, const std::string& what, int value)
{
	if (who == "RecruitmentOpenClose")
	{
		isOpen = !isOpen;
		if (isOpen)
			open();
		else
			close();
	}
	else if (who == "RecruitmentIcon")
	{
		if (what == "click")
		{
			addRecruitmentOrderIcon(&icons[value]->troopReference);
			callParent(icons[value]->className, 1);
		}
	}
	else if (who == "CancelRecruitment")
	{
		if (what == "click")
		{
			// inform parent to remove troop recruitment from province orders
			callParent(icons[value]->className, 2);

			// remove the icon from recruitment orders menu
			for (size_t i = 0; i < recruitmentOrders.size(); ++i)
			{
				if (recruitmentOrders[i]->className == icons[value]->className)
				{
					removeRecruitmentOrderIndex = i;
					break;
				}
			}

			setTargetScale(sa::vec3<float>(1, height(), 0));
		}
	}
}

void ProvinceRecruitmentTab::draw(std::shared_ptr<sa::Graphics> graphics) const
{
	bg.draw(graphics);
	for (auto& rec : recruitmentOrders)
		rec->visualise(graphics);
}

void ProvinceRecruitmentTab::hide()
{
	m_focus = false;
	openClose->setFocus(false);
	for (auto& elem : icons)
		elem->setFocus(false);
	for (auto& elem : recruitmentOrders)
		elem->setFocus(false);

	m_targetPosition = [this]() { return sa::vec3<float>(0, -2, 0); };
}

void ProvinceRecruitmentTab::update(float dt)
{
	// delayed removal.
	if (removeRecruitmentOrderIndex >= 0)
	{
		// award cost of troop back to player bank account.
		m_faction.currency += recruitmentOrders[removeRecruitmentOrderIndex]->troopReference.goldCost;

		recruitmentOrders.erase(recruitmentOrders.begin() + removeRecruitmentOrderIndex);
		removeRecruitmentOrderIndex = -1;

		// update remaining recruitment orders' positions
		for (size_t i = 0; i < recruitmentOrders.size(); ++i)
		{
			auto& recruitmentOrderIcon = recruitmentOrders[i];
			float elementWidth = 0.06f * 1.01f;
			int elementsPerRow = static_cast<int>(m_targetScale.x / elementWidth);
			int myX = i % elementsPerRow;
			int myY = i / elementsPerRow;
			recruitmentOrderIcon->setTargetPosition([this, elementWidth, myX, myY]() {
				return getExteriorPosition(sa::MenuComponent::LEFT | sa::MenuComponent::TOP) +
					sa::vec3<float>(myX * elementWidth, myY * -elementWidth * m_pWindow->getAspectRatio(), 0);
			});
		}
	}

	bg.update(dt);

	if (isMouseOver())
	{
		int key = m_pUserIO->getMouseKeyCode(0);
		if (m_pUserIO->isKeyClicked(key))
			m_pUserIO->consume(key);
	}

	for (size_t i = 0; i<recruitmentOrders.size(); ++i)
		recruitmentOrders[i]->tick(dt);
}