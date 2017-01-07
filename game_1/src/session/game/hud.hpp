#pragma once

#include "menu/MenuComponent.hpp"
#include "menu/MenuButton.hpp"
#include "menu/MenuSlider.hpp"
#include "graphics/color.hpp"
#include "input/userio.hpp"

#include "worldmap/ProvinceGraph.hpp"

#include "localization/localization.hpp"

#include <memory>

class Game;

class Hud : public sa::MenuComponent {
public:
	Hud(
		Game& game,
		MenuComponent* parent,
		const std::string& name,
		const sa::vec3<float>& position,
		const sa::vec3<float>& scale
	) : MenuComponent(parent, name, position, scale)
		, game(game)
	{
		auto button = std::make_shared<sa::MenuButton>(
			this,
			"RunBattleTest",
			sa::vec3<float>(0.99f, -0.99f, 0),
			sa::vec3<float>(0.4f, 0.10f, 0),
			"ButtonBase",
			"Battle Test",
			sa::MenuComponent::BOTTOM | sa::MenuComponent::RIGHT
		);

		auto demoButton = std::make_shared<sa::MenuButton>(
			this,
			"PositionFunctionDemo",
			[button]() { return button->getExteriorPosition(sa::MenuComponent::TOP | sa::MenuComponent::RIGHT); },
			sa::vec3<float>(0.4f, 0.10f, 0),
			"ButtonBase",
			"^gH^ye^bl^wl^co ^w:)",
			sa::MenuComponent::BOTTOM | sa::MenuComponent::RIGHT
		);

		auto demoButton2 = std::make_shared<sa::MenuButton>(
			this,
			"PositionFunctionDemo",
			[demoButton]() { return demoButton->getExteriorPosition(sa::MenuComponent::TOP | sa::MenuComponent::RIGHT); },
			sa::vec3<float>(0.4f, 0.10f, 0),
			"ButtonBase",
			"^gH^ye^bl^wl^co ^w:)",
			sa::MenuComponent::BOTTOM | sa::MenuComponent::RIGHT
		);

		addChild(button);
		addChild(demoButton);
		addChild(demoButton2);
	}


	struct ProvinceRecruitmentTab : public sa::MenuComponent
	{

	};

	struct ProvinceInfoTab : public sa::MenuComponent
	{

	};

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

		ProvinceCommandersTab(sa::MenuComponent* parent, ProvinceGraph::Province& province)
			: sa::MenuComponent(parent, "CommandersTab", []() {return sa::vec3<float>(-0.95f, +0.9f, 0);}, sa::vec3<float>(0.5f, 1.0f, 0))
			, bg(this, "BG", "ButtonBase", sa::vec4<float>(1, 1, 1, 0.4f))
		{
			this->positionAlign = sa::MenuComponent::PositionAlign::LEFT | sa::MenuComponent::PositionAlign::TOP;
			this->m_focus = true;

			for (auto& commander : province.commanders)
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
			m_targetPosition = []() { return sa::vec3<float>(-1.7f, -1.7f, 0); };
		}

		virtual void update(float dt) override {
			bg.update(dt);
		}

		sa::MenuFrameBackground bg;
		std::vector<std::shared_ptr<CommanderIcon>> icons;
	};

	void selectProvince(ProvinceGraph::Province& province)
	{
		unselectProvince();
		auto commanderTab = std::make_shared<ProvinceCommandersTab>(this, province);
		provinceMenu.emplace_back(commanderTab);
	}

	void unselectProvince()
	{
		for(auto& entry : provinceMenu)
			entry->hide();
	}

private:

	Game& game;
	std::vector<std::shared_ptr<sa::MenuComponent>> provinceMenu;

	virtual void childComponentCall(const std::string& who, const std::string& what, int = 0) override;

	virtual void draw(std::shared_ptr<sa::Graphics> graphics) const override
	{
		for (auto& provinceItem : provinceMenu)
		{
			provinceItem->visualise(graphics);
		}
	}

	virtual void update(float dt) override
	{
		for (auto& provinceItem : provinceMenu)
		{
			provinceItem->tick(dt);
		}

		// delete obsolete menus
		for (size_t i = 0; i < provinceMenu.size(); ++i)
		{
			auto& menuItem = provinceMenu[i];
			if (!menuItem->hasFocus() && !menuItem->inScreen())
			{
				provinceMenu[i] = provinceMenu.back();
				provinceMenu.pop_back();
			}
		}
	}
};
