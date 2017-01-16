
#pragma once

#include "menu/MenuComponent.hpp"
#include "menu/MenuFrame.hpp"
#include "menu/MenuButton.hpp"
#include "graphics/graphics.hpp"
#include "worldmap/ProvinceGraph.hpp"

#include <memory>
#include <string>

class Game;
class Faction;

struct ProvinceRecruitmentTab : public sa::MenuComponent
{
	ProvinceRecruitmentTab(
		sa::MenuComponent* parent,
		const ProvinceGraph::Province& province,
		const Game& game,
		Faction& faction
	);

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

	void addRecruitmentOrderIcon(const TroopReference* troopReference);

	virtual void childComponentCall(const std::string& who, const std::string& what, int value = 0);
	virtual void draw(std::shared_ptr<sa::Graphics> graphics) const override;
	virtual void hide() override;
	virtual void update(float dt) override;

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
	Faction& m_faction; // points to temporary data used only for menu visualization.

	std::shared_ptr<sa::MenuButton> openClose;
	std::vector<std::shared_ptr<RecruitmentIcon>> icons;
	std::vector<std::shared_ptr<RecruitmentIcon>> recruitmentOrders;
};