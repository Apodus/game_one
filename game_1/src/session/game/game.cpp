#include "game.hpp"

#include "BattleSim/BattleSim.hpp"
#include "menu/menuroot.hpp"

Game::Game(
	std::shared_ptr<sa::MenuRoot> menuRootNode,
	std::shared_ptr<sa::UserIO>)
	: nextUnitId(1)
	, menuRootNode(menuRootNode)
{
	hud = std::make_shared<Hud>(*this, menuRootNode.get(), "GameHud", sa::vec3<float>(), sa::vec3<float>(1, 1, 1));
	menuRootNode->addChild(hud);

	cameraPosition = sa::vec3<float>(5, -5, 20);
	targetCameraPosition = sa::vec3<float>(-5, +5, 20);

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
		for(int i=0; i<10; ++i)
			province.troopsToRecruit.emplace_back(militia.name);
	}

	auto& first = provinces.front();
	first.m_owner = 0;
	for(int i =0; i<23; ++i)
		first.commanders.emplace_back(troopReferences["zealot"], ++nextUnitId, first.m_owner);

	auto& second = provinces.back();
	second.m_owner = 1;
	for (int i = 0; i<23; ++i)
		second.commanders.emplace_back(troopReferences["zealot"], ++nextUnitId, second.m_owner);
	
	players.emplace_back("Mestari");
	players.emplace_back("Nubu");
}

Game::~Game()
{

}


void Game::draw(std::shared_ptr<sa::Graphics> pGraphics)
{
	if (m_sim == nullptr)
	{
		drawProvinces(pGraphics);
	}
	else
	{
		drawBattle(pGraphics);
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
			if (commander.myOrder.orderType == BattleCommander::OrderType::Move)
			{
				const auto& targetProvince = provinces[commander.myOrder.moveTo];
				auto source = province.m_position;
				auto target = targetProvince.m_position;

				auto direction = target - source;
				direction *= 0.8f;

				sa::vec4<float> arrowColor = Color::RED;
				arrowColor.a = 0.7f;

				if (commander.owner == targetProvince.m_owner)
					arrowColor = Color::GREEN;

				float arrowDensity = 0.075f;
				if (commander.m_selected)
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
				auto arrowPoint1 = sa::math::rotatedXY(reverseDirection, +3.1459f / 6.0f).normalize() * 0.6f;
				auto arrowPoint2 = sa::math::rotatedXY(reverseDirection, -3.1459f / 6.0f).normalize() * 0.6f;

				pGraphics->m_pRenderer->drawLine(
					source + direction,
					source + direction + arrowPoint1,
					arrowDensity,
					arrowColor
				);

				pGraphics->m_pRenderer->drawLine(
					source + direction,
					source + direction + arrowPoint2,
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

	// visualize mouse cursor
	sa::Matrix4 model;
	auto worldPos = mouseToWorld(mousePos);
	model.makeTranslationMatrix(worldPos.x, worldPos.y, 0);
	model.rotate(0, 0, 0, 1);
	model.scale(0.1f, 0.1f, 1);
	pGraphics->m_pRenderer->drawRectangle(model, "Hero");
}

void Game::drawBattle(std::shared_ptr<sa::Graphics> pGraphics)
{
	sa::TextureHandler::getSingleton().bindTexture(0, "Empty");

	float offsetX = 30.0f;
	float offsetY = 20.0f;
	float scale = 1/4.0f;
	
	float unitHeight = 1.5f;

	while (m_simAccu >= m_sim->GetTimePerUpdate())
	{
		const auto* frame = m_sim->GetField().GetFrame();
		if (frame)
		{
			bs::UpdateData::Reader reader = frame->GetReader();
			size_t numUpdates = reader.Read<size_t>();
			m_units.resize(numUpdates);
			for (size_t i = 0; i < numUpdates; i++)
			{
				const auto& unitIn = reader.Read<bs::Field::Frame::Elem>();
				auto& unit = m_units[i];
				unit.current = unit.next;
				unit.next.isValid = true;
				unit.next.x = unitIn.pos.x.toFloat() * scale - offsetX;
				unit.next.y = unitIn.pos.y.toFloat() * scale - offsetY;
				unit.size = unitIn.radius.toFloat() * scale;
				unit.hitpoints = unitIn.hitpoints;
				unit.team = unitIn.team;
			}
			m_sim->GetField().FreeFrame();
			m_simAccu -= m_sim->GetTimePerUpdate();
		}
		else
		{
			break;
		}
	}

	float frameFraction = min(1.0f, static_cast<float>(m_simAccu / m_sim->GetTimePerUpdate()));

	for (size_t i = 0; i < m_units.size(); i++)
	{
		auto& unit = m_units[i];
		sa::Matrix4 model;
		float x, y;
		if (unit.next.isValid)
		{
			if (unit.current.isValid)
			{
				x = unit.current.x + ((unit.next.x - unit.current.x) * frameFraction);
				y = unit.current.y + ((unit.next.y - unit.current.y) * frameFraction);
			}
			else
			{
				continue; // Not yet visible
			}
		}
		else
		{
			// ASSERT(unit.current.isValid, "At least one frame must be valid");
			x = unit.current.x;
			y = unit.current.y;
		}

		model.makeTranslationMatrix(x, y, unit.hitpoints == 0 ? 0.0f : unitHeight * scale);
		model.rotate(0, 0, 0, 1);
		model.scale(unit.size, unit.size, 1);

		pGraphics->m_pRenderer->drawRectangle(model, "Hero",
			unit.hitpoints == 0 ? Color::RED :
			(unit.team == 1 ? Color::GREEN : Color::BLUE));		
	}
}

void Game::tick(long long timeMs)
{
	cameraPosition += (targetCameraPosition - cameraPosition) * 100.0f / 1000.0f;

	if (m_sim)
	{
		auto delta = m_lastSimUpdate != 0 ? timeMs - m_lastSimUpdate : 0;
		m_sim->Simulate(static_cast<size_t>(delta));
		m_simAccu += static_cast<double>(delta) / 1000.0;
		m_lastSimUpdate = timeMs;
	}
	++m_tickID;
}

void Game::showBattle()
{
	m_sim = std::make_unique<bs::BattleSim>();
	m_sim->TestSetup();
	m_lastSimUpdate = 0;
	m_simAccu = 0;
}