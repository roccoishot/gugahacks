#include <algorithm> 
#include <stdio.h>
#include <string>
#include <iostream>
#include "backtrack.h"
#include "./helpers/math.hpp"
#include "options.hpp"
#include "Globals.h"
#include "features/aimbot.hpp"

#define TICK_INTERVAL			(g_GlobalVars->interval_per_tick)
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

float LagFix()
{
	float updaterate = g_CVar->FindVar("cl_updaterate")->GetFloat();
	auto* minupdate = g_CVar->FindVar("sv_minupdaterate");
	auto* maxupdate = g_CVar->FindVar("sv_maxupdaterate");

	if (minupdate && maxupdate)
		updaterate = maxupdate->GetFloat();

	float ratio = g_CVar->FindVar("cl_interp_ratio")->GetFloat();

	if (ratio == 0)
		ratio = 1.0f;

	float lerp = g_CVar->FindVar("cl_interp")->GetFloat();
	auto* cmin = g_CVar->FindVar("sv_client_min_interp_ratio");
	auto* cmax = g_CVar->FindVar("sv_client_max_interp_ratio");

	if (cmin && cmax && cmin->GetFloat() != 1)
		ratio = std::clamp(ratio, cmin->GetFloat(), cmax->GetFloat());

	return max(lerp, ratio / updaterate);
}

void TimeWarp::CreateMove(CUserCmd* cmd)
{
	int bestTargetIndex = -1;
	float bestFov = FLT_MAX;

	if (!g_LocalPlayer->IsAlive())
		return;
		for (int i = 1; i < g_EngineClient->GetMaxClients(); i++)
		{
			C_BasePlayer* pEntity = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i));
			if (!pEntity || !g_LocalPlayer) continue;
			if (!g_LocalPlayer->IsAlive()) continue;


			if (pEntity->valid(true, true)) {

			float simtime = pEntity->m_flSimulationTime();
			Vector hitboxPos = pEntity->GetHitboxPos(0);

			this->TimeWarpData[i][cmd->command_number % (static_cast<int>(NUM_OF_TICKS) + 1)] = StoredData{ simtime, hitboxPos };
			Vector ViewDir;
			Math::AngleVectors(cmd->viewangles + (g_LocalPlayer->m_aimPunchAngle() * 2.f), ViewDir);
			float FOVDistance = Math::DistancePointToLine(hitboxPos, g_LocalPlayer->GetEyePos(), ViewDir);

			if (bestFov > FOVDistance)
			{
				bestFov = FOVDistance;
				bestTargetIndex = i;
			}

			float bestTargetSimTime = -1;
			if (bestTargetIndex != -1)
			{
				float tempFloat = FLT_MAX;
				Vector ViewDir;
				Math::AngleVectors(cmd->viewangles + (g_LocalPlayer->m_aimPunchAngle() * 2.f), ViewDir);
				for (int t = 0; t < static_cast<int>(NUM_OF_TICKS); ++t)
				{
					Globals::bestbtid = bestTargetIndex;
					float tempFOVDistance = Math::DistancePointToLine(TimeWarpData[bestTargetIndex][t].hitboxPos, g_LocalPlayer->GetEyePos(), ViewDir);
					if (tempFloat > tempFOVDistance && TimeWarpData[bestTargetIndex][t].simtime > g_LocalPlayer->m_flSimulationTime() - 1)
					{
						if (g_LocalPlayer->CanSeePlayer(static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(bestTargetIndex)), TimeWarpData[bestTargetIndex][t].hitboxPos))
						{
							tempFloat = tempFOVDistance;
							bestTargetSimTime = TimeWarpData[bestTargetIndex][t].simtime;
						}
					}
				}

				if (g_Options.aimbot.backtrack)
				{
					if (bestTargetSimTime >= 0 && cmd->buttons & IN_ATTACK)
					{
						auto thenigga = (C_BasePlayer*)(g_EntityList->GetClientEntity(bestTargetIndex));

						//thenigga->m_vecOrigin() = extrapolate(thenigga, TIME_TO_TICKS(g_EngineClient->GetNetChannelInfo()->GetAvgLatency(FLOW_OUTGOING) + g_EngineClient->GetNetChannelInfo()->GetAvgLatency(FLOW_INCOMING)));
						cmd->tick_count = TIME_TO_TICKS(bestTargetSimTime) + TIME_TO_TICKS(LagFix()); //+ TIME_TO_TICKS(g_EngineClient->GetNetChannelInfo()->GetAvgLatency(FLOW_OUTGOING) + g_EngineClient->GetNetChannelInfo()->GetAvgLatency(FLOW_INCOMING));
					}
				}
			}
		}
	}
}
