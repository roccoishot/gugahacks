#pragma once

#include "..//options.hpp"
#include "..//valve_sdk/csgostructs.hpp"

class CLegitbot
{
public:
	CLegitbot()
	{
		current_punch = last_punch = { 0, 0, 0 };
		target = nullptr;

		lastShotTick = shotsFired = 0;
		
		shot_delay_time = kill_delay_time = 0;
		shot_delay = kill_delay = false;
	}

	bool ran = false;
	void Run(CUserCmd* cmd);
	bool IsEnabled(CUserCmd* cmd);
	float GetFovToPlayer(QAngle viewAngle, QAngle aimAngle);
	C_BasePlayer* target = nullptr;
	int hitboxforbt = -1;
	float GetFov();

private:
	void RCS(QAngle& angle, C_BasePlayer* target);
	bool IsLineGoesThroughSmoke(Vector vStartPos, Vector vEndPos);
	void Smooth(QAngle currentAngle, QAngle aimAngle, QAngle& angle);
	bool IsSilent();
	C_BasePlayer* GetClosestPlayer(CUserCmd* cmd, int& bestBone, float& fov, QAngle& angles);

	int lastShotTick;
	int shotsFired;

	bool shot_delay = false;
	int shot_delay_time = 0;

	bool kill_delay = false;
	int kill_delay_time = 0;

	QAngle current_punch = { 0, 0, 0 };
	QAngle last_punch = { 0, 0, 0 };
};

inline CLegitbot* g_Legitbot;