#include "resolver.h"
#include "features/aimbot.hpp"
#include "Globals.h"
#include "menu.hpp"
#include "./helpers/math.hpp"
#include "features/aimbot.hpp"

void resolver::PrintInChat(int Value, bool candesync)
{
	const char* state;

	if (candesync == false || Value == 0)
		state = "Can't Desync";

	//Prints the logs yessir
	if (g_Options.aimbot.printresolver) {
		if (Globals::killedplayer) {
			if (shotsfired < 4) {
				if (Value > 0 && candesync == true) {
					g_ChatElement->ChatPrintf(0, 0, (std::string(
						" \x04"
						"GUGAHACKS.KITCHEN"
						" \x01"
						"| Target Desync Degree: [")
						+ std::to_string(Value)
						+ std::string("]")).c_str());
				}
				else if (candesync == false || Value == 0) {
					g_ChatElement->ChatPrintf(0, 0, (std::string(
						" \x04"
						"GUGAHACKS.KITCHEN"
						" \x01"
						"| Target State: [")
						+ std::string(state)
						+ std::string("]")).c_str());
				}
			}
			else if (shotsfired >= 4)
			{
				if (Value > 0 && candesync == true) {
					g_ChatElement->ChatPrintf(0, 0, (std::string(
						" \x04"
						"GUGAHACKS.KITCHEN"
						" \x01"
						"| Target Desync Degree: [")
						+ std::string("Next LBY")
						+ std::string("]")).c_str());
				}
				else if (candesync == false || Value == 0) {
					g_ChatElement->ChatPrintf(0, 0, (std::string(
						" \x04"
						"GUGAHACKS.KITCHEN"
						" \x01"
						"| Target State: [")
						+ std::string(state)
						+ std::string("]")).c_str());
				}
			}
		}
	}

}

void resolver::Resolve(CUserCmd* cmd)
{
	if (!g_EngineClient->IsInGame() || !g_LocalPlayer || !g_LocalPlayer->IsAlive())
	{
		//Resets
		ohyeahbabythatsthespot = 0;
		Globals::hitplayer = false;
		Globals::killedplayer = false;
		tryagain = false;
		tryagain2 = false;
		resetted = false;
	}

	if (g_EngineClient->IsInGame()) {

		if (g_LocalPlayer && g_LocalPlayer->IsAlive()){
		bool ibeshootin = (g_LocalPlayer->m_iShotsFired() >= 1 || cmd->buttons & IN_ATTACK);

		if (ibeshootin)
		{
			//Starts
			shotsfired++;
			tryagain = true;
			tryagain2 = true;
		}

		if (tryagain == false && go == true)
		{
			//Resets when it stops
			shotsfired = 0;
			tryagain = true;
		}

		if (tryagain2 == false)
		{
			tryagain2 = true;
		}

		for (int i = 1; i < g_EngineClient->GetMaxClients(); i++)
		{
			auto player = C_BasePlayer::GetPlayerByIndex(i);

			if (g_Options.aimbot.enabled && CLegitbot::CLegitbot().target != nullptr)
				player = CLegitbot::CLegitbot().target;
			else
				player = C_BasePlayer::GetPlayerByIndex(i);

			if (!player) continue;
			if (!player->valid(true, true)) continue;

			if (player && player->valid(true, true)) {

				//Some bools for desync checks
				bool onground = player->m_fFlags() & FL_ONGROUND;
				bool fastasf = player->m_vecVelocity().Length2D() >= 120;
				bool climbin = player->m_nMoveType() & MOVETYPE_LADDER;

				if (player->m_vecVelocity().Length2D() <= 1.1f)
				{
					side = std::clamp(int((player->m_angEyeAngles().yaw - player->m_flLowerBodyYawTarget()) * 1000), -1, 1);
				}
				else // a little special something from sopmek
				{
					side = player->m_flPoseParameter()[7] > 0.5 ? 1 : -1;
				}

				float africanamerican = 0.f;

				if (!Globals::hitplayer)
				{
					if (tryagain) {
							if (shotsfired == 0)
								ohyeahbabythatsthespot = (Math::random_float(20.f, 32.f) * side);
							if (shotsfired == 1)
								ohyeahbabythatsthespot = (Math::random_float(10.f, 22.f) * side);
							if (shotsfired == 2)
								ohyeahbabythatsthespot = (Math::random_float(30.f, 42.f) * side);
							if (shotsfired == 3)
								ohyeahbabythatsthespot = (Math::random_float(40.f, 58.f) * side);
							if (fastasf || climbin || onground == false) {
								if (shotsfired == 4)
									ohyeahbabythatsthespot = 0;
								if (shotsfired == 5)
									ohyeahbabythatsthespot = 0;
							}
							else
							{
								if (shotsfired == 4)
									ohyeahbabythatsthespot = -player->m_flLowerBodyYawTarget();
								if (shotsfired == 5)
									ohyeahbabythatsthespot = player->m_flLowerBodyYawTarget();
							}
					}
					if (shotsfired >= 6)
						tryagain = false;

				}
				if (Globals::hitplayer)
				{
					if (player->EntIndex() != Globals::playerid)
					{
						go = true;
						tryagain = true;
					}
					else
					{
						africanamerican = ohyeahbabythatsthespot;
						go = false;
						tryagain = true;
					}
				}

				//Roll angles are no longer a trend
				/*if (player->m_angEyeAngles().roll >= 1 || (player->m_angEyeAngles().roll < 0 && player->m_angEyeAngles().roll != 0))
				{
					float theirroll = player->m_angEyeAngles().roll;

					if (tryagain2 == false) {
						if (shotsfired == 0)
							player->m_angEyeAngles().roll -= theirroll;
						if (shotsfired == 1)
							player->m_angEyeAngles().roll += theirroll;
						if (shotsfired == 2)
							player->m_angEyeAngles().roll = (rand() % -60 + 60);
					}
					if (shotsfired >= 3)
						tryagain2 = false;
				}
				else
				{
					player->m_angEyeAngles().roll = 0;
				}*/

				if (onground == true && fastasf == false && climbin == false) //We checkin to see if they can desync
				{
					candesync = true;

					if (!Globals::hitplayer && africanamerican != ohyeahbabythatsthespot) {
						if (side)
							player->m_angEyeAngles().yaw -= ohyeahbabythatsthespot;
						else
							player->m_angEyeAngles().yaw += ohyeahbabythatsthespot;
					}
					if (Globals::hitplayer && africanamerican) {
						if (side)
							player->m_angEyeAngles().yaw -= africanamerican;
						else
							player->m_angEyeAngles().yaw += africanamerican;
					}
				}
				else //They cant desync here lol!
				{
					candesync = false;
					ohyeahbabythatsthespot += 0;
				}
			}

				//Sees if killed player because now we gotta restart again
				if (Globals::killedplayer && shotsfired >= 1)
				{
					resolver::Get().PrintInChat(ohyeahbabythatsthespot, candesync);
					//Resetting shit
					ohyeahbabythatsthespot = 0;
					Globals::hitplayer = false;
					tryagain = false;
					//tryagain2 = false; for roll
					resetted = true;
				}

				//Just resets because otherwise if I reset only 1 will go because it checks for both with &&
				if (resetted)
				{
					Globals::killedplayer = false;
					shotsfired = 0;
					resetted = false;
				}
			}
		}
	}
}