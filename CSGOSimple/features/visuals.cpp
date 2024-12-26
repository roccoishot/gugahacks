#include <algorithm>

#include "visuals.hpp"

#include "../options.hpp"
#include "../helpers/math.hpp"
#include "../helpers/utils.hpp"
#include "../menu.hpp"
#include "../Globals.h"
#include "../xor.h"
#include "../backtrack.h"

RECT GetBBox(C_BaseEntity* ent)
{
	RECT rect{};
	auto collideable = ent->GetCollideable();

	if (!collideable)
		return rect;

	auto min = collideable->OBBMins();
	auto max = collideable->OBBMaxs();

	const matrix3x4_t& trans = ent->m_rgflCoordinateFrame();

	Vector points[] = {
		Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),
		Vector(max.x, max.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(min.x, min.y, max.z),
		Vector(max.x, min.y, max.z)
	};

	Vector pointsTransformed[8];
	for (int i = 0; i < 8; i++) {
		Math::VectorTransform(points[i], trans, pointsTransformed[i]);
	}

	Vector screen_points[8] = {};

	for (int i = 0; i < 8; i++) {
		if (!Math::WorldToScreen(pointsTransformed[i], screen_points[i]))
			return rect;
	}

	auto left = screen_points[0].x;
	auto top = screen_points[0].y;
	auto right = screen_points[0].x;
	auto bottom = screen_points[0].y;

	for (int i = 1; i < 8; i++) {
		if (left > screen_points[i].x)
			left = screen_points[i].x;
		if (top < screen_points[i].y)
			top = screen_points[i].y;
		if (right < screen_points[i].x)
			right = screen_points[i].x;
		if (bottom > screen_points[i].y)
			bottom = screen_points[i].y;
	}
	return RECT{ (long)left, (long)top, (long)right, (long)bottom };
}

Visuals::Visuals()
{
	InitializeCriticalSection(&cs);
}

Visuals::~Visuals() {
	DeleteCriticalSection(&cs);
}

//--------------------------------------------------------------------------------
void Visuals::Render() {
}
//--------------------------------------------------------------------------------
int flPlayerAlpha[64];
int flAlphaFade = 5.f;

bool Visuals::Player::Begin(C_BasePlayer* pl)
{

	if (!pl)
		return false;

	if (!pl->IsAlive())
		return false;


	if (!pl->valid(false, false))
		return false;

	Vector rape{ -1, -1, -1 };
	Vector rape2{ -1, -1, -1, };
	Math::WorldToScreen(pl->m_vecOrigin(), rape);
	if (rape != rape2) {

		if (g_Options.dormantesp == false) {
			if (pl->IsDormant() && flPlayerAlpha[pl->EntIndex()] > 0)
			{
				flPlayerAlpha[pl->EntIndex()] -= flAlphaFade;
			}
			else if (flPlayerAlpha[pl->EntIndex()] < 255 && !(pl->IsDormant()))
			{
				flPlayerAlpha[pl->EntIndex()] += flAlphaFade;
			}

			if (pl->IsDormant())
				return false;
		}
		if (g_Options.dormantesp == true) {
			if (pl->IsDormant() && flPlayerAlpha[pl->EntIndex()] > 0)
			{
				flPlayerAlpha[pl->EntIndex()] -= flAlphaFade;
			}
			else if (flPlayerAlpha[pl->EntIndex()] < 255 && !(pl->IsDormant()))
			{
				flPlayerAlpha[pl->EntIndex()] += flAlphaFade;
			}

			if (flPlayerAlpha <= 0 && pl->IsDormant())
				return false;
		}
		ctx.pl = pl;
		ctx.is_enemy = g_LocalPlayer->m_iTeamNum() != pl->m_iTeamNum();

		for (int i = 0; i <= 18; i++)
		{
			ctx.is_visible = g_LocalPlayer->CanSeePlayer(pl, i);
		}


		if (!ctx.is_enemy)
			return false;

		if (!ctx.is_visible && !g_Options.esp_player_boxesOccluded)
			return false;

		//ctx.clr = ctx.is_enemy ? (ctx.is_visible ? g_Options.color_esp_enemy_visible : g_Options.color_esp_enemy_occluded) : (ctx.is_visible ? g_Options.color_esp_ally_visible : g_Options.color_esp_ally_occluded);

		auto head = pl->GetHitboxPos(HITBOX_HEAD);
		auto origin = pl->m_vecOrigin();

		if (!Math::WorldToScreen(head, ctx.head_pos) ||
			!Math::WorldToScreen(origin, ctx.feet_pos))
			return false;

		ctx.bbox = GetBBox(pl);

		std::swap(ctx.bbox.top, ctx.bbox.bottom);

		return !(!ctx.bbox.left || !ctx.bbox.top || !ctx.bbox.right || !ctx.bbox.bottom);
	}
}
//--------------------------------------------------------------------------------
void Visuals::Player::RenderBox(C_BaseEntity* pl) {


	Vector rape{ -1, -1, -1 };
	Vector rape2{ -1, -1, -1, };
	Math::WorldToScreen(pl->m_vecOrigin(), rape);
	if (rape != rape2) {

		if (!ctx.is_visible && g_Options.esp_player_boxesOccluded) {
			Render::Get().RenderBoxByType(ctx.bbox.left, ctx.bbox.top, ctx.bbox.right, ctx.bbox.bottom, g_Options.color_esp_enemy_occluded, 1.f, g_Options.boxtype);
			Render::Get().RenderBoxByType(ctx.bbox.left - 1, ctx.bbox.top - 1, ctx.bbox.right + 1, ctx.bbox.bottom + 1, Color(0, 0, 0, g_Options.color_esp_enemy_occluded.a()), 1.0f, g_Options.boxtype);
			Render::Get().RenderBoxByType(ctx.bbox.left + 1, ctx.bbox.top + 1, ctx.bbox.right - 1, ctx.bbox.bottom - 1, Color(0, 0, 0), 1.0f, g_Options.boxtype);
		}
		if (ctx.is_visible) {
			Render::Get().RenderBoxByType(ctx.bbox.left, ctx.bbox.top, ctx.bbox.right, ctx.bbox.bottom, g_Options.color_esp_enemy_visible, 1.f, g_Options.boxtype);
			Render::Get().RenderBoxByType(ctx.bbox.left - 1, ctx.bbox.top - 1, ctx.bbox.right + 1, ctx.bbox.bottom + 1, Color(0, 0, 0, g_Options.color_esp_enemy_visible.a()), 1.0f, g_Options.boxtype);
			Render::Get().RenderBoxByType(ctx.bbox.left + 1, ctx.bbox.top + 1, ctx.bbox.right - 1, ctx.bbox.bottom - 1, Color(0, 0, 0, g_Options.color_esp_enemy_visible.a()), 1.0f, g_Options.boxtype);
		}
	}
}

void Visuals::Player::Skemlanton()
{


	Vector rape{ -1, -1, -1 };
	Vector rape2{ -1, -1, -1, };
	Math::WorldToScreen(ctx.pl->m_vecOrigin(), rape);
	if (rape != rape2) {
	auto chest = ctx.pl->GetHitboxPos(HITBOX_UPPER_CHEST);
	Vector Chestpos;
	auto rarm = ctx.pl->GetHitboxPos(HITBOX_RIGHT_UPPER_ARM);
	Vector Rightarm;
	auto larm = ctx.pl->GetHitboxPos(HITBOX_LEFT_UPPER_ARM);
	Vector Leftarm;
	auto forearmr = ctx.pl->GetHitboxPos(HITBOX_RIGHT_FOREARM);
	Vector ForeRight;
	auto forearml = ctx.pl->GetHitboxPos(HITBOX_LEFT_FOREARM);
	Vector ForeLeft;
	auto rhand = ctx.pl->GetHitboxPos(HITBOX_RIGHT_HAND);
	Vector RHand;
	auto lhand = ctx.pl->GetHitboxPos(HITBOX_LEFT_HAND);
	Vector LHand;
	auto penis = ctx.pl->GetHitboxPos(HITBOX_PELVIS);
	Vector Pelvis;
	auto lleg = ctx.pl->GetHitboxPos(HITBOX_LEFT_THIGH);
	Vector ThighL;
	auto rleg = ctx.pl->GetHitboxPos(HITBOX_RIGHT_THIGH);
	Vector ThighR;
	auto lfeetsie = ctx.pl->GetHitboxPos(HITBOX_LEFT_FOOT);
	Vector LFoot;
	auto rfeetsie = ctx.pl->GetHitboxPos(HITBOX_RIGHT_FOOT);
	Vector RFoot;
	auto Lcalf = ctx.pl->GetHitboxPos(HITBOX_LEFT_CALF);
	Vector COWL;
	auto Rcalf = ctx.pl->GetHitboxPos(HITBOX_RIGHT_CALF);
	Vector COWR;

	Math::WorldToScreen(chest, Chestpos);
	Math::WorldToScreen(rarm, Rightarm);
	Math::WorldToScreen(larm, Leftarm);
	Math::WorldToScreen(rhand, RHand);
	Math::WorldToScreen(lhand, LHand);
	Math::WorldToScreen(penis, Pelvis);
	Math::WorldToScreen(lleg, ThighL);
	Math::WorldToScreen(rleg, ThighR);
	Math::WorldToScreen(rfeetsie, RFoot);
	Math::WorldToScreen(lfeetsie, LFoot);
	Math::WorldToScreen(forearmr, ForeRight);
	Math::WorldToScreen(forearml, ForeLeft);
	Math::WorldToScreen(Lcalf, COWL);
	Math::WorldToScreen(Rcalf, COWR);

	if (ctx.is_visible) {
		Render::Get().RenderLine(ctx.head_pos.x, ctx.head_pos.y, Chestpos.x, Chestpos.y, g_Options.color_skemletom, g_Options.skelethicc);
		Render::Get().RenderLine(Chestpos.x, Chestpos.y, Rightarm.x, Rightarm.y, g_Options.color_skemletom, g_Options.skelethicc);
		Render::Get().RenderLine(Chestpos.x, Chestpos.y, Leftarm.x, Leftarm.y, g_Options.color_skemletom, g_Options.skelethicc);
		Render::Get().RenderLine(Rightarm.x, Rightarm.y, ForeRight.x, ForeRight.y, g_Options.color_skemletom, g_Options.skelethicc);
		Render::Get().RenderLine(Leftarm.x, Leftarm.y, ForeLeft.x, ForeLeft.y, g_Options.color_skemletom, g_Options.skelethicc);
		Render::Get().RenderLine(ForeRight.x, ForeRight.y, RHand.x, RHand.y, g_Options.color_skemletom, g_Options.skelethicc);
		Render::Get().RenderLine(ForeLeft.x, ForeLeft.y, LHand.x, LHand.y, g_Options.color_skemletom, g_Options.skelethicc);
		Render::Get().RenderLine(Chestpos.x, Chestpos.y, Pelvis.x, Pelvis.y, g_Options.color_skemletom, g_Options.skelethicc);
		Render::Get().RenderLine(Pelvis.x, Pelvis.y, ThighR.x, ThighR.y, g_Options.color_skemletom, g_Options.skelethicc);
		Render::Get().RenderLine(Pelvis.x, Pelvis.y, ThighL.x, ThighL.y, g_Options.color_skemletom, g_Options.skelethicc);
		Render::Get().RenderLine(ThighL.x, ThighL.y, COWL.x, COWL.y, g_Options.color_skemletom, g_Options.skelethicc);
		Render::Get().RenderLine(ThighR.x, ThighR.y, COWR.x, COWR.y, g_Options.color_skemletom, g_Options.skelethicc);
		Render::Get().RenderLine(COWL.x, COWL.y, LFoot.x, LFoot.y, g_Options.color_skemletom, g_Options.skelethicc);
		Render::Get().RenderLine(COWR.x, COWR.y, RFoot.x, RFoot.y, g_Options.color_skemletom, g_Options.skelethicc);
	}
	if (!ctx.is_visible && g_Options.esp_player_boxesOccluded) {
		Render::Get().RenderLine(ctx.head_pos.x, ctx.head_pos.y, Chestpos.x, Chestpos.y, g_Options.color_skemletom, g_Options.skelethicc);
		Render::Get().RenderLine(Chestpos.x, Chestpos.y, Rightarm.x, Rightarm.y, g_Options.color_skemletom, g_Options.skelethicc);
		Render::Get().RenderLine(Chestpos.x, Chestpos.y, Leftarm.x, Leftarm.y, g_Options.color_skemletom, g_Options.skelethicc);
		Render::Get().RenderLine(Rightarm.x, Rightarm.y, ForeRight.x, ForeRight.y, g_Options.color_skemletom, g_Options.skelethicc);
		Render::Get().RenderLine(Leftarm.x, Leftarm.y, ForeLeft.x, ForeLeft.y, g_Options.color_skemletom, g_Options.skelethicc);
		Render::Get().RenderLine(ForeRight.x, ForeRight.y, RHand.x, RHand.y, g_Options.color_skemletom, g_Options.skelethicc);
		Render::Get().RenderLine(ForeLeft.x, ForeLeft.y, LHand.x, LHand.y, g_Options.color_skemletom, g_Options.skelethicc);
		Render::Get().RenderLine(Chestpos.x, Chestpos.y, Pelvis.x, Pelvis.y, g_Options.color_skemletom, g_Options.skelethicc);
		Render::Get().RenderLine(Pelvis.x, Pelvis.y, ThighR.x, ThighR.y, g_Options.color_skemletom, g_Options.skelethicc);
		Render::Get().RenderLine(Pelvis.x, Pelvis.y, ThighL.x, ThighL.y, g_Options.color_skemletom, g_Options.skelethicc);
		Render::Get().RenderLine(ThighL.x, ThighL.y, COWL.x, COWL.y, g_Options.color_skemletom, g_Options.skelethicc);
		Render::Get().RenderLine(ThighR.x, ThighR.y, COWR.x, COWR.y, g_Options.color_skemletom, g_Options.skelethicc);
		Render::Get().RenderLine(COWL.x, COWL.y, LFoot.x, LFoot.y, g_Options.color_skemletom, g_Options.skelethicc);
		Render::Get().RenderLine(COWR.x, COWR.y, RFoot.x, RFoot.y, g_Options.color_skemletom, g_Options.skelethicc);
	}
	}
}

void Visuals::Player::Drugs()
{
	auto base = ctx.pl->GetHitboxPos(HITBOX_UPPER_CHEST);
	Vector origin;

	Math::WorldToScreen(base, origin);

	int screenWidth, screenHeight;
	g_EngineClient->GetScreenSize(screenWidth, screenHeight);

	if (ctx.pl && ctx.pl->EntIndex() != g_LocalPlayer->EntIndex() && ctx.pl->IsEnemy())
	{

		Vector rape{ -1, -1, -1 };
		Vector rape2{ -1, -1, -1, };
		Math::WorldToScreen(ctx.pl->m_vecOrigin(), rape);
		if (rape != rape2) {

			if (ctx.is_visible) {
				Render::Get().RenderLine((float)screenWidth / 2, (float)screenHeight / 2, origin.x, origin.y, g_Options.color_cumming, g_Options.snapthicc);
			}
			if (!ctx.is_visible && g_Options.esp_player_boxesOccluded) {
				Render::Get().RenderLine((float)screenWidth / 2, (float)screenHeight / 2, origin.x, origin.y, g_Options.color_cumming, g_Options.snapthicc);
			}
		}
	}
}

//--------------------------------------------------------------------------------
//NAME COLORSSSS
void Visuals::Player::RenderName(C_BaseEntity* pl)
{
	player_info_t info = ctx.pl->GetPlayerInfo();

	auto sz = g_pDefaultFont->CalcTextSizeA(12.f, FLT_MAX, 0.0f, info.szName);

	Vector rape{ -1, -1, -1 };
	Vector rape2{ -1, -1, -1, };
	Math::WorldToScreen(ctx.pl->m_vecOrigin(), rape);
	if (rape != rape2) {

		//	Render::Get().RenderText(info.szName, ctx.feet_pos.x - sz.x / 2, ctx.head_pos.y - sz.y, 12.f, Color::White);
		if (ctx.pl->m_iHealth() && ctx.pl->m_iHealth() == 1 && g_Options.flashkillcheck)
			Render::Get().RenderText(info.szName, ctx.bbox.left + (ctx.bbox.right - ctx.bbox.left - sz.x) / 2, (ctx.bbox.top - sz.y - 1), 0.f, Color(255, 0, 0), false, true, g_pDefaultFont);
		else
			Render::Get().RenderText(info.szName, ctx.bbox.left + (ctx.bbox.right - ctx.bbox.left - sz.x) / 2, (ctx.bbox.top - sz.y - 1), 0.f, Color(g_Options.color_name_player), false, true, g_pDefaultFont);
	}
}

void Visuals::Player::BruhTubb()
{

	if (g_Options.aimbot.backtrack && g_Options.showbt)
	{
		for (int t = 0; t < static_cast<int>(NUM_OF_TICKS); ++t)
		{
			if (Globals::bestbtid > -1) {
				Vector sex;
				Math::WorldToScreen(TimeWarp::Get().TimeWarpData[Globals::bestbtid][t].hitboxPos, sex);

				Render::Get().RenderCircleFilled(sex.x, sex.y, 3.f, 48, g_Options.color_backtrackind);

			}

		}
	}
}

//--------------------------------------------------------------------------------
void Visuals::Player::RenderHealth(C_BaseEntity* pl)
{
	auto  hp = ctx.pl->m_iHealth();
	float box_h = (float)fabs(ctx.bbox.bottom - ctx.bbox.top);
	//float off = (box_h / 6.f) + 5;
	float off = 8;

	int height = (box_h * hp) / 100;

	int x = ctx.bbox.left - off;
	int y = ctx.bbox.top;
	int w = 4;
	int h = box_h;

	int r = 255 - (hp * 2.55f);
	int g = hp * 2.55f;

	Vector rape{ -1, -1, -1 };
	Vector rape2{ -1, -1, -1, };
	Math::WorldToScreen(pl->m_vecOrigin(), rape);
	if (rape != rape2) {

		Render::Get().RenderBox(x, y - 1, x + w, y + h + 1, Color(0, 0, 0));
		Render::Get().RenderBox(x + 1, y + h - height, x + w - 1, y + h, Color(r, g, 0));
	}

}
//--------------------------------------------------------------------------------
void Visuals::Player::RenderArmour()
{
	auto  armour = ctx.pl->m_ArmorValue();
	float box_h = (float)fabs(ctx.bbox.bottom - ctx.bbox.top);
	//float off = (box_h / 6.f) + 5;
	float off = 4;

	int height = (((box_h * armour) / 100));

	int x = ctx.bbox.right + off;
	int y = ctx.bbox.top;
	int w = 4;
	int h = box_h;

	Render::Get().RenderBox(x, y, x + w, y + h, Color(0, 0, 0, g_Options.color_armour_player.a()), 1.f, true);
	Render::Get().RenderBox(x + 1, y + 1, x + w - 1, y + height - 2, Color(g_Options.color_armour_player), 1.f, true);
}
//--------------------------------------------------------------------------------
void Visuals::Player::RenderWeaponName(C_BaseEntity* pl)
{
	auto weapon = ctx.pl->m_hActiveWeapon().Get();

	if (!weapon) return;
	if (!weapon->GetCSWeaponData()) return;

	auto text = weapon->GetCSWeaponData()->szWeaponName + 7;
	auto sz = g_pDefaultFont->CalcTextSizeA(12.f, FLT_MAX, 0.0f, text);
	//Render::Get().RenderText(text, ctx.feet_pos.x, ctx.feet_pos.y, 12.f, Color::White, true,g_pDefaultFont);
	Render::Get().RenderText(text, ImVec2(ctx.bbox.left + (ctx.bbox.right - ctx.bbox.left - sz.x) / 2, ctx.bbox.bottom + 1), 12.f, Color(g_Options.color_esp_weapons), false);
}

CInput* m_input() {
	if (!g_Input)
		g_Input = *reinterpret_cast<CInput**>(Utils::PatternScan2(XorStr("client.dll"), "B9 ? ? ? ? F3 0F 11 04 24 FF 50 10") + 0x1);

	return g_Input;
}

//--------------------------------------------------------------------------------
void Visuals::Player::RenderSnapline()
{

	auto cmd = Globals::m_cmd;

	if (!g_LocalPlayer)
		return;

	if (!g_LocalPlayer->IsAlive())
		return;

	if (!m_input()->m_fCameraInThirdPerson)
		return;

	if (Globals::freezetime || g_LocalPlayer->m_fFlags() & FL_FROZEN || g_LocalPlayer->m_bGunGameImmunity())
		return;

	int movetype = g_LocalPlayer->m_nMoveType();

	if (movetype == MOVETYPE_FLY || movetype == MOVETYPE_NOCLIP || cmd->buttons & IN_USE || cmd->buttons & IN_GRENADE1 || cmd->buttons & IN_GRENADE2)
		return;

	C_BaseCombatWeapon* weapon = g_LocalPlayer->m_hActiveWeapon().Get();

	if (movetype == MOVETYPE_LADDER)
		return;

	if (!weapon)
		return;

	if (!weapon->IsGun() && !weapon->IsZeus())
		return;

	bool inverted = false;

	if (GetKeyState(g_Options.invertaa) && g_Options.ragebot_antiaim_desync && g_EngineClient->IsInGame() && g_LocalPlayer->IsAlive()) {
		inverted = true;
	}
	else {
		inverted = false;
	}

	auto origin = g_LocalPlayer->m_vecOrigin();

	if (g_Options.indicatorhb == 1)
		origin = g_LocalPlayer->m_vecOrigin();
	if (g_Options.indicatorhb == 2)
		origin = g_LocalPlayer->GetHitboxPos(HITBOX_RIGHT_UPPER_ARM);
	if (g_Options.indicatorhb == 3)
		origin = g_LocalPlayer->GetHitboxPos(HITBOX_LEFT_UPPER_ARM);
	if (g_Options.indicatorhb == 4)
		origin = g_LocalPlayer->GetHitboxPos(HITBOX_HEAD);

	Vector area;
	
	Math::WorldToScreen(origin, area);

	if ((g_Options.ragebot_antiaim_yaw != 1) || (g_Options.ragebot_antiaim_yaw == 4))
		inverted = !inverted;
	else
		inverted = inverted;

	const char* cummingornah = inverted ? "<---- Left" : "Right ---->";
	const char* dicking = inverted ? "Inverted" : " ";

	int screen_w, screen_h;
	g_EngineClient->GetScreenSize(screen_w, screen_h);

	if (g_Options.indicatorhb > 0)
	{
		if (g_Options.ragebot_antiaim_yaw != 4) {
			if ((g_Options.ragebot_antiaim_desync && g_Options.fakepreset > 0) || g_Options.ragebot_antiaim_yaw == 2)
				Render::Get().RenderText(XorStr(cummingornah), area.x, area.y, 16.f, g_Options.menucolor, false, true, SHitted);
		}
		if (g_Options.ragebot_antiaim_yaw == 4) {
			if (g_Options.ragebot_antiaim_desync && g_Options.fakepreset > 0 || (g_Options.ragebot_antiaim_yaw == 5))
				Render::Get().RenderText(XorStr(dicking), area.x, area.y, 16.f, g_Options.menucolor, false, true, SHitted);
		}
	}
}
//--------------------------------------------------------------------------------
void Visuals::RenderWeapon(C_BaseCombatWeapon* ent)
{

	auto clean_item_name = [](const char* name) -> const char* {
		if (name[0] == 'C')
			name++;

		auto start = strstr(name, "Weapon");
		if (start != nullptr)
			name = start + 6;

		return name;
	};

	// We don't want to Render weapons that are being held
	if (ent->m_hOwnerEntity().IsValid())
		return;

	Vector rape{ -1, -1, -1 };
	Vector rape2{ -1, -1, -1, };
	Math::WorldToScreen(ent->m_vecOrigin(), rape);
	if (rape != rape2) {
		auto bbox = GetBBox(ent);

		if (bbox.right == 0 || bbox.bottom == 0)
			return;

		//Render::Get().RenderBox(bbox, g_Options.color_esp_weapons);


		auto name = clean_item_name(ent->GetClientClass()->m_pNetworkName);

		auto sz = g_pDefaultFont->CalcTextSizeA(12.f, FLT_MAX, 0.0f, name);
		int w = bbox.right - bbox.left;


		Render::Get().RenderText(name, ImVec2((bbox.left + w * 0.5f) - sz.x * 0.5f, bbox.bottom + 1), 12.f, Color(g_Options.color_esp_weapons));
	}
}

//--------------------------------------------------------------------------------
void Visuals::RenderDefuseKit(C_BaseEntity* ent)
{
	if (ent->m_hOwnerEntity().IsValid())
		return;

	Vector rape{ -1, -1, -1 };
	Vector rape2{ -1, -1, -1, };
	Math::WorldToScreen(ent->m_vecOrigin(), rape);
	if (rape != rape2) {

		auto bbox = GetBBox(ent);

		if (bbox.right == 0 || bbox.bottom == 0)
			return;

		Render::Get().RenderBox(bbox, Color(255, 255, 255));

		auto name = "Defuse Kit";
		auto sz = g_pDefaultFont->CalcTextSizeA(12.f, FLT_MAX, 0.0f, name);
		int w = bbox.right - bbox.left;
		Render::Get().RenderText(name, ImVec2((bbox.left + w * 0.5f) - sz.x * 0.5f, bbox.bottom + 1), 12.f, Color::White);
	}
}

void Visuals::Player::sexdick(C_BasePlayer* ent)
{

	if (!g_LocalPlayer->IsAlive())
		return;

	if (ent && ent->IsAlive() && ent->IsEnemy()) {
		QAngle eyeangles;
		Vector balls;
		int screen_w, screen_h;
		g_EngineClient->GetScreenSize(screen_w, screen_h);
		g_EngineClient->GetViewAngles(&eyeangles);
		QAngle newangle = Math::CalcAngle(Vector(g_LocalPlayer->abs_origin().x, g_LocalPlayer->abs_origin().y, 0), Vector(ent->abs_origin().x, ent->abs_origin().y, 0));
		Math::AngleVectors(QAngle(0, 270, 0) - newangle + QAngle(0, eyeangles.yaw, 0), balls);
		auto circlevec = Vector(screen_w / 2, screen_h / 2, 0) + (balls * 250.f);
		Render::Get().RenderCircleFilled(circlevec.x, circlevec.y, 13, 25, ent->IsDormant() ? Color(100, 100, 100, 70) : Color(g_Options.color_indicator.r(), g_Options.color_indicator.g(), g_Options.color_indicator.b(), 100));
	}
}

//--------------------------------------------------------------------------------
void Visuals::RenderPlantedC4(C_BaseEntity* ent)
{
	auto bbox = GetBBox(ent);

	if (bbox.right == 0 || bbox.bottom == 0)
		return;


	Vector rape{ -1, -1, -1 };
	Vector rape2{ -1, -1, -1, };
	Math::WorldToScreen(ent->m_vecOrigin(), rape);
	if (rape != rape2) {

		Render::Get().RenderBox(bbox, Color(255, 255, 255));


		int bombTimer = std::ceil(ent->m_flC4Blow() - g_GlobalVars->curtime);
		std::string timer = std::to_string(bombTimer);

		auto name = (bombTimer < 0.f) ? "Bomb" : timer;
		auto sz = g_pDefaultFont->CalcTextSizeA(12.f, FLT_MAX, 0.0f, name.c_str());
		int w = bbox.right - bbox.left;

		Render::Get().RenderText(name, ImVec2((bbox.left + w * 0.5f) - sz.x * 0.5f, bbox.bottom + 1), 12.f, Color(255, 255, 255));
	}
}
//--------------------------------------------------------------------------------
void Visuals::RenderItemEsp(C_BaseEntity* ent)
{
	std::string itemstr = "Undefined";
	const model_t* itemModel = ent->GetModel();
	if (!itemModel)
		return;
	studiohdr_t* hdr = g_MdlInfo->GetStudiomodel(itemModel);
	if (!hdr)
		return;

	Vector rape{ -1, -1, -1 };
	Vector rape2{ -1, -1, -1, };
	Math::WorldToScreen(ent->m_vecOrigin(), rape);
	if (rape != rape2) {

		itemstr = hdr->szName;
		if (ent->GetClientClass()->m_ClassID == ClassId_CBumpMine)
			itemstr = "Bump Mine";
		else if (itemstr.find("case_pistol") != std::string::npos)
			itemstr = "Pistol Case";
		else if (itemstr.find("case_light_weapon") != std::string::npos)
			itemstr = "Light Case";
		else if (itemstr.find("case_heavy_weapon") != std::string::npos)
			itemstr = "Heavy Case";
		else if (itemstr.find("case_explosive") != std::string::npos)
			itemstr = "Explosive Case";
		else if (itemstr.find("case_tools") != std::string::npos)
			itemstr = "Tools Case";
		else if (itemstr.find("random") != std::string::npos)
			itemstr = "Airdrop";
		else if (itemstr.find("dz_armor_helmet") != std::string::npos)
			itemstr = "Full Armor";
		else if (itemstr.find("dz_helmet") != std::string::npos)
			itemstr = "Helmet";
		else if (itemstr.find("dz_armor") != std::string::npos)
			itemstr = "Armor";
		else if (itemstr.find("upgrade_tablet") != std::string::npos)
			itemstr = "Tablet Upgrade";
		else if (itemstr.find("briefcase") != std::string::npos)
			itemstr = "Briefcase";
		else if (itemstr.find("parachutepack") != std::string::npos)
			itemstr = "Parachute";
		else if (itemstr.find("dufflebag") != std::string::npos)
			itemstr = "Cash Dufflebag";
		else if (itemstr.find("ammobox") != std::string::npos)
			itemstr = "Ammobox";
		else if (itemstr.find("dronegun") != std::string::npos)
			itemstr = "Turrel";
		else if (itemstr.find("exojump") != std::string::npos)
			itemstr = "Exojump";
		else if (itemstr.find("healthshot") != std::string::npos)
			itemstr = "Healthshot";
		else {
			/*May be you will search some missing items..*/
			/*static std::vector<std::string> unk_loot;
			if (std::find(unk_loot.begin(), unk_loot.end(), itemstr) == unk_loot.end()) {
				Utils::ConsolePrint(itemstr.c_str());
				unk_loot.push_back(itemstr);
			}*/
			return;
		}

		auto bbox = GetBBox(ent);
		if (bbox.right == 0 || bbox.bottom == 0)
			return;
		auto sz = g_pDefaultFont->CalcTextSizeA(12.f, FLT_MAX, 0.0f, itemstr.c_str());
		int w = bbox.right - bbox.left;


		//Render::Get().RenderBox(bbox, g_Options.color_esp_item);
		Render::Get().RenderText(itemstr, ImVec2((bbox.left + w * 0.5f) - sz.x * 0.5f, bbox.bottom + 1), 12.f, Color(g_Options.color_dropped_weapons));
	}
}


void Visuals::ThirdPerson() {
	bool ThirdPersponToggle = true;
	if (!g_LocalPlayer) return;

	if (GetKeyState(g_Options.misc_thirdperson_key))
		ThirdPersponToggle = !ThirdPersponToggle;

	if (g_Options.misc_thirdperson && g_LocalPlayer->IsAlive() && ThirdPersponToggle) // V Key
	{
		if (!m_input()->m_fCameraInThirdPerson)
			m_input()->m_fCameraInThirdPerson = true;
	}
	else
		m_input()->m_fCameraInThirdPerson = false;

	static auto require_reset = false;

	bool fat = false;

	if (g_LocalPlayer->IsAlive())
	{
		require_reset = false;
		return;
	}
	if (g_Options.thirdinspec) {
		if (require_reset)
			g_LocalPlayer->m_iObserverMode() = OBS_MODE_CHASE;

		if (g_LocalPlayer->m_iObserverMode() == OBS_MODE_IN_EYE)
			require_reset = true;

		fat = true;
	}
	else
	{
		if (fat == true) {
			g_LocalPlayer->m_iObserverMode() = OBS_MODE_IN_EYE;
			require_reset = false;
			fat = false;
		}
	}
}

bool lastvelsaved = false; //saver 
int lastjump, lastvel, lasttick = 0; // last vel holder 
std::string drawvel; //text drawer holder 
std::string drawvel2;
void speed()
{
	if (!g_Options.Velocity)
		return;

	if (!g_LocalPlayer)
		return;

	int screenWidth, screenHeight;
	g_EngineClient->GetScreenSize(screenWidth, screenHeight);

	auto local_player = g_LocalPlayer;

	Vector speed = local_player->m_vecVelocity();
	int intspeed = round(speed.Length2D());

	const float delta = intspeed - lastvel;

	std::string vel = std::to_string(intspeed);

	if (local_player->m_fFlags() & FL_ONGROUND)
	{
		if (lastvelsaved)
		{
			lastvelsaved = false;
		}

		drawvel = vel;
	}
	else
	{
		if (!lastvelsaved)
		{
			lastjump = intspeed;
			lastvelsaved = true;
		}
		drawvel = vel + " (" + std::to_string(lastjump) + ")";
	}
	drawvel2 = "(" + std::to_string(lastjump) + ")";

	if (local_player->m_nMoveType() == MOVETYPE_NOCLIP) {

	}
	else {

		//	if (intspeed >= 280)
			//	g_Render->RenderText(std::to_string(intspeed), screenWidth / 2, screenHeight - 100, 27.f, Color(170, 255, 0), false, false, g_VeloFont);


		//	else
		if (g_Options.outline)
			Render::Get().RenderText(std::to_string(intspeed), screenWidth / 2, screenHeight - 100, 27.f, Color(0, 0, 0, 255), false, true, g_VeloFont);

		Render::Get().RenderText(std::to_string(intspeed), screenWidth / 2, screenHeight - 100, 27.f, g_Options.Velocitycol, false, false, g_VeloFont);

		if (!(local_player->m_fFlags() & FL_ONGROUND)) {
			//if (lastjump >= 270)
			//	g_Render->RenderText(drawvel2, screenWidth / 2+ 50, screenHeight - 100, 27.f, Color(170, 255, 0), false, false, g_VeloFont);

			//else
			if (lastjump >= 100 && g_Options.lastjump)
			{
				if (g_Options.lastjumpoutline)
					Render::Get().RenderText(drawvel2, screenWidth / 2 + 60, screenHeight - 100, 27.f, Color(0, 0, 0), false, true, g_VeloFont);

				Render::Get().RenderText(drawvel2, screenWidth / 2 + 60, screenHeight - 100, 27.f, g_Options.Velocitycol, false, false, g_VeloFont);

			}

		}

	}
	static std::vector<float> velData(120, 0);
	float currentVelocity = sqrt(speed.x * speed.x + speed.y * speed.y);

	velData.erase(velData.begin());
	velData.push_back(currentVelocity);

	for (auto i = 0; i < velData.size() - 1; i++)
	{
		int cur = velData.at(i);
		int next = velData.at(i + 1);

		if (g_Options.velgraphline) {
			Render::Get().RenderLine<float>(screenWidth / 2 + (velData.size() * 5 / 2) - (i - 1) * 5.f, screenHeight / 1.15 - (std::clamp(cur, 0, 450) * .3f), screenWidth / 2 + (velData.size() * 5 / 2) - i * 5.f, screenHeight / 1.15 - (std::clamp(next, 0, 450) * .3f), Color(255, 255, 255, 255));
		}
	}
}

//--------------------------------------------------------------------------------

void Visuals::AddToDrawList() {
	auto player = Player();
	player.RenderSnapline();

	for (auto i = 1; i <= g_EntityList->GetHighestEntityIndex(); ++i) {
		auto entity = C_BaseEntity::GetEntityByIndex(i);

		if (!entity)
			continue;

		if (entity == g_LocalPlayer && !m_input()->m_fCameraInThirdPerson)
			continue;

		if (i <= g_EngineClient->GetMaxClients()) {
			if (player.Begin((C_BasePlayer*)entity)) {
				if (g_Options.snaplines)			player.Drugs();
				if (g_Options.showbt)			    player.BruhTubb();
				if (g_Options.skeletonesp)			player.Skemlanton();
				if (g_Options.esp_player_boxes)     player.RenderBox(entity);
				if (g_Options.esp_player_weapons)   player.RenderWeaponName(entity);
				if (g_Options.esp_player_names)     player.RenderName(entity);
				if (g_Options.esp_player_health)    player.RenderHealth(entity);
				if (g_Options.esp_player_armour)    player.RenderArmour();
				if (g_Options.playerind)			player.sexdick((C_BasePlayer*)entity);
			}
		}
		else if (g_Options.esp_dropped_weapons && entity->IsWeapon())
			RenderWeapon(static_cast<C_BaseCombatWeapon*>(entity));
		else if (g_Options.esp_dropped_weapons && entity->IsDefuseKit())
			RenderDefuseKit(entity);
		else if (entity->IsPlantedC4() && g_Options.esp_planted_c4)
			RenderPlantedC4(entity);
		else if (entity->IsLoot() && g_Options.esp_items)
			RenderItemEsp(entity);
	}
	speed();
}

bool edgebugged = false;
int edgebugs = 0;


