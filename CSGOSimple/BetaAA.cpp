#include "./BetaAA.h"
#include "./options.hpp"
#include "./helpers/math.hpp"
#include "options.hpp"
#include "features/aimbot.hpp"
#include "features/autowall.hpp"
#include "Globals.h"
#include "features/Misc.hpp"

bool CAntiAim::CanDesync(CUserCmd* cmd)
{

	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive())
	{
		return false;
	}

	C_BaseCombatWeapon* weapon = g_LocalPlayer->m_hActiveWeapon().Get();

	if ((Globals::freezetime && !Globals::stepping) || g_LocalPlayer->m_bGunGameImmunity() || g_LocalPlayer->m_fFlags() & FL_FROZEN || !g_Options.ragebot_antiaim_desync || g_Options.fakepreset == 0)
		return false;

	int movetype = g_LocalPlayer->m_nMoveType();

	bool firing = false;

	if (weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER)
	{
		if (cmd->buttons & IN_ATTACK2)
			firing = true;
		else
			firing = false;
	}
	else
	{
		if (cmd->buttons & IN_ATTACK)
			firing = true;
		else
			firing = false;

	}

	if (g_Options.aimbot.fastaimbot && g_Options.aimbot.enabled) {
		if (movetype == MOVETYPE_FLY || movetype == MOVETYPE_NOCLIP || cmd->buttons & IN_USE || cmd->buttons & IN_GRENADE1 || cmd->buttons & IN_GRENADE2)
			return false;
	}
	else {
		if (movetype == MOVETYPE_FLY || movetype == MOVETYPE_NOCLIP || firing || cmd->buttons & IN_USE || cmd->buttons & IN_GRENADE1 || cmd->buttons & IN_GRENADE2)
			return false;
	}

	if (!weapon)
		return false;

	if (movetype == MOVETYPE_LADDER)
		return false;

	if (!weapon->IsGun() && !weapon->IsZeus())
		return false;

	if (weapon->IsGrenade())
		return false;

	return true;
}

void CAntiAim::CreateMove(CUserCmd* cmd, bool& bSendPacket)
{

	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive())
	{
		return;
	}

	if ((Globals::freezetime && !Globals::stepping) || g_LocalPlayer->m_bGunGameImmunity() || g_LocalPlayer->m_fFlags() & FL_FROZEN)
		return;

	int movetype = g_LocalPlayer->m_nMoveType();

	if (g_Options.aimbot.fastaimbot && g_Options.aimbot.enabled) {
		if (movetype == MOVETYPE_FLY || movetype == MOVETYPE_NOCLIP || cmd->buttons & IN_USE || cmd->buttons & IN_GRENADE1 || cmd->buttons & IN_GRENADE2)
			return;
	}
	else {
		if (movetype == MOVETYPE_FLY || movetype == MOVETYPE_NOCLIP || cmd->buttons & IN_ATTACK || cmd->buttons & IN_USE || cmd->buttons & IN_GRENADE1 || cmd->buttons & IN_GRENADE2)
			return;
	}

	C_BaseCombatWeapon* weapon = g_LocalPlayer->m_hActiveWeapon().Get();

	if (!weapon)
		return;

	if (movetype == MOVETYPE_LADDER)
		return;

	if (!weapon->IsGun() && !weapon->IsZeus())
		return;

	if (weapon->IsGrenade())
		return;

	DoAntiAim(cmd, bSendPacket);
}

float WallThickness(Vector from, Vector to, C_BasePlayer* skip, C_BasePlayer* skip2)
{
	Vector endpos1, endpos2;

	Ray_t ray;
	ray.Init(from, to);

	CTraceFilterSkipTwoEntities filter(skip, skip2);

	trace_t trace1, trace2;
	g_EngineTrace->TraceRay(ray, MASK_SHOT_BRUSHONLY, &filter, &trace1);

	if (trace1.DidHit())
		endpos1 = trace1.endpos;
	else
		return -1.f;

	ray.Init(to, from);
	g_EngineTrace->TraceRay(ray, MASK_SHOT_BRUSHONLY, &filter, &trace2);

	if (trace2.DidHit())
		endpos2 = trace2.endpos;

	return endpos1.DistTo(endpos2);
}

void NormalizeNum(Vector& vIn, Vector& vOut)
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

float fov_player(Vector ViewOffSet, QAngle View, C_BasePlayer* entity)
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
	NormalizeNum(Delta, Delta);

	// Get dot product between delta position and directional forward vectors
	FLOAT DotProduct = Forward.Dot(Delta);

	// Time to calculate the field of view
	return (acos(DotProduct) * (MaxDegrees / M_PI));
}

int GetNearestPlayerToCrosshair()
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

		float CFov = fov_player(g_LocalPlayer->m_vecOrigin(), MyAng, entity); //Math::GetFOV(MyAng, Math::CalcAngle(g_LocalPlayer->GetEyePos(), entity->GetEyePos()));

		if (CFov < BestFov)
		{
			BestFov = CFov;
			BestEnt = i;
		}
	}

	return BestEnt;
}

void CAntiAim::DoAntiAim(CUserCmd* cmd, bool& bSendPacket)
{

	if (Globals::freezetime || g_LocalPlayer->m_bGunGameImmunity() || g_LocalPlayer->m_fFlags() & FL_FROZEN)
		return;

	if (!Globals::freezetime && !g_LocalPlayer->m_bGunGameImmunity() && g_LocalPlayer->m_fFlags() != FL_FROZEN)
	{
		Yaw(cmd, false);
		Pitch(cmd, bSendPacket);
	}

	float fakepreset = 0.f;

	bool balls = false;

	if (GetKeyState(g_Options.invertaa))
		balls = true;
	else if (!(GetKeyState(g_Options.invertaa)))
		balls = false;

	if (!g_EngineClient->IsInGame() && !g_LocalPlayer->IsAlive())
		balls = false;

	if (g_Options.fakepreset == 0)
		fakepreset = 0.f;
	if (g_Options.fakepreset == 1)
		fakepreset = balls ? 29.f : -29.f;
	if (g_Options.fakepreset == 2)
		fakepreset = balls ? 58.f : -58.f;
	if (g_Options.fakepreset == 3)
		fakepreset = balls ? g_Options.fakedegree : -g_Options.fakedegree;
	if (g_Options.fakepreset == 4)
		fakepreset = balls ? (fmodf(cmd->tick_count * 6.f, 360.f) * g_Options.fakespinsp) : -(fmodf(cmd->tick_count * 6.f, 360.f) * g_Options.fakespinsp);
	if (g_Options.randomizefake)
		fakepreset = (Math::random_float(-360.f, 360.f));
	sexyboodyballs = fakepreset;
	inverted = balls;

	float best_rotation = 0.f;
	auto local_eyeposition = g_LocalPlayer->GetEyePos();
	auto head_position = g_LocalPlayer->GetHitboxPos(HITBOX_HEAD);
	float thickest = -1.f;

	auto pEntity = C_BasePlayer::GetPlayerByIndex(GetNearestPlayerToCrosshair());

	float step = (DirectX::XM_2PI) / 8.f;

	float radius = fabs(Vector(head_position - g_LocalPlayer->m_vecOrigin()).Length2D());

	for (float rotation = 0; rotation < (DirectX::XM_2PI); rotation += step)
	{
		if (!pEntity || !g_LocalPlayer) continue;
		if (!pEntity->IsPlayer()) continue;
		if (pEntity == g_LocalPlayer) continue;
		if (pEntity->IsDormant()) continue;
		if (!pEntity->IsAlive()) continue;
		if (g_LocalPlayer->m_iTeamNum() == pEntity->m_iTeamNum()) continue;

		Vector newhead(radius * cos(rotation) + local_eyeposition.x, radius * sin(rotation) + local_eyeposition.y, local_eyeposition.z);

		float thickness = WallThickness(pEntity->GetEyePos(), newhead, pEntity, g_LocalPlayer);

		if (thickness > thickest)
		{
			thickest = thickness;
			best_rotation = rotation;
		}
	}

	if (!Globals::freezetime && !g_LocalPlayer->m_bGunGameImmunity() && g_LocalPlayer->m_fFlags() != FL_FROZEN)
	{
		if (g_Options.ragebot_antiaim_yaw == 4 && best_rotation)
			cmd->viewangles.yaw = RAD2DEG(best_rotation);
		else if (g_Options.ragebot_antiaim_yaw == 4 && !best_rotation)
			cmd->viewangles.yaw -= 180.f;

		if (g_Options.ragebot_antiaim_desync)
		{
			auto lby = g_EngineClient->GetNetChannel();

			if (!lby)
				return;

			if (g_Options.fakelag == false || g_Options.faketicks <= 0) {
				if (lby->m_nChokedPackets != true)
				{
					bSendPacket = cmd->command_number % 2 ? true : false;
				}
			}

			float fake = (fakepreset / 3) * 2;
			float real = (fakepreset / 3);

			if (g_Options.ragebot_antiaim_yaw != 5) {
				if (g_Options.fakepreset != 4) {
					if (!bSendPacket)
					{
						cmd->viewangles.yaw += fake;
					}

					if (bSendPacket)
					{
						cmd->viewangles.yaw -= real;
					}
				}
				if (g_Options.fakepreset == 4) {
					if (!bSendPacket)
					{
						cmd->viewangles.yaw += fakepreset;
					}

					if (bSendPacket)
					{
						cmd->viewangles.yaw -= 0.f;
					}
				}
			}
			if (g_Options.ragebot_antiaim_yaw == 5) {
				if (g_Options.fakepreset != 4) {
					if (!bSendPacket)
					{
						cmd->viewangles.yaw += fake;
					}
					if (bSendPacket)
					{
						cmd->viewangles.yaw -= g_Options.customreal;
					}
				}
				if (g_Options.fakepreset == 4) {
					if (!bSendPacket)
					{
						cmd->viewangles.yaw += fakepreset;
					}
					if (bSendPacket)
					{
						cmd->viewangles.yaw -= g_Options.customreal;
					}
				}
			}
		}
	}

}

void CAntiAim::Pitch(CUserCmd* cmd, bool fake)
{

	if (Globals::freezetime || g_LocalPlayer->m_bGunGameImmunity() || g_LocalPlayer->m_fFlags() & FL_FROZEN)
		return;

	static bool bFlip = false;
	bool Moving = g_LocalPlayer->m_vecVelocity().Length2D() > 0.1;
	bool InAir = !(g_LocalPlayer->m_fFlags() & FL_ONGROUND);
	bool Standing = !Moving && !InAir;
	PitchAntiAims mode = (PitchAntiAims)g_Options.ragebot_antiaim_pitch;

	bool headthing = false;
	if (Globals::headthing && g_Options.headthing)
		headthing = true;
	else
		headthing = false;

	if (Globals::valve)
	{
		fake = false;
	}
	else
	{
		fake = fake;
	}

	float CustomPitch = 0.f;

	switch (mode)
	{
	case PitchAntiAims::EMOTION:
		cmd->viewangles.pitch = headthing ? 0.f : 58.f;
		break;

	case PitchAntiAims::DOWN:
		cmd->viewangles.pitch = headthing ? 0.f : 89.f;
		break;

	case PitchAntiAims::UP:
		cmd->viewangles.pitch = headthing ? 0.f : -89.f;
		break;

	case PitchAntiAims::ZERO:
		cmd->viewangles.pitch = 0.f;
		break;
	case PitchAntiAims::FUP:
		cmd->viewangles.pitch = fake ? 89.f : (headthing ? 0.f : -89.f);
		break;
	case PitchAntiAims::FDOWN:
		cmd->viewangles.pitch = fake ? -89.f : (headthing ? 0.f : 89.f);
		break;
	case PitchAntiAims::FZERO:
		cmd->viewangles.pitch = fake ? 89.f : 0.f;
		break;
	}

}

void CAntiAim::Yaw(CUserCmd* cmd, bool fake)
{
	if (Globals::freezetime || g_LocalPlayer->m_bGunGameImmunity() || g_LocalPlayer->m_fFlags() & FL_FROZEN)
		return;

	fake = g_Options.ragebot_antiaim_desync;
	bool Moving = g_LocalPlayer->m_vecVelocity().Length2D() > 0.1;
	bool InAir = !(g_LocalPlayer->m_fFlags() & FL_ONGROUND);
	bool Standing = !Moving && !InAir;
	YawAntiAims mode = (YawAntiAims)g_Options.ragebot_antiaim_yaw;

	bool balls = false;

	if (GetKeyState(g_Options.invertaa))
		balls = true;
	else if (!(GetKeyState(g_Options.invertaa)))
		balls = false;

	if (!g_EngineClient->IsInGame() && !g_LocalPlayer->IsAlive())
		balls = false;

	switch (mode)
	{
	case YawAntiAims::BACKWARDS:
		cmd->viewangles.yaw -= 180.f;
		break;

	case YawAntiAims::SPINBOT:
		cmd->viewangles.yaw += balls ? (fmodf(cmd->tick_count * 6.f, 360.f) * g_Options.spinspeed) : -(fmodf(cmd->tick_count * 6.f, 360.f) * g_Options.spinspeed);
		break;

	case YawAntiAims::LOWER_BODY:
		cmd->viewangles.yaw = g_LocalPlayer->m_flLowerBodyYawTarget();
		break;

	case YawAntiAims::CUSTOM:
		cmd->viewangles.yaw += g_Options.customreal;
		break;
	case YawAntiAims::JITTER:
		cmd->viewangles.yaw += cmd->command_number % 2 ? -180 - g_Options.jitterdegree : 180 + g_Options.jitterdegree;
		break;
	}
}