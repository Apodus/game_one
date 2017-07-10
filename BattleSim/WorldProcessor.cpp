#include "stdafx.h"
#include "WorldProcessor.h"
#include "FixedMath.h"

void bs::WorldProcessor::Run()
{
	myMoveTargetDirs.x[0] = myMoveTargets.x[0] - myPositions.x[0];
	myMoveTargetDirs.y[0] = myMoveTargets.y[0] - myPositions.y[0];
	myMoveTargetDirs.Normalize();

	myAimTargetDirs.x[0] = myAimTargets.x[0] - myPositions.x[0];
	myAimTargetDirs.y[0] = myAimTargets.y[0] - myPositions.y[0];
	myAimTargetDirs.Normalize();
}