#pragma once

#include "Types.h"
#include "IdPool.h"
#include "Array.hpp"

#include "FixedVec.h"

namespace bs
{
	class WorldProcessor
	{
		static const size_t MaxElements = 4;

	public:
		WorldProcessor() {}
		~WorldProcessor() {}

		void Run();

		struct Vec2Group
		{
			Array<core::FixedVec32, MaxElements/4> x;
			Array<core::FixedVec32, MaxElements/4> y;
			void Store(Vec& pos0, Vec& pos1, Vec& pos2, Vec& pos3)
			{
				x[0] = core::FixedVec32(pos0.x, pos1.x, pos2.x, pos3.x);
				y[0] = core::FixedVec32(pos0.y, pos1.y, pos2.y, pos3.y);
			}
			void Load(Vec* pos)
			{
				x[0].Load(pos[0].x, pos[1].x, pos[2].x, pos[3].x);
				y[0].Load(pos[0].y, pos[1].y, pos[2].y, pos[3].y);
			}

			core::FixedVec32 Length() const { return x[0] * x[0] + y[0] * y[0]; }

			void Normalize()
			{
				auto sum = Length();

				// sqrt(x*x + y*y)
				auto currentVal = sum;
				currentVal = currentVal * core::Fraction32(1, 2);
				for (int i = 0; i < 10; ++i)
				{
					currentVal = currentVal + (sum / (currentVal));
					currentVal = currentVal * core::Fraction32(1, 2);
				}
				x[0] = x[0] / currentVal;
				y[0] = y[0] / currentVal;
			}
		};

		// Inputs
		__declspec(align(16)) Vec2Group myPositions;
		__declspec(align(16)) Vec2Group myMoveTargets;
		__declspec(align(16)) Vec2Group myAimTargets;

		// Output
		__declspec(align(16)) Vec2Group myMoveTargetDirs;
		__declspec(align(16)) Vec2Group myAimTargetDirs;
	};
}