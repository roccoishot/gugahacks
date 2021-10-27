#include "./lagcompensation.h"

void LagComp::Run()
{
		if (!g_LocalPlayer->IsAlive())
			return;

		for (int i = 1; i < g_EngineClient->GetMaxClients(); i++)
		{
			auto player = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i));
			if (!player || !g_LocalPlayer) continue;
			if (!player->IsPlayer()) continue;
			if (player == g_LocalPlayer) continue;
			if (player->IsDormant()) continue;
			if (!player->IsAlive()) continue;
			if (player->m_iTeamNum() == g_LocalPlayer->m_iTeamNum()) continue;

			if (!g_LocalPlayer->IsAlive())
				return;

			LagRecord record;
			memcpy(record.server_layers, player->GetAnimOverlays(), sizeof(AnimationLayer) * player->GetNumAnimOverlays());
			record.simtime = player->m_flSimulationTime();
			record.oldsimtime = player->m_flOldSimulationTime();
			record.origin = player->m_vecOrigin();

			resolver::Get().Resolve(player, record);

			// valid amount of records
			if (records[player->EntIndex()].size() > 1)
			 {
				auto cur_record = record; 
				records[player->EntIndex()].at(records[player->EntIndex()].size());
				auto& prev_record = records[player->EntIndex()].at(records[player->EntIndex()].size() - 1);
				auto delta = cur_record.origin - prev_record.origin;

				// make sure they arent breaking LC
				if (fabs(delta.Length()) < 4096.f) // are they? shit. nigga.
				LagCompensate(player, cur_record);
			}
			else
				records[player->EntIndex()].push_back(record);
		}
	}

void LagComp::LagCompensate(C_BasePlayer* player, LagRecord record)
{
		// simulate animlayers
		{
			player->GetPlayerAnimState()->m_flGoalFeetYaw = player->m_angEyeAngles().yaw;
			player->UpdateClientSideAnimation();
			memcpy(record.resolver_layers[0], player->GetAnimOverlays(), sizeof(AnimationLayer) * player->GetNumAnimOverlays());

			player->GetPlayerAnimState()->m_flGoalFeetYaw = player->m_angEyeAngles().yaw + 60.f;
			player->UpdateClientSideAnimation();
			memcpy(record.resolver_layers[1], player->GetAnimOverlays(), sizeof(AnimationLayer) * player->GetNumAnimOverlays());

			player->GetPlayerAnimState()->m_flGoalFeetYaw = player->m_angEyeAngles().yaw - 60.f;
			player->UpdateClientSideAnimation();
			memcpy(record.resolver_layers[2], player->GetAnimOverlays(), sizeof(AnimationLayer) * player->GetNumAnimOverlays());
		}

		resolver::Get().Resolve(player, record);

		// update anims
		player->UpdateClientSideAnimation();
	}