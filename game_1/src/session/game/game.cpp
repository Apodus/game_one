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

	TroopReference militia;
	militia.accuracy = 10;
	militia.armor = 2;
	militia.attack = 8;
	militia.defence = 8;
	militia.fitness = 1;
	militia.hp = 8;
	militia.leadership = 0;
	militia.name = "militia";
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
		province.troopsToRecruit.emplace_back(militia.name);
	}

	provinces.front().m_owner = 0;
	provinces.back().m_owner = 1;

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

	for (const auto& province : provinces)
	{
		for (auto connectionIndex : province.m_connections)
		{
			pGraphics->m_pRenderer->drawLine(
				province.m_position,
				provinces[connectionIndex].m_position,
				0.1f,
				Color::GREEN
			);
		}
	}

	for (const auto& province : provinces)
	{
		float scale = 0.4f * province.area() / 100.f;

		sa::Matrix4 model;
		model.makeTranslationMatrix(province.m_position.x, province.m_position.y, 0);
		model.rotate(0, 0, 0, 1);
		model.scale(scale, scale, 1);

		sa::vec4<float> color(0.3f, 0.3f, 0.3f, 1);
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

	sa::Matrix4 model;
	model.makeTranslationMatrix(mousePos.x * 20 * 0.985f, mousePos.y * 20 * 0.985f / aspectRatio, 0);
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

	auto& frame = m_sim->GetField().GetFrame();
	for (size_t i = 0; i < frame.units.size(); i++)
	{
		auto& unit = frame.units[i];
		float x = unit.pos.x.toFloat() * scale - offsetX;
		float y = unit.pos.y.toFloat() * scale - offsetY;


		float size = unit.radius.toFloat() * scale;

		sa::Matrix4 model;
		model.makeTranslationMatrix(x, y, unit.hitpoints == 0 ? -1.0f : 0);
		model.rotate(0, 0, 0, 1);
		model.scale(size, size, 1);

		pGraphics->m_pRenderer->drawRectangle(model, "Hero", 
			unit.hitpoints == 0 ? Color::RED :			 
			(unit.team == 1 ? Color::GREEN : Color::BLUE));
	}
}

void Game::tick(long long timeMs)
{
	if (m_sim)
	{
		auto delta = m_lastSimUpdate != 0 ? timeMs - m_lastSimUpdate : 0;
		m_sim->Simulate(static_cast<size_t>(delta));
		m_lastSimUpdate = timeMs;
	}
}

void Game::showBattle()
{
	m_sim = std::make_unique<bs::BattleSim>();
	m_sim->TestSetup();
	m_lastSimUpdate = 0;
}