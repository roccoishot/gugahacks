#include "aimbot.hpp"
#include "autowall.hpp"
#include "../backtrack.h"

#include "..//helpers/math.hpp"
#include "..//helpers/input.hpp"
#include "Misc.hpp"

float CLegitbot::GetFovToPlayer(QAngle viewAngle, QAngle aimAngle)
{
	QAngle delta = aimAngle - viewAngle;
	Math::FixAngles(delta);
	return sqrtf(powf(delta.pitch, 2.0f) + powf(delta.yaw, 2.0f));
}

bool CLegitbot::IsLineGoesThroughSmoke(Vector startPos, Vector endPos)
{
	static auto LineGoesThroughSmokeFn = (bool(*)(Vector, Vector))Utils::PatternScan2("client.dll", "55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0");
	return LineGoesThroughSmokeFn(startPos, endPos);
}

bool CLegitbot::IsEnabled(CUserCmd* cmd)
{
	auto i = 1; i <= g_GlobalVars->maxClients; i++;
		
	if (!C_BasePlayer::GetPlayerByIndex(i))
		return false;

		if (!g_EngineClient->IsInGame() || !g_LocalPlayer)
			return false;

		if (!g_LocalPlayer->IsAlive())
			return false;

		auto weapon = g_LocalPlayer->m_hActiveWeapon();
		if (!weapon || !weapon->IsGun())
			return false;

		settings = g_Options.weapons[weapon->m_Item().m_iItemDefinitionIndex()].legit;

		if (!settings.enabled)
			return false;

		if (!weapon->HasBullets())
			return false;

		return (cmd->buttons & IN_ATTACK) || (settings.autofire.enabled);
}

void CLegitbot::Smooth(QAngle currentAngle, QAngle aimAngle, QAngle& angle)
{
	auto smooth_value = max(1.0f, settings.smooth);
	
	Vector current, aim;

	Math::AngleVectors(currentAngle, current);
	Math::AngleVectors(aimAngle, aim);

	const Vector delta = aim - current;
	const Vector smoothed = current + delta / smooth_value;

	Math::VectorAngles(smoothed, angle);
}

void CLegitbot::RCS(QAngle& angle, C_BasePlayer* target)
{
	if (!settings.rcs.enabled || shotsFired < settings.rcs.start - 1)
		return;

	if (!settings.rcs.x && !settings.rcs.y)
		return;

	

	QAngle punch = g_LocalPlayer->m_aimPunchAngle() * g_CVar->FindVar("weapon_recoil_scale")->GetFloat();

	static auto recoil_scale = g_CVar->FindVar("weapon_recoil_scale");
	auto scale = recoil_scale->GetFloat();

	

	const auto x = float(settings.rcs.y) / 100.f * scale;
	const auto y = float(settings.rcs.x) / 100.f * scale;

	

	if (target)
		punch = { current_punch.pitch * x, current_punch.yaw * y, 0 };
	else if (settings.rcs.type == 0)
		punch = { (current_punch.pitch - last_punch.pitch) * x, (current_punch.yaw - last_punch.yaw) * y, 0 };

	if ((punch.pitch != 0.f || punch.yaw != 0.f) && current_punch.roll == 0.f) {
		angle -= punch;
		Math::FixAngles(angle);
	}
}

bool CLegitbot::IsSilent()
{
	if (settings.silent2 == 2)
		return !(shotsFired > 0 || !settings.silent2 || !settings.silent_fov);
	if (settings.silent2 == 1)
		return !(!settings.silent2 || !settings.silent_fov);
	if (settings.silent2 == 0)
		return !(shotsFired > 0 || !settings.silent2 || !settings.silent_fov);
}

float CLegitbot::GetFov()
{
	if (IsSilent())
		return settings.silent_fov;

	return settings.fov;
}

C_BasePlayer* CLegitbot::GetClosestPlayer(CUserCmd* cmd, int& bestBone, float& bestFov, QAngle& bestAngles)
{
	target = nullptr;

	std::vector<int> hitboxes;

	//I be getting mad head :weary:
	if (settings.hitboxes.head)
		hitboxes.emplace_back(HITBOX_HEAD);
		hitboxes.emplace_back(HITBOX_NECK);

	if (settings.hitboxes.chest)
	{
		hitboxes.emplace_back(HITBOX_UPPER_CHEST);
		hitboxes.emplace_back(HITBOX_CHEST);
		hitboxes.emplace_back(HITBOX_LOWER_CHEST);
	}

	if (settings.hitboxes.pelvis)
	{
		hitboxes.emplace_back(HITBOX_PELVIS);
	}

	if (settings.hitboxes.arms)
	{
		hitboxes.emplace_back(HITBOX_LEFT_FOREARM);
		hitboxes.emplace_back(HITBOX_LEFT_UPPER_ARM);

		hitboxes.emplace_back(HITBOX_RIGHT_FOREARM);
		hitboxes.emplace_back(HITBOX_RIGHT_UPPER_ARM);
	}

	if (settings.hitboxes.legs)
	{
		hitboxes.emplace_back(HITBOX_RIGHT_CALF);
		hitboxes.emplace_back(HITBOX_RIGHT_THIGH);

		hitboxes.emplace_back(HITBOX_LEFT_CALF);
		hitboxes.emplace_back(HITBOX_LEFT_THIGH);
	}

	//We about the handjobs
	if (settings.hitboxes.hands)
	{
		hitboxes.emplace_back(HITBOX_RIGHT_HAND);
		hitboxes.emplace_back(HITBOX_LEFT_HAND);
	}

	//Foot porn gang 
	if (settings.hitboxes.feet)
	{
		hitboxes.emplace_back(HITBOX_RIGHT_FOOT);
		hitboxes.emplace_back(HITBOX_LEFT_FOOT);
	}
	
	const Vector eyePos = g_LocalPlayer->GetEyePos();

	for (auto i = 1; i <= g_GlobalVars->maxClients; i++)
	{
		C_BasePlayer* player = C_BasePlayer::GetPlayerByIndex(i);

		if (!player || !player->IsAlive() || !player->IsPlayer() || player->m_bGunGameImmunity())
			continue;

		if (!player->IsEnemy())
			continue;

		player->GetBody();

		if (g_LocalPlayer->m_iTeamNum() == player->m_iTeamNum())
			continue;

		for (const auto hitbox : hitboxes)
		{
			Vector hitboxPos = player->GetHitboxPos(hitbox);
			QAngle ang;
			Math::VectorAngles(hitboxPos - eyePos, ang);
			const float fov = GetFovToPlayer(cmd->viewangles + last_punch * 2.f, ang);

			if (fov > GetFov())
				continue;

			if (!g_LocalPlayer->CanSeePlayer(player, hitboxPos))
			{
				if (!settings.autowall.enabled)
					continue;

				const auto damage = int(Autowall::GetDamage(hitboxPos));

				if (damage <= settings.autowall.min_damage)
					continue;
			}

			if (bestFov > fov)
			{
				bestBone = hitbox;
				bestAngles = ang;
				bestFov = fov;
				target = player;
			}
		}
	}

	return target;
}

void CLegitbot::Run(CUserCmd* cmd)
{
	if (int(GetTickCount()) > lastShotTick + 50)
		shotsFired = 0;

	current_punch = g_LocalPlayer->m_aimPunchAngle();

	if (!IsEnabled(cmd))
	{
		last_punch = { 0, 0, 0 };
		shot_delay = false;
		kill_delay = false;
		target = nullptr;
		return;
	}

	//RandomSeed(cmd->command_number);

	auto weapon = g_LocalPlayer->m_hActiveWeapon().Get();
	if (!weapon)
		return;

	auto weapon_data = weapon->GetCSWeaponData();
	if (!weapon_data)
		return;

	auto angles = cmd->viewangles;
	const auto current = angles;

	float fov = FLT_MAX;

	int bestBone = -1;

	if (GetClosestPlayer(cmd, bestBone, fov, angles))
	{
		if (settings.autofire.enabled && target->IsEnemy() && target->IsAlive() && !target->IsNotTarget())
			cmd->buttons |= IN_ATTACK;
	}

	if ((cmd->buttons & IN_ATTACK) /*&& !IsSilent()*/)
		RCS(angles, target);
	last_punch = current_punch;

	if (!IsSilent())
		Smooth(current, angles, angles);

	Math::FixAngles(angles);
	cmd->viewangles = angles;
	if (!IsSilent())
		g_EngineClient->SetViewAngles(&angles);

	if (~IN_ATTACK)
		g_LocalPlayer->SetVAngles(current);

	float oldForward;
	float oldSideMove;
	oldForward = cmd->forwardmove;
	oldSideMove = cmd->sidemove;
	if (g_LocalPlayer->m_nMoveType() != MOVETYPE_LADDER)
	Misc::MovementFix(current, cmd, oldForward, oldSideMove);

	if (g_LocalPlayer->m_hActiveWeapon()->IsPistol() && settings.autopistol)
	{
		const float server_time = g_LocalPlayer->m_nTickBase() * g_GlobalVars->interval_per_tick;
		const float next_shot = g_LocalPlayer->m_hActiveWeapon()->m_flNextPrimaryAttack() - server_time;

		if (next_shot > 0)
			cmd->buttons &= ~IN_ATTACK;
	}

	if (cmd->buttons & IN_ATTACK)
	{
		lastShotTick = GetTickCount();
		shotsFired++;
	}
}