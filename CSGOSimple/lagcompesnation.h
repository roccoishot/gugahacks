#pragma once
#include "./features/Misc.hpp"
#include <algorithm>
#include "BetaAA.h"
#include <vector>

using namespace std;

class LagRecord
{
public:
	AnimationLayer server_layers[15];
	AnimationLayer resolver_layers[3][15];
	float lby = 0;
	int simtime, oldsimtime = 0;
	Vector origin;
};

class LagComp : public Singleton<LagComp> {
public:
	vector<LagRecord> records[65];
	void Run();
	void LagCompensate(C_BasePlayer* player, LagRecord record);
};

class resolver : public Singleton<resolver>
{
public: // puberty
	void Resolve(C_BasePlayer* player, LagRecord record);
};