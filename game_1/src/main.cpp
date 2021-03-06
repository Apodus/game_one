
#include "thread/TaskManager.hpp"
#include "util/attributes.hpp"
#include "session/application/session.hpp"
#include "graphics/opengl/opengl.hpp"

#include <iostream>
#include <vector>
#include <random>

#include <SFML/Audio.hpp>
#include "script/Scripter.hpp"

#include "BattleSim/BattleSim.hpp"

int main(int argc, char** argv)
{
    /*
    sf::Music music;
	if(!music.openFromFile(filename))
	{
		LOG("Unable to open music file '%s'", filename.c_str());
	}
	else
	{
		music.play();
		LOG("Playing music file '%s'", filename.c_str());
	}
    */

	try
	{
	
		// Temporary for testing battle sim
		if (argc >= 2 && strcmp(argv[1], "sim") == 0)
		{
			auto battle = bs::BattleSim::Generate();

			LOG("Running test");
			auto startTime = std::chrono::high_resolution_clock::now();
			bs::BattleSim* sim = new bs::BattleSim(battle, bs::Field::StreamingMode::Disabled);
			sim->Resolve();
			size_t checksum = 0;
			for (size_t i = 0; i < battle.NumUnits(); i++)
			{
				checksum = (checksum + 1) * (battle.Get(i).hitpoints + 1);
			}
			auto ticks = sim->GetField().GetTick() + 1;
			delete sim;
			auto endTime = std::chrono::high_resolution_clock::now();
			using ms = std::chrono::duration<float, std::milli>;
			auto deltaTime = static_cast<long long>(std::chrono::duration_cast<ms>(endTime - startTime).count());
			auto deltaSeconds = static_cast<double>(deltaTime) / 1000;
			LOG("Total sim time: %f seconds, %f sec/tick (checksum=%zu;ticks=%zu)",
				deltaSeconds, deltaSeconds / ticks, checksum, ticks);
			return EXIT_SUCCESS;
		}

		auto session = std::make_unique<Session::Application::Session>();

		while (session->tick());

        /*
		thread::TaskMaster taskMaster;
		thread::TaskMaster::Task task1("test1", [](){std::cout << "func1" << std::endl; });
		thread::TaskMaster::Task task2("test2", [](){std::cout << "func2" << std::endl; });

		task1.requireWrite("physics");
		task1.requireRead("physics");

		taskMaster.addTask(task1);
		taskMaster.addTask(task2);

		taskMaster.tick();

		PREFETCH_READ(&taskMaster);

		sa::MeshHandler meshHandler;
		meshHandler.importModelsFromScene("../models/cube.dae");
		// auto cubeMesh = meshHandler["Cube"];
        */
	}
	catch(std::exception& e)
	{
		std::cerr << "Uncaught exception: " << e.what() << '\n';
	}
	catch(...)
	{
		std::cerr << "Uncaught unknown exception\n";
	}

	return EXIT_SUCCESS;
}