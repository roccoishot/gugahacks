#pragma once

#include "../options.hpp"
#include "../valve_sdk/csgostructs.hpp"

struct target {
	int damage;
	C_BasePlayer* ent;
	Vector pos;
};

struct rage {

	void ragebot( CUserCmd* cmd );
	target get_target( );
	void autostop( CUserCmd* cmd );
	bool hitchance( C_BasePlayer* pEnt, C_BaseCombatWeapon* pWeapon, QAngle Angle, int chance );
	int get_hitchance( );
	int get_mid_dmg( );

};

inline rage g_rage;