#include "lagcompesnation.h"

void resolver::Resolve(C_BasePlayer* player, LagRecord record)
{
	
	if (player->m_fFlags() & FL_ONGROUND) // no desync in air lmao
	{
		int side = 0;
		float delta = 58.f;
		
		auto animState = player->GetPlayerAnimState();

		if (!animState)
			return;

		float flMaxBodyYaw = *reinterpret_cast<float*>(&animState->pad10[512]);
		float flMinBodyYaw = *reinterpret_cast<float*>(&animState->pad10[516]);

		// rebuild setupbones
		{
			auto delta_time = fmaxf(g_GlobalVars->curtime - animState->m_flLastClientSideAnimationUpdateTime, 0.f);


			float deltatime = fabs(delta_time);
			float stop_to_full_running_fraction = 0.f;
			bool is_standing = true;
			float v25 = std::clamp(player->m_flDuckAmount() + animState->m_fLandingDuckAdditiveSomething, 0.0f, 1.0f);
			float v26 = animState->m_fDuckAmount;
			float v27 = deltatime * 6.0f;
			float v28;

			// clamp
			if ((v25 - v26) <= v27) {
				if (-v27 <= (v25 - v26))
					v28 = v25;
				else
					v28 = v26 - v27;
			}
			else {
				v28 = v26 + v27;
			}

			auto weapon = player->m_hActiveWeapon().Get();
			if (!weapon)
				return;

			auto wpndata = weapon->GetCSWeaponData();

			if (!wpndata)
				return;


			float flMaxMovementSpeed = 260.0f;

			float speed = std::fminf(player->m_vecVelocity().Length(), 260.0f);

			float flRunningSpeed = speed / (flMaxMovementSpeed * 0.520f);
			float flDuckingSpeed_2 = speed / (flMaxMovementSpeed * 0.340f);

			float flDuckAmount = std::clamp(v28, 0.0f, 1.0f);
			
			float flYawModifier = (((stop_to_full_running_fraction * -0.3f) - 0.2f) * flRunningSpeed) + 1.0f;
			if (flDuckAmount > 0.0f) {
				float flDuckingSpeed = std::clamp(flDuckingSpeed_2, 0.0f, 1.0f);
				flYawModifier += (flDuckAmount * flDuckingSpeed) * (0.5f - flYawModifier);
			}

			if (!flYawModifier)
				return;

			flMaxBodyYaw *= flYawModifier;
			flMinBodyYaw *= flYawModifier;
		}
		

		if (player->m_vecVelocity().Length2D() <= 1.1f)
		{
			side = std::clamp(int((player->m_angEyeAngles().yaw - player->m_flLowerBodyYawTarget()) * 1000), -1, 1);
		}
		else // a little special something from sopmek
		{
			if (record.server_layers[6].m_flWeight > 0.1f
				&& !(record.server_layers[12].m_flCycle * 1000)) // make sure that they are animating
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
			}
		}

		if (side)
			player->GetPlayerAnimState()->m_flGoalFeetYaw = player->m_angEyeAngles().yaw + flMaxBodyYaw;
		else
			player->GetPlayerAnimState()->m_flGoalFeetYaw = player->m_angEyeAngles().yaw + flMinBodyYaw;
	}
}