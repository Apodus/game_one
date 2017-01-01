#pragma once

#include "Level.hpp"
#include "Array.hpp"

namespace bs
{
	class Field
	{

	private:
		// Field is divided into levels in depth-axis. 
		// E.g. level 0 has underground units, level 1 has ground units and level 2 has flying units
		Array<Level, 1> myLevels;
	};
}