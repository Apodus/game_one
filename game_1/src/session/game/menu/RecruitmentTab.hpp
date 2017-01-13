
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

	struct RecruitmentIcon : public sa::MenuComponent
	{
		RecruitmentIcon(sa::MenuComponent* parent, std::string className, const TroopReference& reference, const std::string& componentName)
			: sa::MenuComponent(parent, componentName, []() {return sa::vec3<float>(0, 0, 0); }, sa::vec3<float>(0.1f, 0.1f, 0))
			, troopReference(reference)
		{
			this->className = className;
			this->m_text = className;
			m_color = Color::WHITE;
			setPositionUpdateType(true);

			icon = reference.icon;
		}

		virtual void childComponentCall(const std::string& who, const std::string& what, int = 0) {}

		virtual void update(float dt) override
		{
			if (hasFocus() && isMouseOver())
			{
				targetAlpha = 1.0f;

				if (m_pUserIO->isKeyClicked(m_pUserIO->getMouseKeyCode(0)))
				{
					m_pUserIO->consume(m_pUserIO->getMouseKeyCode(0));
					callParent("click", myIndex);
				}
			}
			else
			{
				targetAlpha = 0.7f;
			}

			m_color.a += (targetAlpha - m_color.a) * dt * 12;
		}

		virtual void draw(std::shared_ptr<sa::Graphics> graphics) const override
		{
			sa::Matrix4 model;
			model.makeTranslationMatrix(getPosition());
			model.scale(getScale() * 0.5f);

			const sa::vec3<float>& m_pos = getPosition();
			graphics->m_pRenderer->drawRectangle(model, icon, m_color);

			if (!m_text.empty()) {
				graphics->m_pTextRenderer->drawText(
					m_text,
					m_pos.x,
					m_pos.y - m_worldScale.y * 0.5f,
					0.035f,
					Color::GOLDEN,
					sa::TextRenderer::Align::CENTER,
					graphics->m_fontConsola
				);
			}
		}

		void noText()
		{
			m_text = "";
		}

		float targetAlpha = 1.0f;
		std::string icon;
		std::string m_text;

		int myIndex = -1;
		sa::vec4<float> m_color;
		std::string className;
		const TroopReference& troopReference;
	};

	void addRecruitmentOrderIcon(const TroopReference* troopReference) {
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