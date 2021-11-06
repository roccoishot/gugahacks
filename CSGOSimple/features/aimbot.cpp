#include "aimbot.hpp"
#include "autowall.hpp"
#include "../backtrack.h"

#include "..//helpers/math.hpp"
#include "..//helpers/input.hpp"
#include "Misc.hpp"
#include "../Globals.h"

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

		if (!g_Options.aimbot.enabled)
			return false;

		if (!weapon->HasBullets())
			return false;

				return (cmd->buttons & IN_ATTACK) || (g_Options.aimbot.autofire);
}

void CLegitbot::Smooth(QAngle currentAngle, QAngle aimAngle, QAngle& angle)
{
	auto smooth_value = max(1.0f, g_Options.aimbot.smoof);
	
	Vector current, aim;

	Math::AngleVectors(currentAngle, current);
	Math::AngleVectors(aimAngle, aim);

	const Vector delta = aim - current;
	const Vector smoothed = current + delta / smooth_value;

	Math::VectorAngles(smoothed, angle);
}

void CLegitbot::RCS(QAngle& angle, C_BasePlayer* target)
{
	if (!g_Options.aimbot.rcs || shotsFired < g_Options.aimbot.rcsstart - 1)
		return;

	if (!g_Options.aimbot.x && !g_Options.aimbot.y)
		return;

	

	QAngle punch = g_LocalPlayer->m_aimPunchAngle() * g_CVar->FindVar("weapon_recoil_scale")->GetFloat();

	static auto recoil_scale = g_CVar->FindVar("weapon_recoil_scale");
	auto scale = recoil_scale->GetFloat();

	

	const auto x = float(g_Options.aimbot.y) / 100.f * scale;
	const auto y = float(g_Options.aimbot.x) / 100.f * scale;

	
	if (g_Options.fatassmf) {
		if (target)
			punch = { Globals::aim_punch_old.pitch * x, Globals::aim_punch_old.yaw * y, 0 };
		else if (g_Options.aimbot.rcstype == 0)
			punch = { (Globals::aim_punch_old.pitch - last_punch.pitch) * x, (Globals::aim_punch_old.yaw - last_punch.yaw) * y, 0 };
	}

	if (!g_Options.fatassmf) {
		if (target)
			punch = { current_punch.pitch * x,  current_punch.yaw * y, 0 };
		else if (g_Options.aimbot.rcstype == 0)
			punch = { (current_punch.pitch - last_punch.pitch) * x, (current_punch.yaw - last_punch.yaw) * y, 0 };
	}

	if ((punch.pitch != 0.f || punch.yaw != 0.f) && punch.roll == 0.f) {
		angle -= punch;
		Math::FixAngles(angle);
	}
}

bool CLegitbot::IsSilent()
{
	if (g_Options.aimbot.silent == 2)
		return !(shotsFired > 0 || !g_Options.aimbot.silent || !g_Options.aimbot.silentfov);
	if (g_Options.aimbot.silent == 1)
		return !(!g_Options.aimbot.silent || !g_Options.aimbot.silentfov);
	if (g_Options.aimbot.silent == 0)
		return !(shotsFired > 0 || !g_Options.aimbot.silent || !g_Options.aimbot.silentfov);
}

float CLegitbot::GetFov()
{
	if (IsSilent())
		return g_Options.aimbot.silentfov;

	return g_Options.aimbot.fov;
}

C_BasePlayer* CLegitbot::GetClosestPlayer(CUserCmd* cmd, int& bestBone, float& bestFov, QAngle& bestAngles)
{
	target = nullptr;

	std::vector<int> hitboxes;

	//I be getting mad head :weary:
	if (g_Options.aimbot.head) {
		hitboxes.emplace_back(HITBOX_HEAD);
	}

	if (g_Options.aimbot.chest)
	{
		hitboxes.emplace_back(HITBOX_UPPER_CHEST);
		hitboxes.emplace_back(HITBOX_CHEST);
		hitboxes.emplace_back(HITBOX_LOWER_CHEST);
	}

	if (g_Options.aimbot.pelvis)
	{
		hitboxes.emplace_back(HITBOX_PELVIS);
	}

	if (g_Options.aimbot.arms)
	{
		hitboxes.emplace_back(HITBOX_LEFT_FOREARM);
		hitboxes.emplace_back(HITBOX_LEFT_UPPER_ARM);

		hitboxes.emplace_back(HITBOX_RIGHT_FOREARM);
		hitboxes.emplace_back(HITBOX_RIGHT_UPPER_ARM);
	}

	if (g_Options.aimbot.legs)
	{
		hitboxes.emplace_back(HITBOX_RIGHT_CALF);
		hitboxes.emplace_back(HITBOX_RIGHT_THIGH);

		hitboxes.emplace_back(HITBOX_LEFT_CALF);
		hitboxes.emplace_back(HITBOX_LEFT_THIGH);
	}

	//We about the handjobs
	if (g_Options.aimbot.hands)
	{
		hitboxes.emplace_back(HITBOX_RIGHT_HAND);
		hitboxes.emplace_back(HITBOX_LEFT_HAND);
	}

	//Foot porn gang 
	if (g_Options.aimbot.feet)
	{
		hitboxes.emplace_back(HITBOX_RIGHT_FOOT);
		hitboxes.emplace_back(HITBOX_LEFT_FOOT);
	}
	
	const Vector eyePos = g_LocalPlayer->GetEyePos();

	for (auto i = 1; i <= g_GlobalVars->maxClients; i++)
	{
		C_BasePlayer* player = C_BasePlayer::GetPlayerByIndex(i);

		if (!player || !player->IsAlive() || !player->IsPlayer() || player->m_bGunGameImmunity() || player->IsNotTarget())
			continue;

		if (!player->IsEnemy())
			continue;

		if (player->IsTeammate())
			continue;

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
				if (!g_Options.aimbot.autowall)
					continue;

				const auto damage = int(Autowall::GetDamage(hitboxPos));

				if (damage <= g_Options.aimbot.autowallmin)
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

	if (g_Options.aimbot.autorevolver1) {
		if (g_LocalPlayer->IsAlive() && g_LocalPlayer->m_hActiveWeapon()->m_iItemDefinitionIndex() == WEAPON_REVOLVER)
			g_Options.aimbot.autorevolver2 = true;
		else
			g_Options.aimbot.autorevolver2 = false;
	}
	else
		g_Options.aimbot.autorevolver2 = false;

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
		if (g_Options.aimbot.hc)
		{
			if (g_LocalPlayer->m_hActiveWeapon()->GetInaccuracy() / g_LocalPlayer->m_hActiveWeapon()->GetSpread() <= g_Options.aimbot.hitchance)
			{

				if (g_Options.aimbot.autorevolver2 && g_LocalPlayer->m_hActiveWeapon()->m_iItemDefinitionIndex() == WEAPON_REVOLVER) {
					if (g_Options.aimbot.autofire && target->IsEnemy() && !target->IsTeammate() && target->IsAlive() && !target->IsNotTarget()) {
						cmd->buttons |= IN_ATTACK2;
						const float server_time = g_LocalPlayer->m_nTickBase() * g_GlobalVars->interval_per_tick;
						const float next_shot = g_LocalPlayer->m_hActiveWeapon()->m_flNextPrimaryAttack() - server_time;

						if (next_shot > 0)
							cmd->buttons &= ~IN_ATTACK2;
					}
				}

				if (!g_Options.aimbot.autorevolver2 && !(g_LocalPlayer->m_hActiveWeapon()->m_iItemDefinitionIndex() == WEAPON_REVOLVER)) {
					if (g_Options.aimbot.autofire && target->IsEnemy() && !target->IsTeammate() && target->IsAlive() && !target->IsNotTarget()) {
						cmd->buttons |= IN_ATTACK;
						const float server_time = g_LocalPlayer->m_nTickBase() * g_GlobalVars->interval_per_tick;
						const float next_shot = g_LocalPlayer->m_hActiveWeapon()->m_flNextPrimaryAttack() - server_time;

						if (next_shot > 0)
							cmd->buttons &= ~IN_ATTACK;
					}
				}
			}
		}
		if (!g_Options.aimbot.hc) {
			if (g_Options.aimbot.autorevolver2 && g_LocalPlayer->m_hActiveWeapon()->m_iItemDefinitionIndex() == WEAPON_REVOLVER) {
				if (g_Options.aimbot.autofire && target->IsEnemy() && !target->IsTeammate() && target->IsAlive() && !target->IsNotTarget()) {
					cmd->buttons |= IN_ATTACK2;
					const float server_time = g_LocalPlayer->m_nTickBase() * g_GlobalVars->interval_per_tick;
					const float next_shot = g_LocalPlayer->m_hActiveWeapon()->m_flNextPrimaryAttack() - server_time;

					if (next_shot > 0)
						cmd->buttons &= ~IN_ATTACK2;
				}
			}

			if (!g_Options.aimbot.autorevolver2 && !(g_LocalPlayer->m_hActiveWeapon()->m_iItemDefinitionIndex() == WEAPON_REVOLVER)) {
				if (g_Options.aimbot.autofire && target->IsEnemy() && !target->IsTeammate() && target->IsAlive() && !target->IsNotTarget()) {
					cmd->buttons |= IN_ATTACK;
					const float server_time = g_LocalPlayer->m_nTickBase() * g_GlobalVars->interval_per_tick;
					const float next_shot = g_LocalPlayer->m_hActiveWeapon()->m_flNextPrimaryAttack() - server_time;

					if (next_shot > 0)
						cmd->buttons &= ~IN_ATTACK;
				}
			}
		}
	}

			if (g_Options.aimbot.autorevolver2 && g_LocalPlayer->m_hActiveWeapon()->m_iItemDefinitionIndex() == WEAPON_REVOLVER) {
					auto pWeapon = g_LocalPlayer->m_hActiveWeapon();
					static int pasteme = 0;
					pasteme++;
					if (pasteme <= 14.5f) {
						cmd->buttons |= IN_ATTACK;
					}
					else {
						pasteme = 0;

						float flPostponeFireReady = pWeapon->m_flPostponeFireReadyTime();
						if (flPostponeFireReady > 0 && flPostponeFireReady < g_GlobalVars->curtime) {
							cmd->buttons &= ~IN_ATTACK;
						}
					}
				}

			if (cmd->buttons & IN_ATTACK)
				RCS(angles, target);
			last_punch = current_punch;

			if (!IsSilent())
				Smooth(current, angles, angles);

			cmd->viewangles = angles;
			if (!IsSilent())
				g_EngineClient->SetViewAngles(&angles);

			if (!g_Options.aimbot.autorevolver2 && !(g_LocalPlayer->m_hActiveWeapon()->m_iItemDefinitionIndex() == WEAPON_REVOLVER)) {
					if (!(g_LocalPlayer->m_iShotsFired() >= 1))
						g_LocalPlayer->SetVAngles(current);
				}

			if (g_Options.aimbot.autorevolver2 && g_LocalPlayer->m_hActiveWeapon()->m_iItemDefinitionIndex() == WEAPON_REVOLVER) {
					if (!(cmd->buttons & IN_ATTACK2))
						g_LocalPlayer->SetVAngles(current);
				}

			Math::FixAngles(angles);

			float oldForward;
			float oldSideMove;
			oldForward = cmd->forwardmove;
			oldSideMove = cmd->sidemove;
			if (g_LocalPlayer->m_nMoveType() != MOVETYPE_LADDER)
				Misc::Get().MovementFix(current, cmd, oldForward, oldSideMove);

					if (cmd->buttons & IN_ATTACK)
					{
						lastShotTick = GetTickCount();
						shotsFired++;
					}
}