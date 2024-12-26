#pragma once

#include "./features/Misc.hpp"
#include <algorithm>
#include "BetaAA.h"
namespace ShowSpectators
{
	std::list<int> GetObservervators(int playerId);
	void RenderWindow();
}