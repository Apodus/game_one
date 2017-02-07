
#include "game.hpp"

#include "menu/menuroot.hpp"
#include "session/game/worldmap/Terrain.hpp"

#include "session/game/Combat.hpp"

Game::Game(
	std::shared_ptr<sa::MenuRoot> menuRootNode,
	std::shared_ptr<sa::UserIO>)
	: nextUnitId(1)
	, menuRootNode(menuRootNode)
	, m_activeCamera(&m_provinceCamera)
{
	hud = std::make_shared<Hud>(*this, menuRootNode.get(), "GameHud", sa::vec3<float>(), sa::vec3<float>(1, 1, 1));
	menuRootNode->addChild(hud);

	m_provinceCamera.pos = sa::vec3<float>(5, -5, 20);
	m_provinceCamera.target = sa::vec3<float>(-5, +5, 20);

	TroopReference militia;
	militia.accuracy = 10;
	militia.armor = 2;
	militia.attack = 8;
	militia.defence = 8;
	militia.fitness = 1;
	militia.hp = 8;
	militia.leadership = 0;
	militia.name = "militia";
	militia.icon = "Hero";
	militia.strength = 8;
	militia.strategyMovement = 4;
	militia.terrainFast |= Terrain::Fields | Terrain::Woods;
	
	TroopReference zealot;
	zealot.accuracy = 10;
	zealot.armor = 5;
	zealot.attack = 11;
	zealot.defence = 11;
	zealot.fitness = 3;
	zealot.hp = 10;
	zealot.leadership = 30; // if promoted to commander by some means, this would take effect.
	zealot.name = "zealot";
	zealot.icon = "Hero";
	zealot.strength = 10;
	militia.strategyMovement = 6;
	zealot.terrainFast |= Terrain::Fields | Terrain::Woods;

	TroopReference marksman;
	marksman.accuracy = 13;
	marksman.armor = 1;
	marksman.attack = 8;
	marksman.defence = 8;
	marksman.fitness = 3;
	marksman.hp = 10;
	marksman.leadership = 10;
	marksman.name = "marksman";
	marksman.icon = "Hero";
	marksman.strength = 9;
	militia.strategyMovement = 5;
	marksman.terrainFast |= Terrain::Fields | Terrain::Woods;

	TroopReference rider;
	rider.accuracy = 10;
	rider.armor = 3;
	rider.attack = 12;
	rider.defence = 9;
	rider.fitness = 3;
	rider.hp = 10;
	rider.leadership = 30;
	rider.name = "rider";
	rider.icon = "Hero";
	rider.strength = 10;
	rider.strategyMovement = 8;
	rider.terrainFast |= Terrain::Fields | Terrain::Hills;

	troopReferences.insert(std::make_pair(militia.name, militia));
	troopReferences.insert(std::make_pair(zealot.name, zealot));
	troopReferences.insert(std::make_pair(marksman.name, marksman));
	troopReferences.insert(std::make_pair(rider.name, rider));

#ifdef _DEBUG
	graph.random(5);
#else
	graph.random();
#endif

	auto& provinces = graph.provinces();

	for (auto& province : provinces)
	{
		for (int i = 0; i < 10; ++i)
			province.troopsToRecruit.emplace_back(militia.name);
	}



	auto& first = provinces.front();
	first.m_owner = 0;
	for (int i = 0; i < 23; ++i)
		first.commanders.emplace_back(troopReferences["rider"], ++nextUnitId, first.m_owner);

	auto& second = provinces.back();
	second.m_owner = 1;
	for (int i = 0; i < 23; ++i)
		second.commanders.emplace_back(troopReferences["rider"], ++nextUnitId, second.m_owner);

	for (auto& province : provinces)
	{
		uint32_t plop = sa::math::rand(74892u);
		if (province.m_owner == ~size_t(0))
		{
			province.commanders.emplace_back(troopReferences["zealot"], ++nextUnitId, ~size_t(0));
			province.commanders.emplace_back(troopReferences["rider"], ++nextUnitId, ~size_t(0));
			
			plop = sa::math::rand(plop);
			for (uint32_t i = 0; i < plop % 10; ++i)
			{
				province.units.emplace_back(troopReferences["marksman"], ++nextUnitId, ~size_t(0));
				province.units.emplace_back(troopReferences["militia"], ++nextUnitId, ~size_t(0));
			}
		}
	}

	players.emplace_back("Mestari", 0);
	players.emplace_back("Nubu", 1);
}

Game::~Game()
{

}


void Game::draw(std::shared_ptr<sa::Graphics> pGraphics)
{
	if (!m_showCombat)
	{
		drawProvinces(pGraphics);
	}
	else
	{
		m_combatView.draw(pGraphics);
	}
}


void Game::drawProvinces(std::shared_ptr<sa::Graphics> pGraphics)
{
	const auto& provinces = graph.provinces();

	sa::TextureHandler::getSingleton().bindTexture(0, "Empty");

	// draw connections between provinces
	for (const auto& province : provinces)
	{
		for (auto connectionIndex : province.m_connections)
		{
			pGraphics->m_pRenderer->drawLine(
				province.m_position,
				provinces[connectionIndex].m_position,
				0.05f,
				Color::GREEN
			);
		}
	}

	// for animation of things.
	float modifier = sa::math::sin(2 * 3.1459f * (m_tickID % 60) / 60.0f);

	// visualize movement orders
	for (const auto& province : provinces)
	{
		for (const auto& commander : province.commanders)
		{
			if (commander.strategyOrder.orderType == OrderType::Move)
			{
				const auto& targetProvince = provinces[commander.strategyOrder.moveTo];
				auto source = province.m_position;
				auto target = targetProvince.m_position;

				auto direction = target - source;
				direction *= 0.8f;

				sa::vec4<float> arrowColor = Color::RED;
				arrowColor.a = 0.7f;

				if (commander.owner == targetProvince.m_owner)
					arrowColor = Color::GREEN;

				float arrowDensity = 0.075f;
				if (hud->isCommanderSelected(commander.id))
				{
					direction *= 1.0f + 0.1f * modifier;
					arrowDensity *= 1.0f + 0.2f * modifier;
					arrowColor.a = 1.0f;
				}

				pGraphics->m_pRenderer->drawLine(
					source,
					source + direction,
					arrowDensity,
					arrowColor
				);

				auto reverseDirection = source - target;
				auto arrowPoint1 = sa::math::rotatedXY(reverseDirection, +3.1459f / 4.0f).normalize();
				auto arrowPoint2 = sa::math::rotatedXY(reverseDirection, -3.1459f / 4.0f).normalize();

				pGraphics->m_pRenderer->drawLine(
					source + direction - arrowPoint1 * arrowDensity,
					source + direction + arrowPoint1 * 0.6f,
					arrowDensity,
					arrowColor
				);

				pGraphics->m_pRenderer->drawLine(
					source + direction - arrowPoint2 * arrowDensity,
					source + direction + arrowPoint2 * 0.6f,
					arrowDensity,
					arrowColor
				);
			}
		}
	}

	// draw something to visualize provinces
	for (const auto& province : provinces)
	{
		float scale = 0.4f * province.area() * province.scale() / 100.f;
		sa::vec4<float> color(0.3f, 0.3f, 0.3f, province.alpha());

		if (&province == hud->getActiveProvince())
		{
			scale *= 1.0f + 0.2f * modifier;
			color.a = 1.0f;
		}

		sa::Matrix4 model;
		model.makeTranslationMatrix(province.m_position.x, province.m_position.y, 0);
		model.rotate(0, 0, 0, 1);
		model.scale(scale, scale, 1);

		if (province.m_owner == 0)
		{
			color.r = 1;
			color.g = 0;
			color.b = 0;
		}
		if (province.m_owner == 1)
		{
			color.r = 1;
			color.g = 1;
			color.b = 1;
		}
		pGraphics->m_pRenderer->drawRectangle(model, "Frame", color);
	}

	// visualize selected units' movement options
	{
		const auto& movementPossibilities = hud->movementOptions();
		for (size_t i = 0; i < movementPossibilities.size(); ++i)
		{
			int value = movementPossibilities[i];
			if (value < 4 && value > 0)
			{
				sa::vec4<float> color = Color::GREEN;
				if (value == 2)
					color = Color::YELLOW;
				else if (value == 3)
					color = Color::ORANGE;
				color.a = 0.6f;

				sa::Matrix4 model;
				auto pos = graph.provinces()[i].m_position;
				float s = graph.provinces()[i].scale();
				model.makeTranslationMatrix(pos.x, pos.y, 0);
				model.rotate(2.0f * 3.1459f * (m_tickID % 60) / 60.0f, 0, 0, 1);
				model.scale(s, s, s);
				pGraphics->m_pRenderer->drawRectangle(model, "Swirl", color);

				s *= 0.8f;
				color.a = 0.2f;

				model.makeTranslationMatrix(pos.x, pos.y, 0);
				model.rotate(-2.0f * 3.1459f * (m_tickID % 40) / 40.0f, 0, 0, 1);
				model.scale(s, s, s);
				pGraphics->m_pRenderer->drawRectangle(model, "Swirl", color);
			}
		}
	}

	// visualize mouse cursor
	sa::Matrix4 model;
	auto worldPos = mouseToWorld(mousePos);
	model.makeTranslationMatrix(worldPos.x, worldPos.y, 0);
	model.rotate(0, 0, 0, 1);
	model.scale(0.1f, 0.1f, 1);
	pGraphics->m_pRenderer->drawRectangle(model, "Hero");
}

void Game::tick(long long timeMs)
{
	m_combatView.tick(timeMs);
	m_activeCamera->tick(timeMs);
	++m_tickID;
}

void Game::toggleBattle()
{
	if (!m_showCombat)
	{
		m_combatView.start();
		m_showCombat = true;
		m_activeCamera = &m_combatView.getCamera();
	}
	else
	{
		m_combatView.stop();
		m_showCombat = false;
		m_activeCamera = &m_provinceCamera;
	}
}

void Game::resolveCombat(size_t provinceIndex)
{
	auto& provinces = graph.provinces();
	auto& commanders = provinces[provinceIndex].commanders;

	Combat combat;
	combat.add(commanders);
	// TODO add troops

	if (combat.factionCount() > 1)
	{
		combat.resolve();

		// just copying combat for everyone... 
		// TODO: filter it to relevat parties
		for (auto&& player : players)
		{
			player.turn->m_combats.emplace_back(combat);
		}

		// TODO: REMOVE: Here just to show latest combat in view.
		m_combatView.setCombat(combat);

		// Handle result
		std::vector<size_t> killed;
		for (size_t i = 0; i < commanders.size(); i++)
		{
			if (combat.getPostBattleCommander(i).hitpoints == 0)
			{
				killed.emplace_back(i);
			}
		}
		for (auto iter = killed.rbegin(); iter != killed.rend(); ++iter)
		{
			commanders.erase(commanders.begin() + *iter);
		}
	}
	else
	{
		LOG("Combat skipped - not enough factions!");
	}

	// TODO: Combat should provide winner player ID
	// If there's a scout in the province who didn't fight, then commander owner is not reliable.
	if (!provinces[provinceIndex].commanders.empty())
	{
		provinces[provinceIndex].m_owner = provinces[provinceIndex].commanders.front().owner;
	}
}
