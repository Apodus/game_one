
#include "session/game/menu/TroopManagementTab.hpp"
#include "session/game/menu/CommanderTab.hpp"

CommanderEntry::CommanderEntry(
	TroopsTab* parent,
	sa::MenuComponent* alignUnder,
	BattleCommander* commander,
	Hider& hider,
	Faction& faction,
	ProvinceGraph::Province& province
)
	: sa::MenuComponent(
		parent,
		"CommanderEntry",
		[this, alignUnder]() { return alignUnder->getExteriorPosition(BOTTOM); },
		sa::vec3<float>(1.0f, 0.03f, 0)
	)
	, m_hider(hider)
	, commander(commander)
	, bg(this, "bg", "Frame")
	, m_faction(faction)
	, m_province(province)
{
	positionAlign = TOP;
	bg.tick(0);
}


void CommanderEntry::draw(std::shared_ptr<sa::Graphics> graphics) const
{
	if (m_drawEnabled)
	{
		bg.visualise(graphics);

		float iconScale = 0.02f;
		float ar = m_pWindow->getAspectRatio();

		// 1. draw commander icon to top left corner
		auto commanderIconPos = getExteriorPosition(LEFT | TOP);
		commanderIconPos.x += iconScale;
		commanderIconPos.y -= iconScale;
		commanderIconPos.y /= ar;

		sa::Matrix4 model;
		model.makeTranslationMatrix(commanderIconPos);
		model.scale(iconScale, iconScale, 0);
		graphics->m_pRenderer->drawRectangle(model, commander->reference->icon, sa::vec4<float>(1, 1, 1, m_alpha));

		// 2. draw list of squads to left side (visualise squad members towards the right)
		auto pos = getExteriorPosition(TOP);
		pos.x += iconScale / 2.0f;
		pos.y /= ar;
		model.makeTranslationMatrix(pos);
		model.scale(getScale() - sa::vec3<float>(iconScale / 2.0f, 0, 0));
		graphics->m_pRenderer->drawRectangle(model, "Empty", sa::vec4<float>(1, 1, 1, 0.5f * m_alpha));


		// NOTE: Doing the following every frame is waste of time. If it turns out to be a problem, can optimize for sure.
		//       just do this step in the constructor.

		// gather unit icons and their amounts.
		std::unordered_map<std::string, int> iconAndNumber;
		
		for (auto troopId : commander->squad.unitIds) {
			// bin the units by type?
			// TODO: take experience levels into account
			// TODO: something im forgetting?
			for (auto& troop : m_province.units) {
				if (troop.id == troopId) {
					++iconAndNumber[troop.reference->icon];
					continue;
				}
			}
		}

		for (auto& entry : iconAndNumber) {
			model.makeTranslationMatrix(pos.x - getScale().x - (iconScale * 0.5f), iconScale * 0.5f, 0);
			model.scale(iconScale);
			graphics->m_pRenderer->drawRectangle();
		}
	}
	
	// TODO: List of CommanderEntries should scroll
}

void CommanderEntry::update(float dt)
{
	bg.tick(dt);
	m_alpha = m_parentAlpha * m_hider.alpha(m_worldPosition.y);
	bg.getColor().a = m_alpha;

	m_drawEnabled = m_alpha > 0.0000001f;
	
	setTargetScale(
		sa::vec3<float>(
			1.0f,
			0.06f, // todo adjust size dynamically how?
			0
		)
	);

	// TODO: Editing squads?? How?
}

TroopsTab::TroopsTab(
	sa::MenuComponent* parent,
	ProvinceGraph::Province& province,
	ProvinceCommandersTab& commandersTab
)
	: sa::MenuComponent(
			parent,
			"TroopsTab",
			[this, parent]() {
				return parent->getExteriorPosition(RIGHT | TOP);
			},
			[this, parent]() {
				return sa::vec3<float>(1.0f, 0.9f, 0);
			}
				)
	, bg(this, "bg", "Frame")
	, battleArea(this, "battleArea", [this]() {return battleAreaPos(); }, [this]() {return battleAreaScale(); })
	, m_commandersTab(commandersTab)
	, m_province(province)
{
	positionAlign = LEFT | TOP;
	setPositionUpdateType(true);
	bg.update(0);

	m_alpha = 0;
	m_targetAlpha = 0;
}

sa::vec3<float> TroopsTab::startPosToUI(sa::vec2<int> posInt) const {
	auto areaScale = battleAreaScale();
	return sa::vec3<float>(
		areaScale.x * (posInt.x / 100.0f),
		areaScale.y * (posInt.y / 100.0f),
		0
	);
}

sa::vec2<int> TroopsTab::uiPosToStart(sa::vec3<float> posUI) const {
	auto areaScale = battleAreaScale();
	return sa::vec2<int>(
		static_cast<int>(100.0f * posUI.x / areaScale.x),
		static_cast<int>(100.0f * posUI.y / areaScale.y)
	);
}

CommanderOrSquad TroopsTab::findNearest(float x, float y, float selectionSquareDistance) const {
	float bestValue = 1000000;
	CommanderOrSquad bestItem;

	float bestSelectedValue = bestValue;
	CommanderOrSquad bestSelectedItem;

	sa::vec3<float> requestedPos(x, y, 0);
	for (size_t i = 0; i < m_province.commanders.size(); ++i) {
		const auto& commander = m_province.commanders[i];
		auto pos = uiPosOf(commander.combatOrder.startPos);
		float lengthSquared = (requestedPos - pos).lengthSquared();

		if (m_commandersTab.isSelectedIndex(i)) {
			if (lengthSquared < bestSelectedValue) {
				bestSelectedValue = lengthSquared;
				bestSelectedItem.commanderIndex = commander.id;
				bestSelectedItem.squadIndex = -1;
				bestSelectedItem.distanceSquared = lengthSquared;
			}
		}

		if (lengthSquared < bestValue) {
			bestValue = lengthSquared;
			bestItem.commanderIndex = commander.id;
			bestItem.squadIndex = -1;
			bestItem.distanceSquared = lengthSquared;
		}

		/*
		for (size_t k = 0; k < commander.squads.size(); ++k) {
			auto& squad = commander.squads[k];
			sa::vec3<float> uiPos = uiPosOf(squad.startPosition);
			lengthSquared = (uiPos - requestedPos).lengthSquared();
			if (lengthSquared < bestValue) {
				bestValue = lengthSquared;
				bestItem.squadIndex = static_cast<int>(k);
				bestItem.distanceSquared = lengthSquared;
			}
		}
		*/
	}

	if (bestSelectedValue < selectionSquareDistance)
		return bestSelectedItem;
	return bestItem;
}

void TroopsTab::update(float dt)
{
	float ar = m_pWindow->getAspectRatio();
	m_hider.min = getExteriorPosition(BOTTOM).y / ar;
	m_hider.max = battleArea.getExteriorPosition(BOTTOM).y / ar;
	m_hider.setSofteningByPercentage(0.05f);

	m_alpha += (m_targetAlpha - m_alpha) > 0 ? 0.05f : -0.05f;
	if ((m_targetAlpha - m_alpha) * (m_targetAlpha - m_alpha) < 0.05f * 0.05f)
		m_alpha = m_targetAlpha;
	bg.getColor().a = m_alpha;
	battleArea.tick(dt);

	for (const auto& entry : commanderEntries) {
		entry->tick(dt);
		entry->m_parentAlpha = m_alpha;
	}

	int mouseCode = m_pUserIO->getMouseKeyCode(0);

	if (m_pUserIO->isKeyReleased(mouseCode))
	{
		m_dragActive = false;
	}

	if (troopTabEnabled())
	{
		bg.update(0);

		if(hasFocus() && isMouseOver())
		{
			if (m_pUserIO->isKeyPressed(mouseCode))
			{
				float selectionSqrDist = 0.05f * 0.05f;

				m_dragActive = true;
				mousePos = m_pUserIO->getCursorPosition();
				mousePos.y /= m_pWindow->getAspectRatio();
				auto result = findNearest(mousePos.x, mousePos.y, selectionSqrDist);

				if (result.distanceSquared < selectionSqrDist) {
					// select (if already selected, drag).
					if (result.squadIndex == -1) {
						if (!m_commandersTab.isSelectedId(result.commanderIndex)) {
							m_commandersTab.unselectAll();
							m_commandersTab.select(result.commanderIndex);
							updateCommanderEntries();
						}

						auto selected = m_commandersTab.selectedCommandersIds();
						commanderOffsets.resize(selected.size());

						// Note: This could be optimized.
						for (uint32_t i = 0; i < selected.size(); ++i) {
							uint32_t commanderId = selected[i];
							for (uint32_t k = i; k < m_province.commanders.size(); ++k) {
								auto& commander = m_province.commanders[k];
								if (commander.id == commanderId) {
									commanderOffsets[i] = commander.combatOrder.startPos - uiPosToStart(mousePos);
									break;
								}
							}
						}
					}
					else {
						// squad selection?
						// TODO:
						// Plan: Squads can not be multiselected.
						//       Squads always move in relation with their commander.
					}
				}
				else {
					// unselect
					m_commandersTab.unselectAll();
					commanderOffsets.clear();
					updateCommanderEntries();
				}
			}
			else if (m_dragActive && m_pUserIO->isKeyDown(mouseCode)) {
				// continue drag.
				auto nextPos = m_pUserIO->getCursorPosition();
				nextPos.y /= m_pWindow->getAspectRatio();

				auto selected = m_commandersTab.selectedCommandersIds();
				for (size_t i = 0; i < selected.size(); ++i)
				{
					for (size_t k = i; k < m_province.commanders.size(); ++k)
					{
						if (m_province.commanders[k].id == selected[i])
						{
							m_province.commanders[k].combatOrder.startPos = wrapToBattleArea(
								commanderOffsets[i] + uiPosToStart(nextPos)
							);
							break;
						}
					}
				}
			}

			// handle input
		}
	}
}

void TroopsTab::toggle()
{
	m_targetAlpha = 1 - m_targetAlpha;
}

bool TroopsTab::troopTabEnabled() const
{
	return m_alpha > 0.001f;
}

void TroopsTab::updateCommanderEntries()
{
	// CommanderEntry(TroopsTab* parent, sa::MenuComponent* alignUnder, BattleCommander* commander);
	commanderEntries.clear();
	sa::MenuComponent* alignUnder = &battleArea;

	auto selected = m_commandersTab.selectedCommandersIndices();
	if (!selected.empty())
	{
		// add the remaining selected commanders
		for (size_t i = 0; i < selected.size(); ++i) {
			commanderEntries.emplace_back(
				std::make_unique<CommanderEntry>(
					this,
					alignUnder,
					&m_province.commanders[selected[i]],
					m_hider
				)
			);
			alignUnder = commanderEntries.back().get();
		}

		// add all non-selected commanders.
		size_t activeIndex = 0;
		for (size_t i = 0; i < m_province.commanders.size(); ++i) {
			if (m_faction.playerIndex == m_province.commanders[i].owner) {
				if (activeIndex < selected.size() && i == selected[activeIndex]) {
					++activeIndex;
				}
				else {
					commanderEntries.emplace_back(
						std::make_unique<CommanderEntry>(
							this,
							commanderEntries.back().get(),
							&m_province.commanders[i],
							m_hider
						)
					);
				}
			}
		}
	}
	else
	{
		// add all non-selected commanders
		for (size_t i = 0; i < m_province.commanders.size(); ++i) {
			if (m_faction.playerIndex == m_province.commanders[i].owner) {
				commanderEntries.emplace_back(
					std::make_unique<CommanderEntry>(
						this,
						alignUnder,
						&m_province.commanders[i],
						m_hider
					)
				);
				alignUnder = static_cast<sa::MenuComponent*>(commanderEntries.back().get());
			}
		}
	}
}

void TroopsTab::draw(std::shared_ptr<sa::Graphics> graphics) const
{
	if (troopTabEnabled())
	{
		bg.visualise(graphics);

		{
			// draw "battle area"
			sa::Matrix4 model;
			auto pos = battleArea.getPosition();
			pos.y *= m_pWindow->getAspectRatio();
			model.makeTranslationMatrix(pos);
			model.scale(battleArea.getScale());
			graphics->m_pRenderer->drawRectangle(model, "Empty", sa::vec4<float>(1, 1, 1, m_alpha));
		}

		sa::vec3<float> areaScale = battleAreaScale();
		sa::vec3<float> areaPos = battleAreaPos();

		auto selectedCommanders = m_commandersTab.selectedCommandersIds();
		size_t index = 0;

		// draw commanders and squads.
		for (size_t i = 0; i < m_province.commanders.size(); ++i) {
			const auto& commander = m_province.commanders[i];
			sa::vec2<int> commanderPosInt = commander.combatOrder.startPos;
			sa::vec3<float> commanderPos(
				areaScale.x * (commanderPosInt.x / 100.0f),
				areaScale.y * (commanderPosInt.y / 100.0f),
				0
			);
			
			float commanderAlpha = 0.7f;
			float commanderScale = 0.02f;
			if (index < selectedCommanders.size() && selectedCommanders[index] == commander.id) {
				++index;
				commanderAlpha = 1.0f;
				commanderScale = 0.022f;
			}

			sa::Matrix4 commanderModel;
			commanderModel.makeTranslationMatrix(areaPos + commanderPos);
			commanderModel.scale(commanderScale, commanderScale, 0);
			graphics->m_pRenderer->drawRectangle(
				commanderModel,
				commander.reference->icon,
				sa::vec4<float>(1, 1, 1, m_alpha * commanderAlpha)
			);
		}

		for (const auto& entry : commanderEntries)
			entry->visualise(graphics);
	}
}

void TroopsTab::hide()
{
	m_targetAlpha = 0;
}
