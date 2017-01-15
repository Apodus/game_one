#pragma once

#include "menu/MenuComponent.hpp"
#include "menu/MenuFrame.hpp"
#include "worldmap/ProvinceGraph.hpp"

#include <vector>
#include <string>

struct ProvinceCommandersTab : public sa::MenuComponent
{
	struct CommanderIcon : public sa::MenuComponent
	{
		CommanderIcon(sa::MenuComponent* parent, BattleCommander& commander)
			: sa::MenuComponent(parent, "CommandersTab", []() {return sa::vec3<float>(0, 0, 0);}, sa::vec3<float>(0.1f, 0.1f, 0))
			, m_commander(commander)
		{
			this->id = commander.id;
			this->name = commander.name;
			this->className = commander.reference.name;

			m_color = Color::GREY;
			setPositionUpdateType(true);
			m_commander.m_selected = false;
		}

		virtual void childComponentCall(const std::string& who, const std::string& what, int = 0) {
		}

		virtual void draw(std::shared_ptr<sa::Graphics> graphics) const override {
			sa::Matrix4 model;
			auto& pos = m_worldPosition;
			model.makeTranslationMatrix(pos.x, pos.y, 0);
			model.rotate(rot, 0, 0, 1);
			model.scale(m_worldScale.x * 0.5f, m_worldScale.y * 0.5f, 0);
			graphics->m_pRenderer->drawRectangle(model, m_commander.reference.icon, m_color);
			graphics->m_pTextRenderer->drawText(
				name,
				pos.x,
				(pos.y - 0.015f),
				0.04f,
				Color::GOLDEN,
				sa::TextRenderer::Align::CENTER,
				graphics->m_fontConsola
			);

			std::string actionName = "NoActionName";
			if (m_commander.myOrder.orderType == BattleCommander::OrderType::Idle) {
				actionName = "Idle";
			}
			else if(m_commander.myOrder.orderType == BattleCommander::OrderType::Move) {
				actionName = "Move";
			}

			graphics->m_pTextRenderer->drawText(
				actionName,
				pos.x,
				(pos.y - 0.035f),
				0.04f,
				Color::GOLDEN,
				sa::TextRenderer::Align::CENTER,
				graphics->m_fontConsola
			);
		}

		virtual void update(float dt) override
		{
			if (hasFocus() && isMouseOver())
			{
				targetAlpha = 1.0f;

				if (m_pUserIO->isKeyClicked(m_pUserIO->getMouseKeyCode(0)))
				{
					m_pUserIO->consume(m_pUserIO->getMouseKeyCode(0));
					callParent("click", static_cast<int>(id));

					selected = !selected;
					m_commander.m_selected = selected;

					float a = m_color.a;
					if (selected)
						m_color = Color::WHITE;
					else
						m_color = Color::GREY;
					m_color.a = a;
				}
			}
			else
			{
				targetAlpha = 0.8f;
			}

			m_color.a += (targetAlpha - m_color.a) * dt * 12;
		}

		BattleCommander& m_commander;

		float rot = 0; // in case want to make some rotation effect lol
		float targetAlpha = 1.0f;
		sa::vec4<float> m_color;

		size_t id;
		std::string name;
		std::string className;

		bool selected = false;
	};

	ProvinceCommandersTab(sa::MenuComponent* parent, ProvinceGraph::Province& province)
		: sa::MenuComponent(parent, "CommandersTab", []() {return sa::vec3<float>(-0.965f, +0.9f, 0);}, sa::vec3<float>(0.44f, 1.0f, 0))
		, bg(this, "BG", "ButtonBase", sa::vec4<float>(1, 1, 1, 0.4f))
	{
		this->positionAlign = sa::MenuComponent::PositionAlign::LEFT | sa::MenuComponent::PositionAlign::TOP;
		this->m_focus = true;

		for (auto& commander : province.commanders)
		{
			auto icon = std::make_shared<CommanderIcon>(this, commander);

			if (icons.empty())
			{
				icon->setTargetPosition([this]() {
					return this->getExteriorPosition(sa::MenuComponent::TOP | sa::MenuComponent::LEFT) + sa::vec3<float>(0.0178f, -0.1f * m_pWindow->getAspectRatio(), 0);
				});
				icon->positionAlign = sa::MenuComponent::TOP | sa::MenuComponent::LEFT;
			}
			else
			{
				if ((icons.size() % 4) == 0)
				{
					int index = icons.size() - 4;
					icon->setTargetPosition([this, index]() {
						auto pos = icons[index]->getExteriorPosition(sa::MenuComponent::BOTTOM);
						return pos;
					});
					icon->positionAlign = sa::MenuComponent::TOP;
				}
				else
				{
					int index = icons.size() - 1;
					icon->setTargetPosition([this, index]() {
						auto pos = icons[index]->getExteriorPosition(sa::MenuComponent::RIGHT);
						return pos;
					});
					icon->positionAlign = sa::MenuComponent::LEFT;
				}
			}

			this->addChild(icon);
			icons.emplace_back(icon);
		}
	}

	void emptyOrder() {
		for (auto& icon : icons)
			if (icon->selected)
				icon->m_commander.myOrder.orderType = BattleCommander::OrderType::Idle;
	}

	void orderToProvince(ProvinceGraph::Province* province) {
		for (size_t i = 0; i < icons.size(); ++i)
		{
			if (icons[i]->selected)
			{
				icons[i]->m_commander.myOrder.orderType = BattleCommander::OrderType::Move;
				icons[i]->m_commander.myOrder.moveTo = province->m_index;
			}
		}
	}

	virtual void childComponentCall(const std::string& who, const std::string& what, int = 0) {
	}

	virtual void draw(std::shared_ptr<sa::Graphics> graphics) const override {
		bg.draw(graphics);
		graphics->m_pTextRenderer->drawText(
			"Commanders",
			m_worldPosition.x,
			m_worldPosition.y + m_worldScale.y * 0.5f,
			0.1f,
			Color::CYAN,
			sa::TextRenderer::Align::CENTER,
			graphics->m_fontConsola
		);
	}

	virtual void hide() override
	{
		m_focus = false;
		m_targetPosition = [this]() { return getRelativePosition() + sa::vec3<float>(-0.5f, -0.05f / m_pWindow->getAspectRatio(), 0); };
		for (auto& elem : icons)
			elem->setFocus(false);
	}

	virtual void update(float dt) override {
		bg.update(dt);

		if (isMouseOver())
		{
			int key = m_pUserIO->getMouseKeyCode(0);
			if (m_pUserIO->isKeyClicked(key))
				m_pUserIO->consume(key);
		}
	}

	sa::MenuFrameBackground bg;
	std::vector<std::shared_ptr<CommanderIcon>> icons;
};