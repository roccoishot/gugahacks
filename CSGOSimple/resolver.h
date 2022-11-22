#pragma once
#include "./features/Misc.hpp"
#include <algorithm>
#include "BetaAA.h"
#include <vector>

using namespace std;

class resolver : public Singleton<resolver>
{
public: // puberty
	void Resolve(CUserCmd* cmd);
	void PrintInChat(int Value, bool candesync);
	int shotsfired = 0;
	int side = 0;
	int ohyeahbabythatsthespot = 0;
	bool resetted = false;
	bool sexdickballs = false;
	bool tryagain = true;
	bool go = false;
	bool tryagain2 = true;
	bool candesync = false;
};