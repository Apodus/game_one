
#pragma once

#include <cinttypes>

enum class OrderType
{
	Idle,
	Move,
	Patrol, // battle outside of fortifications if attacked. pacify region.
	RunTests, // discover natural resources
	Research, // discover tech stuff for scientists to perform
	CastSpell // perform miracles of science
};

struct StrategyViewOrder
{
	OrderType orderType = OrderType::Idle;
	size_t moveTo = 0;
	size_t spellToCast = 0;
};