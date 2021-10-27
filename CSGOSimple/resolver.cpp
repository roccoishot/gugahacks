#include "./lagcompensation.h"

void resolver::Resolve(C_BasePlayer* player, LagRecord record)
{
		if (player->m_fFlags() & FL_ONGROUND) // no desync in air lmao
		{
			int side = 0;
			float delta = 58.f;

			auto animState = player->GetPlayerAnimState();

			if (!animState)
				return;


			if (player->m_vecVelocity().Length2D() <= 1.1f)
			{
				side = std::clamp(int((player->m_angEyeAngles().yaw - player->m_flLowerBodyYawTarget()) * 1000), -1, 1);
			}
			else // a little special something from sopmek
			{
				if (record.server_layers[6].m_flWeight > 0.1f && !(record.server_layers[12].m_flCycle * 1000)) // make sure that they are animating
				{
					float nildelta = abs(record.server_layers[6].m_flPlaybackRate - record.resolver_layers[0][6].m_flPlaybackRate);
					float delta1 = abs(record.server_layers[6].m_flPlaybackRate - record.resolver_layers[1][6].m_flPlaybackRate);
					float delta2 = abs(record.server_layers[6].m_flPlaybackRate - record.resolver_layers[2][6].m_flPlaybackRate);

					if (int(nildelta * 1000))
					{
						if (delta1 < delta2)
							side = 1;
						else
							side = -1;
					}

					side = player->m_flPoseParameter()[7] > 0.5 ? 1 : -1;
				}
			}

			//desync detector amounter!!
			//float a = player->GetPlayerAnimState()->m_flEyeYaw;
			//float b = player->GetPlayerAnimState()->m_flCurrentTorsoYaw;
			//long long c = a - b;

			if (side)
				player->GetPlayerAnimState()->m_flGoalFeetYaw = player->m_angEyeAngles().yaw + 58.f;
			else
				player->GetPlayerAnimState()->m_flGoalFeetYaw = player->m_angEyeAngles().yaw - 58.f;
		}
	}