#include "lagcompesnation.h"

void LagComp::Run()
{
	for (auto i = 1; i <= g_GlobalVars->maxClients; i++)
	{
		C_BasePlayer* player = C_BasePlayer::GetPlayerByIndex(i);

		if (!player || !player->IsAlive() || !player->IsPlayer() || player->m_bGunGameImmunity())
			continue;

		if (!player->IsEnemy())
			continue;

		LagRecord record;
		memcpy(record.server_layers, player->GetAnimOverlays(), sizeof(AnimationLayer) * player->GetNumAnimOverlays());
		record.simtime = player->m_flSimulationTime();
		record.oldsimtime = player->m_flOldSimulationTime();
		record.origin = player->m_vecOrigin();

		// valid amount of records
		//if (records[player->EntIndex()].size() > 1)
		{
			auto cur_record = record; // records[player->EntIndex()].at(records[player->EntIndex()].size())
			//auto prev_record = records[player->EntIndex()].at(records[player->EntIndex()].size() - 1);
			//auto delta = cur_record.origin - prev_record.origin;

			// make sure they arent breaking LC
			//if (fabs(delta.Length()) < 4096.f) // are they? shit. nigga.
			LagCompensate(player, cur_record);
		}
		//else
		//	records[player->EntIndex()].push_back(record);
	}
}

void LagComp::LagCompensate(C_BasePlayer* player, LagRecord record)
{
	// extrapolate
	/* {
		auto delta = record.origin - prev_record.origin;
		int choked = max(TIME_TO_TICKS(fabs(record.simtime - record.oldsimtime) - g_GlobalVars->interval_per_tick), 0);
		auto velocity_per_tick = delta * choked;

		if (fabs(velocity_per_tick.Length()) > 0.f)
			player->m_vecVelocity() += delta * velocity_per_tick;
	}*/

	// simulate animlayers
	/* {
		player->GetPlayerAnimState()->m_flGoalFeetYaw = player->m_angEyeAngles().yaw;
		player->UpdateClientSideAnimation();
		memcpy(record.resolver_layers[0], player->GetAnimOverlays(), sizeof(AnimationLayer) * player->GetNumAnimOverlays());

		player->GetPlayerAnimState()->m_flGoalFeetYaw = player->m_angEyeAngles().yaw + 60.f;
		player->UpdateClientSideAnimation();
		memcpy(record.resolver_layers[1], player->GetAnimOverlays(), sizeof(AnimationLayer) * player->GetNumAnimOverlays());

		player->GetPlayerAnimState()->m_flGoalFeetYaw = player->m_angEyeAngles().yaw - 60.f;
		player->UpdateClientSideAnimation();
		memcpy(record.resolver_layers[2], player->GetAnimOverlays(), sizeof(AnimationLayer) * player->GetNumAnimOverlays());
	}*/

	resolver::Get().Resolve(player, record);

	// update anims
	player->UpdateClientSideAnimation();
}