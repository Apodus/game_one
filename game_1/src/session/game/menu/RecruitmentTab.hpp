
#pragma once

#include "menu/MenuComponent.hpp"
#include "menu/MenuFrame.hpp"
#include "menu/MenuButton.hpp"
#include "graphics/graphics.hpp"
#include "worldmap/ProvinceGraph.hpp"

#include <memory>
#include <string>

class Game;

struct ProvinceRecruitmentTab : public sa::MenuComponent
{
	ProvinceRecruitmentTab(sa::MenuComponent* parent, const ProvinceGraph::Province& province, const Game& game);

	struct RecruitmentIcon : public sa::MenuButton
	{
		RecruitmentIcon(sa::MenuComponent* parent, std::string className, const TroopReference& reference, const std::string& componentName)
			: sa::MenuButton(parent, componentName, []() {return sa::vec3<float>(0, 0, 0); }, sa::vec3<float>(0.1f, 0.1f, 0), "Hero", className)
			, troopReference(reference)
		{
			this->className = className;
			color = Color::WHITE;
			setPositionUpdateType(true);
		}

		virtual void childComponentCall(const std::string& who, const std::string& what, int = 0) {}

		virtual void update(float dt) override
		{
			m_frame.tick(dt);

			m_model.makeTranslationMatrix(getPosition());
			m_model.scale(getScale());

			if (hasFocus() && isMouseOver())
			{
				setTargetScale(m_defaultScale * 1.05f);
				targetAlpha = 1.0f;

				if (m_pUserIO->isKeyClicked(m_pUserIO->getMouseKeyCode(0)))
				{
					m_pUserIO->consume(m_pUserIO->getMouseKeyCode(0));
					callParent("click", myIndex);
				}
			}
			else
			{
				setTargetScale(m_defaultScale);
				targetAlpha = 0.3f;
			}

			m_color.a += (targetAlpha - m_color.a) * dt;
		}

		void noText()
		{
			m_text = "";
		}

		int myIndex = -1;
		sa::vec4<float> color;
		std::string className;
		const TroopReference& troopReference;
	};


	virtual void childComponentCall(const std::string& who, const std::string& what, int value = 0)
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
				auto recruitmentOrderIcon = std::make_shared<RecruitmentIcon>(this, icons[value]->className, icons[value]->troopReference, "CancelRecruitment");
				recruitmentOrderIcon->myIndex = value;

				float elementWidth = 0.06f * 1.01f;
				int elementsPerRow = static_cast<int>(m_worldScale.x / elementWidth);
				int myX = recruitmentOrders.size() % elementsPerRow;
				int myY = recruitmentOrders.size() / elementsPerRow;
				recruitmentOrderIcon->setTargetPosition([this, elementWidth, myX, myY]() {
					return getExteriorPosition(sa::MenuComponent::LEFT | sa::MenuComponent::TOP) +
						sa::vec3<float>(myX * elementWidth, myY * -elementWidth * m_pWindow->getAspectRatio(), 0);
				});
				recruitmentOrderIcon->positionAlign = sa::MenuComponent::LEFT | sa::MenuComponent::TOP;
				recruitmentOrderIcon->tick(0.5f);
				recruitmentOrderIcon->setDefaultScale(sa::vec3<float>(elementWidth / 1.01f, elementWidth / 1.01f, 0));
				recruitmentOrderIcon->noText();
				recruitmentOrders.emplace_back(recruitmentOrderIcon);

				setTargetScale(sa::vec3<float>(1, height(), 0));

				// TODO: when creating the tab from scratch, read the province for current recruitment requests.

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

	virtual void draw(std::shared_ptr<sa::Graphics> graphics) const override
	{
		bg.draw(graphics);
		for (auto& rec : recruitmentOrders)
			rec->visualise(graphics);
	}

	virtual void hide() override
	{
		m_focus = false;
		openClose->setFocus(false);
		for (auto& elem : icons)
			elem->setFocus(false);
		for (auto& elem : recruitmentOrders)
			elem->setFocus(false);

		m_targetPosition = [this]() { return sa::vec3<float>(0, -2, 0); };
	}

	virtual void update(float dt) override
	{
		// delayed removal.
		if (removeRecruitmentOrderIndex >= 0)
		{
			recruitmentOrders.erase(recruitmentOrders.begin() + removeRecruitmentOrderIndex);
			removeRecruitmentOrderIndex = -1;

			// update remaining recruitment orders' positions
			for (size_t i = 0; i < recruitmentOrders.size(); ++i)
			{
				auto& recruitmentOrderIcon = recruitmentOrders[i];
				float elementWidth = 0.06f * 1.01f;
				int elementsPerRow = static_cast<int>(m_worldScale.x / elementWidth);
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

		for (size_t i=0; i<recruitmentOrders.size(); ++i)
			recruitmentOrders[i]->tick(dt);
	}

	// open / close this panel
	void open()
	{
		isOpen = true;
		setTargetPosition(
			[this]() { return sa::vec3<float>(0, -1.0f, 0); }
		);
	}

	void close()
	{
		isOpen = false;
		setTargetPosition(
			[this]() { return sa::vec3<float>(0, -1.0f - m_worldScale.y * m_pWindow->getAspectRatio(), 0); }
		);
	}

private:
	float height() const
	{
		return 0.1f + 0.2f * (icons.size() + iconsPerRow - 1) / iconsPerRow;
	}

	const int iconsPerRow = 8;
	bool isOpen = false;
	int removeRecruitmentOrderIndex = -1;

	sa::MenuFrameBackground bg;
	std::shared_ptr<sa::MenuButton> openClose;
	std::vector<std::shared_ptr<RecruitmentIcon>> icons;
	std::vector<std::shared_ptr<RecruitmentIcon>> recruitmentOrders;
};