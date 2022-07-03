#pragma once
#include "./valve_sdk/csgostructs.hpp"
#include "./helpers/math.hpp"
#include "./options.hpp"
#include <d3dx9math.h>
class C_BasePlayer;
class CUserCmd;

class GugerKiller : public  Singleton< GugerKiller > {
public:
	void Sex();
};