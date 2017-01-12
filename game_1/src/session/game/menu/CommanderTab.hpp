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
		CommanderIcon(sa::MenuComponent* parent, std::string name, std::string className, size_t id)
			: sa::MenuComponent(parent, "CommandersTab", []() {return sa::vec3<float>(0, 0, 0);}, sa::vec3<float>(0.1f, 0.1f, 0))
		{
			this->id = id;
			this->name = name;
			this->className = className;
			color = Color::WHITE;
		}

		virtual void childComponentCall(const std::string& who, const std::string& what, int = 0) {
		}

		virtual void draw(std::shared_ptr<sa::Graphics> graphics) const override {
			sa::Matrix4 model;
			auto pos = recursivePosition();
			model.makeTranslationMatrix(pos.x, pos.y, 0);
			model.rotate(rot, 0, 0, 1);
			model.scale(m_worldScale.x * 0.5f, m_worldScale.y * 0.5f, 0);
			graphics->m_pRenderer->drawRectangle(model, "Hero", color);
			graphics->m_pTextRenderer->drawText(
				name,
				pos.x,
				(pos.y - 0.015f),
				0.04f,
				Color::GOLDEN,
				sa::TextRenderer::Align::CENTER,
				graphics->m_fontConsola
			);
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

		virtual void update(float dt) override {
		}

		float rot = 0; // in case want to make some rotation effect lol
		sa::vec4<float> color;

		size_t id;
		std::string name;
		std::string className;
	};

	ProvinceCommandersTab(sa::MenuComponent* parent, const ProvinceGraph::Province& province)
		: sa::MenuComponent(parent, "CommandersTab", []() {return sa::vec3<float>(-0.95f, +0.9f, 0);}, sa::vec3<float>(0.5f, 1.0f, 0))
		, bg(this, "BG", "ButtonBase", sa::vec4<float>(1, 1, 1, 0.4f))
	{
		this->positionAlign = sa::MenuComponent::PositionAlign::LEFT | sa::MenuComponent::PositionAlign::TOP;
		this->m_focus = true;

		for (const auto& commander : province.commanders)
		{
			auto icon = std::make_shared<CommanderIcon>(this, commander.name, commander.reference.name, commander.id);

			if (icons.empty())
			{
				icon->setTargetPosition([this]() {
					return this->getExteriorPositionForChild(sa::MenuComponent::TOP | sa::MenuComponent::LEFT) + sa::vec3<float>(0.01f, -0.1f, 0);
				});
				icon->positionAlign = sa::MenuComponent::TOP | sa::MenuComponent::LEFT;
			}
			else
			{
				if ((icons.size() % 4) == 0)
				{
					int index = icons.size() - 4;
					icon->setTargetPosition([this, index]() {
						auto pos = icons[index]->getLocalExteriorPosition(sa::MenuComponent::BOTTOM);
						return pos;
					});
					icon->positionAlign = sa::MenuComponent::TOP;
				}
				else
				{
					int index = icons.size() - 1;
					icon->setTargetPosition([this, index]() {
						auto pos = icons[index]->getLocalExteriorPosition(sa::MenuComponent::RIGHT);
						return pos;
					});
					icon->positionAlign = sa::MenuComponent::LEFT;
				}
			}

			this->addChild(icon);
			icons.emplace_back(icon);
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