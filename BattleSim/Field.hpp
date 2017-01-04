#pragma once

#include "Level.hpp"
#include "Array.hpp"



namespace bs
{
	class Unit;
	class Field
	{
	public:
		Unit::Id Add(Unit& unit);

		double RToF(const Real& real);


		void Update();

		static const Real TimePerUpdate;

	private:
		// Field is divided into levels in depth-axis. 
		// E.g. level 0 has underground units, level 1 has ground units and level 2 has flying units
		Array<Level, 1> myLevels;
		Vector<Unit::Id> myActiveUnits;
		Vector<Unit> myUnits;

	};
}