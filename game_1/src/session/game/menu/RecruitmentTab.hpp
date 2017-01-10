
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
		RecruitmentIcon(sa::MenuComponent* parent, std::string className, const TroopReference& reference)
			: sa::MenuButton(parent, "RecruitmentIcon", []() {return sa::vec3<float>(0, 0, 0); }, sa::vec3<float>(0.1f, 0.1f, 0), "Hero", className)
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
				setTargetScale(m_defaultScale * 1.2f);
				targetAlpha = 1.0f;

				if (m_pUserIO->isKeyClicked(m_pUserIO->getMouseKeyCode(0)))
				{
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

			}
		}
	}

	virtual void draw(std::shared_ptr<sa::Graphics> graphics) const override
	{
		bg.draw(graphics);
	}

	virtual void hide() override
	{
		m_focus = false;
		m_targetPosition = [this]() { return getExteriorPosition(BOTTOM) - sa::vec3<float>(0, -0.05f / m_pWindow->getAspectRatio(), 0); };
	}

	virtual void update(float dt) override
	{
		bg.update(dt);
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
		return 0.2f * (icons.size() + iconsPerRow - 1) / iconsPerRow;
	}

	const int iconsPerRow = 8;
	bool isOpen = false;

	sa::MenuFrameBackground bg;
	std::shared_ptr<sa::MenuButton> openClose;
	std::vector<std::shared_ptr<RecruitmentIcon>> icons;
};