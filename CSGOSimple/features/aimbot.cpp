#include "aimbot.hpp"
#include "autowall.hpp"
#include "../backtrack.h"
#include "..//helpers/math.hpp"
#include "..//helpers/input.hpp"
#include "Misc.hpp"
#include "../Globals.h"
#include "../xor.h"
#ifdef ENABLE_XOR
#define XorStr _xor_ 
#else
#define XorStr
#endif
#pragma intrinsic(_ReturnAddress)  

bool intercourse = false;

float CLegitbot::GetFovToPlayer(QAngle viewAngle, QAngle aimAngle)
{
	QAngle delta = aimAngle - viewAngle;
	Math::FixAngles(delta);
	return sqrtf(powf(delta.pitch, 2.0f) + powf(delta.yaw, 2.0f));
}

bool CLegitbot::IsLineGoesThroughSmoke(Vector startPos, Vector endPos)
{
	static auto LineGoesThroughSmokeFn = (bool(*)(Vector, Vector))Utils::PatternScan2(XorStr("client.dll"), XorStr("55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0"));
	return LineGoesThroughSmokeFn(startPos, endPos);
}

bool CLegitbot::IsEnabled(CUserCmd* cmd)
{

	auto i = 1; i <= g_EngineClient->GetMaxClients(); i++;

	if (!C_BasePlayer::GetPlayerByIndex(i))
		return false;

	if (!g_EngineClient->IsInGame() || !g_LocalPlayer)
		return false;

	if (!g_LocalPlayer->IsAlive())
		return false;

	auto weapon = g_LocalPlayer->m_hActiveWeapon();
	if (!weapon || (!weapon->IsGun() && !weapon->IsZeus()))
		return false;

	if (!g_Options.aimbot.enabled)
		return false;

	if (!weapon->HasBullets())
		return false;

	if (g_Options.aimbot.silent) {
		if (!weapon->CanFire())
			return false;
	}

	if (g_Options.aimbot.noaonspec)
	{
		if (g_Options.spectators > 0)
			return false;
	}

	if (g_Options.aimbot.afkey <= 0)
		return (GetAsyncKeyState(g_Options.aimbot.aimkey)) || (g_Options.aimbot.autofire);
	if (g_Options.aimbot.afkey > 0)
		return (GetAsyncKeyState(g_Options.aimbot.aimkey)) || (g_Options.aimbot.autofire && GetAsyncKeyState(g_Options.aimbot.afkey));
}

void CLegitbot::Smooth(QAngle currentAngle, QAngle aimAngle, QAngle& angle)
{
	int smoothness = g_Options.aimbot.smoof;

	auto smooth_value = max(1.0f, smoothness);

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

	if (!target)
		return;

	if (!target->IsAlive())
		return;

	if (!target->valid(true, true))
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
	if (g_Options.aimbot.silent == true)
		return !(!g_Options.aimbot.silent || !g_Options.aimbot.silentfov);
	if (g_Options.aimbot.silent == false)
		return !(shotsFired > 0 || !g_Options.aimbot.silent || !g_Options.aimbot.silentfov);
}

float CLegitbot::GetFov()
{
	float newfov = g_Options.aimbot.silentfov;

	if (g_Options.aimbot.silentfov == 180)
		newfov = INT_MAX;
	else
		newfov = g_Options.aimbot.silentfov;

	if (IsSilent())
		return newfov;
	else if (!IsSilent())
		return g_Options.aimbot.fov;
}

void NormalizeNum2(Vector& vIn, Vector& vOut)
{
	float flLen = vIn.Length();

	if (flLen == 0)
	{
		vOut.Init(0, 0, 1);
		return;
	}

	flLen = 1 / flLen;
	vOut.Init(vIn.x * flLen, vIn.y * flLen, vIn.z * flLen);
}

float fov_player2(Vector ViewOffSet, QAngle View, C_BasePlayer* entity)
{
	// Anything past 180 degrees is just going to wrap around
	CONST FLOAT MaxDegrees = 180.0f;

	// Get local angles
	QAngle Angles = View;

	// Get local view / eye position
	Vector Origin = ViewOffSet;

	// Create and intiialize vectors for calculations below
	Vector Delta(0, 0, 0);
	//Vector Origin(0, 0, 0);
	Vector Forward(0, 0, 0);

	// Convert angles to normalized directional forward vector
	Math::AngleVectors(Angles, Forward);

	Vector AimPos = entity->GetHitboxPos(HITBOX_HEAD); //pvs fix disabled

	//VectorSubtract(AimPos, Origin, Delta);
	Origin.VectorSubtract(AimPos, Origin, Delta);
	//Delta = AimPos - Origin;

	// Normalize our delta vector
	NormalizeNum2(Delta, Delta);

	// Get dot product between delta position and directional forward vectors
	FLOAT DotProduct = Forward.Dot(Delta);

	// Time to calculate the field of view
	return (acos(DotProduct) * (MaxDegrees / M_PI));
}

int GetNearestPlayerToCrosshair2()
{
	float BestFov = FLT_MAX;
	int BestEnt = -1;
	QAngle MyAng;
	g_EngineClient->GetViewAngles(&MyAng);

	for (int i = 1; i < g_EngineClient->GetMaxClients(); i++)
	{
		auto entity = static_cast<C_BasePlayer*> (g_EntityList->GetClientEntity(i));

		if (!entity || !g_LocalPlayer || !entity->valid(true, true))
		{
			continue;
		}

		float CFov = fov_player2(g_LocalPlayer->m_vecOrigin(), MyAng, entity); //Math::GetFOV(MyAng, Math::CalcAngle(g_LocalPlayer->GetEyePos(), entity->GetEyePos()));

		if (CFov < BestFov)
		{
			BestFov = CFov;
			BestEnt = i;
		}
	}

	return BestEnt;
}


C_BasePlayer* CLegitbot::GetClosestPlayer(CUserCmd* cmd, int& bestBone, float& bestFov, QAngle& bestAngles)
{
	target = nullptr;

	std::vector<int> hitboxes;

	//I be getting mad head :weary:
	if (g_Options.aimbot.head) {
		hitboxes.emplace_back(HITBOX_HEAD);
	}

	//She Be Givin me this
	if (g_Options.aimbot.neck) {
		hitboxes.emplace_back(HITBOX_NECK);
	}

	//CUM HERE
	if (g_Options.aimbot.chest)
	{
		hitboxes.emplace_back(HITBOX_UPPER_CHEST);
		hitboxes.emplace_back(HITBOX_CHEST);
		hitboxes.emplace_back(HITBOX_LOWER_CHEST);
	}

	//Small waist pretty face with a big bank $$$
	if (g_Options.aimbot.stomach)
	{
		hitboxes.emplace_back(HITBOX_STOMACH);
	}

	//Pevlis? more like pool of cum
	if (g_Options.aimbot.pelvis)
	{
		hitboxes.emplace_back(HITBOX_PELVIS);
	}

	//BULKY ARMS ONG
	if (g_Options.aimbot.arms)
	{
		hitboxes.emplace_back(HITBOX_LEFT_FOREARM);
		hitboxes.emplace_back(HITBOX_LEFT_UPPER_ARM);

		hitboxes.emplace_back(HITBOX_RIGHT_FOREARM);
		hitboxes.emplace_back(HITBOX_RIGHT_UPPER_ARM);
	}


	//THIGHS
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

	for (auto i = 1; i <= g_EngineClient->GetMaxClients(); i++)
	{
		C_BasePlayer* player = C_BasePlayer::GetPlayerByIndex(i);

		Vector sex;

		if (!player) continue;
		if (!player->valid(true, true)) continue;
		if (player->m_bGunGameImmunity()) continue;

		for (int t = 0; t < static_cast<int>(NUM_OF_TICKS); ++t)
		{
			if (Globals::bestbtid > -1) {
				sex = TimeWarp::Get().TimeWarpData[Globals::bestbtid][t].hitboxPos;

			}

		}

		for (const auto hitbox : hitboxes)
		{
			if (player->valid(true, true)) {
				hitboxforbt = hitbox;
				Vector hitboxPos = player->GetHitboxPos(hitbox);

				//RAGE HITBOXES
				int betterhitbox = -1;

				if (g_Options.aimbot.rageaim) {
					if (g_Options.aimbot.safehead == true) {
							const auto semen = Autowall::GetDamage(player->GetHitboxPos(hitbox));
							float rape = -1.f;

							if (semen > rape && intercourse == false && ran == true)
							{
								betterhitbox = hitbox;
							}
							else
							{
								betterhitbox = 0;
							}
					}
					else
					{
						const auto semen = Autowall::GetDamage(player->GetHitboxPos(hitbox));
						float rape = -1.f;

						if (semen > rape)
						{
							betterhitbox = hitbox;

						}
						else
						{
							betterhitbox = betterhitbox;
						}
					}
				}

				if (betterhitbox > -1 && betterhitbox < 20 && g_Options.aimbot.rageaim) {
					if (g_Options.aimbot.backtrack == true && g_Options.aimbot.aimatbt == true && Autowall::GetDamage(player->GetHitboxPos(betterhitbox)) < Autowall::GetDamage(sex)) {
						hitboxPos = sex;
					}
					else
					{
						hitboxPos = player->GetHitboxPos(betterhitbox);
					}
				}
				else
				{
					if (g_Options.aimbot.backtrack == true && g_Options.aimbot.aimatbt == true && Autowall::GetDamage(hitboxPos) < Autowall::GetDamage(sex)) {
						hitboxPos = sex;
					}
					else
					{
						hitboxPos = player->GetHitboxPos(hitbox);
					}
				}
					QAngle ang;
				Math::VectorAngles(hitboxPos - eyePos, ang);
				float fov = GetFovToPlayer(cmd->viewangles + last_punch * 2.f, ang);

				if (fov > GetFov())
					continue;

				bool ovr = false;
				if (GetAsyncKeyState(g_Options.aimbot.dmgovrdk))
					ovr = true;
				if (!GetAsyncKeyState(g_Options.aimbot.dmgovrdk))
					ovr = false;

				if ((!g_LocalPlayer->CanSeePlayer(player, hitboxPos)) && player->valid(true, true))
				{
					if (!g_Options.aimbot.autowall)
						continue;

					auto damage = int(Autowall::GetDamage(hitboxPos));

					int mindmg = ovr ? g_Options.aimbot.dmgovrd : g_Options.aimbot.autowallmin;

					if (damage < mindmg)
						continue;
				}

				if ((g_LocalPlayer->CanSeePlayer(player, hitboxPos)) && player->valid(true, true))
				{

					auto damage = int(Autowall::GetDamage(hitboxPos));

					int mindmg = ovr ? g_Options.aimbot.dmgovrd : g_Options.aimbot.mindmg;

					if (damage < mindmg)
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
	}

	return target;
}

void CLegitbot::Run(CUserCmd* cmd)
{
	ran = true;
	if (int(GetTickCount64()) > lastShotTick + 50)
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

	bool doingthezeus = false;

	if (g_Options.aimbot.autorevolver1) {
		if (g_LocalPlayer && g_LocalPlayer->IsAlive() && weapon && weapon_data && g_LocalPlayer->m_hActiveWeapon()->m_iItemDefinitionIndex() == WEAPON_REVOLVER)
			g_Options.aimbot.autorevolver2 = true;
		if (g_LocalPlayer && g_LocalPlayer->IsAlive() && weapon && weapon_data && g_LocalPlayer->m_hActiveWeapon()->m_iItemDefinitionIndex() != WEAPON_REVOLVER)
			g_Options.aimbot.autorevolver2 = false;
	}
	else
		g_Options.aimbot.autorevolver2 = false;

	if (g_Options.aimbot.zeusbot) {
		if (g_LocalPlayer && g_LocalPlayer->IsAlive() && weapon && weapon_data && g_LocalPlayer->m_hActiveWeapon()->m_iItemDefinitionIndex() == WEAPON_TASER)
			doingthezeus = true;
		if (g_LocalPlayer && g_LocalPlayer->IsAlive() && weapon && weapon_data && g_LocalPlayer->m_hActiveWeapon()->m_iItemDefinitionIndex() != WEAPON_TASER)
			doingthezeus = false;
	}
	else
		doingthezeus = false;

	auto angles = cmd->viewangles;
	const auto current = angles;

	float fov = FLT_MAX;
	int bestBone = -1;
	bool pickhead = false;
	static bool autostopped = false;

	if (target && target->valid(true, true)) {
		bool onground = target->m_fFlags() & FL_ONGROUND;
		bool fastasf = target->m_vecVelocity().Length2D() >= 120;
		bool climbin = target->m_nMoveType() & MOVETYPE_LADDER;

		if (g_Options.aimbot.safehead) {
			//Most Likely Desyncing
			if (onground && !fastasf && !climbin) {
				if (bestBone == 0)
					bestBone + 2;
				else
					bestBone + 0;

				pickhead = false;
			}
			else if (!onground || fastasf || climbin) {
				if (bestBone != 0)
					bestBone = 0;

				pickhead = true;
			}
			if (pickhead)
			{
				if (bestBone != 0)
					bestBone = 0;
			}
			else
			{
				if (bestBone == 0)
					bestBone + 2;
				else
					bestBone + 0;
			}
		}
		else {
			bestBone = bestBone;
			pickhead = false;
		}

		if (g_Options.aimbot.rageaim) {
			//Most Likely Desyncing
			if (!onground || fastasf || climbin) {
				if (bestBone != 0)
					bestBone = 0;
			}
			else
			{
				if (bestBone == 0)
					bestBone + 2;
				else
					bestBone + 0;
			}
		}
		else {
			bestBone = bestBone;
		}
	}

	intercourse = pickhead;

	if (GetClosestPlayer(cmd, bestBone, fov, angles))
	{

		for (int stephaniesoo = 0; stephaniesoo < 19; ++stephaniesoo) {
			if (target && target->valid(true, true)) {

				if (g_Options.aimbot.autostop) {
					if (g_LocalPlayer && g_LocalPlayer->IsAlive() && !g_LocalPlayer->m_hActiveWeapon().Get()->IsZeus() && (!(cmd->buttons & IN_JUMP)) && !g_LocalPlayer->m_hActiveWeapon().Get()->IsKnife()) {
						if (g_LocalPlayer->m_fFlags() & FL_ONGROUND) {
							if (g_LocalPlayer->CanSeePlayer(target, stephaniesoo))
							{
								auto velocity = g_LocalPlayer->m_vecVelocity();

								if (velocity.Length2D() > 20.0f)
								{
									Vector direction;
									QAngle real_view;

									Math::vector_angles(velocity, direction);
									g_EngineClient->GetViewAngles(&real_view);

									direction.y = real_view.yaw - direction.y;

									Vector forward;
									Math::angle_vectors(direction, forward);

									static auto cl_forwardspeed = g_CVar->FindVar(XorStr("cl_forwardspeed"));
									static auto cl_sidespeed = g_CVar->FindVar(XorStr("cl_sidespeed"));

									auto negative_forward_speed = -cl_forwardspeed->GetFloat();
									auto negative_side_speed = -cl_sidespeed->GetFloat();

									auto negative_forward_direction = forward * negative_forward_speed;
									auto negative_side_direction = forward * negative_side_speed;

									cmd->forwardmove = negative_forward_direction.x;
									cmd->sidemove = negative_side_direction.y;
								}
								else
								{
									auto speed = 0.1f;

									if (cmd->buttons & IN_DUCK)
										speed *= 2.94117647f;

									static auto switch_move = false;

									if (switch_move)
										cmd->sidemove += speed;
									else
										cmd->sidemove -= speed;

									switch_move = !switch_move;
								}

								autostopped = true;
							}
							else
							{
								autostopped = false;
							}
						}
						else
						{
							autostopped = false;
						}
					}
					else
					{
						autostopped = false;
					}
				}
				else
				{
					autostopped = false;
				}

				int hitchance;

				if (Globals::dting)
				{
					bool shots = g_LocalPlayer->m_iShotsFired();

					int shotcounter = 0;

					if (shots)
						shotcounter++;

					if (shotcounter == 1 || shotcounter < 1)
					{
						if (!g_LocalPlayer->m_hActiveWeapon()->IsSniper() && !g_LocalPlayer->m_hActiveWeapon()->IsAuto() && !g_LocalPlayer->m_hActiveWeapon()->IsRifle())
							hitchance = autostopped ? 100 - (g_Options.aimbot.dthc * 0.43) : g_Options.aimbot.dthc * 0.34;
						else
							hitchance = autostopped ? 100 - (g_Options.aimbot.dthc * 0.43) : g_Options.aimbot.dthc * 0.34;
					}

					if (shotcounter >= 2)
					{
						if (!g_LocalPlayer->m_hActiveWeapon()->IsSniper() && !g_LocalPlayer->m_hActiveWeapon()->IsAuto() && !g_LocalPlayer->m_hActiveWeapon()->IsRifle())
							hitchance = autostopped ? 100 - (g_Options.aimbot.dthc * 0.43) : g_Options.aimbot.dthc * 0.34;
						else
							hitchance = autostopped ? 100 - (g_Options.aimbot.dthc * 0.43) : g_Options.aimbot.dthc * 0.34;
						shotcounter = 0;
					}
				}
				else
				{
					if (!g_LocalPlayer->m_hActiveWeapon()->IsSniper() && !g_LocalPlayer->m_hActiveWeapon()->IsAuto() && !g_LocalPlayer->m_hActiveWeapon()->IsRifle())
						hitchance = autostopped ? g_Options.aimbot.dthc * 0.43 : g_Options.aimbot.dthc * 0.34;
					else
						hitchance = autostopped ? g_Options.aimbot.dthc * 0.43 : g_Options.aimbot.dthc * 0.34;
				}

				if (doingthezeus) {

					if (!((target->m_vecOrigin() - g_LocalPlayer->m_vecOrigin()).Length() > 115.f)) {
						cmd->buttons |= IN_ATTACK;
						const float server_time = g_LocalPlayer->m_nTickBase() * g_GlobalVars->interval_per_tick;
						const float next_shot = g_LocalPlayer->m_hActiveWeapon()->m_flNextPrimaryAttack() - server_time;

						if (next_shot >= 1)
							cmd->buttons &= ~IN_ATTACK;
					}
					else
					{
						return;
					}
				}
				else
				{
					if (g_Options.aimbot.hc == true)
					{
						if (g_LocalPlayer->m_hActiveWeapon()->GetInaccuracy() / g_LocalPlayer->m_hActiveWeapon()->GetSpread() <= hitchance)
						{

							if (g_Options.aimbot.autorevolver2) {
								if (g_Options.aimbot.autofire) {
									cmd->buttons |= IN_ATTACK2;
									const float server_time = g_LocalPlayer->m_nTickBase() * g_GlobalVars->interval_per_tick;
									const float next_shot = g_LocalPlayer->m_hActiveWeapon()->m_flNextSecondaryAttack() - server_time;

									if (next_shot >= 1)
										cmd->buttons &= ~IN_ATTACK2;
								}
							}

							if (!g_Options.aimbot.autorevolver2) {
								if (g_Options.aimbot.autofire) {
									cmd->buttons |= IN_ATTACK;
									const float server_time = g_LocalPlayer->m_nTickBase() * g_GlobalVars->interval_per_tick;
									const float next_shot = g_LocalPlayer->m_hActiveWeapon()->m_flNextPrimaryAttack() - server_time;

									if (next_shot >= 1)
										cmd->buttons &= ~IN_ATTACK;
								}
							}
						}
						if (g_LocalPlayer->m_hActiveWeapon()->GetInaccuracy() / g_LocalPlayer->m_hActiveWeapon()->GetSpread() > hitchance) {

							return;

						}
					}
					if (g_Options.aimbot.hc == false) {
						if (g_Options.aimbot.autorevolver2) {
							if (g_Options.aimbot.autofire) {
								cmd->buttons |= IN_ATTACK2;
								const float server_time = g_LocalPlayer->m_nTickBase() * g_GlobalVars->interval_per_tick;
								const float next_shot = g_LocalPlayer->m_hActiveWeapon()->m_flNextSecondaryAttack() - server_time;

								if (next_shot >= 1)
									cmd->buttons &= ~IN_ATTACK2;
							}
						}

						if (!g_Options.aimbot.autorevolver2) {
							if (g_Options.aimbot.autofire) {
								cmd->buttons |= IN_ATTACK;
								const float server_time = g_LocalPlayer->m_nTickBase() * g_GlobalVars->interval_per_tick;
								const float next_shot = g_LocalPlayer->m_hActiveWeapon()->m_flNextPrimaryAttack() - server_time;

								if (next_shot >= 1)
									cmd->buttons &= ~IN_ATTACK;
							}
						}
					}
				}
			}
		}
	}

	if (g_Options.aimbot.autorevolver2) {
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
	Globals::view_punch_old = current_punch;

	if (!IsSilent()) {
		Smooth(current, angles, angles);
		g_EngineClient->SetViewAngles(&angles);
		Globals::abfr = true;
	}
	else if (IsSilent())
	{
		cmd->viewangles = angles;
		Globals::abfr = true;
	}

	if (g_LocalPlayer->m_iShotsFired() < 1)
	{
		g_LocalPlayer->SetVAngles(current);
		Globals::abfr = false;
	}

	if (g_LocalPlayer->m_iShotsFired() >= 1)
	{
		lastShotTick = GetTickCount64();
		shotsFired++;
	}
}