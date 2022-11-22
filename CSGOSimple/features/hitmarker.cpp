#include "./hitmarker.hpp"
#include "../features/visuals.hpp"
#include "../options.hpp"
#pragma comment(lib, "winmm.lib")
#include "../Globals.h"
#include "Misc.hpp"
#include "../hooks.hpp"

void HitMarkerEvent::FireGameEvent(IGameEvent* event)
{
	if (!event)
		return;

	Globals::eventgl = event;

	const char* szEventName = event->GetName();

	if (!strcmp(szEventName, "round_freeze_end"))
		Globals::freezetime = false;

	if (!strcmp(szEventName, "round_prestart"))
		Globals::freezetime = true;

	if (!strcmp(szEventName, "round_prestart"))
		Globals::freezetime = true;

	if (!strcmp(szEventName, "server_spawn"))
		Globals::serverspawn = true;
	else
		Globals::serverspawn = false;

	if (!strcmp(szEventName, "round_start"))
		Globals::clear = true;

	if (!strcmp(szEventName, "server_spawn"))
	{

		const auto net_channel = g_EngineClient->GetNetChannelInfo();

		if (net_channel != nullptr)
		{
			const auto index = 40;
			Hooks::hk_netchannel.setup(net_channel);
			Hooks::hk_netchannel.hook_index(index, Hooks::send_net_msg);
		}
	}

	if (!strcmp(szEventName, "cs_game_disconnected"))
	{
		if (Hooks::hk_netchannel.is_hooked())
		{
			Hooks::hk_netchannel.unhook_all();
		}
	}

	if (!strcmp(szEventName, "player_death"))
	{
		if (g_LocalPlayer && g_LocalPlayer->IsAlive() && g_EngineClient->GetPlayerForUserID(event->GetInt("attacker")) == g_EngineClient->GetLocalPlayer() &&
			g_EngineClient->GetPlayerForUserID(event->GetInt("userid")) != g_EngineClient->GetLocalPlayer())
		{
			Globals::killedplayer = true;

			//Luke said pop off like da ruger in da clipppppp
			//g_EngineClient->ExecuteClientCmd("say ez");
		}

	}

	//if (event)
	//Misc::Get().Cum(event);

	if (!strcmp(szEventName, "player_hurt")) {
		if (g_EngineClient->GetPlayerForUserID(event->GetInt("attacker")) == g_EngineClient->GetLocalPlayer() &&
			g_EngineClient->GetPlayerForUserID(event->GetInt("userid")) != g_EngineClient->GetLocalPlayer())
		{
			Globals::playerid = g_EngineClient->GetPlayerForUserID(event->GetInt("userid"));
			Globals::hitplayer = true;
		}
		if (g_Options.misc_hitmarker)
		{
			if (g_EngineClient->GetPlayerForUserID(event->GetInt("attacker")) == g_EngineClient->GetLocalPlayer() &&
				g_EngineClient->GetPlayerForUserID(event->GetInt("userid")) != g_EngineClient->GetLocalPlayer())
			{
				if (g_Options.hitmarkersound == 0) {
					hitMarkerInfo.push_back({ g_GlobalVars->curtime + 0.8f, event->GetInt("dmg_health") });
					g_EngineClient->ExecuteClientCmd("playvol buttons/arena_switch_press_02 1.0");
				}
				if (g_Options.hitmarkersound == 1) {
					hitMarkerInfo.push_back({ g_GlobalVars->curtime + 0.8f, event->GetInt("dmg_health") });
					g_EngineClient->ExecuteClientCmd("playvol player/vo/balkan/thanks03 1.0");
				}
				if (g_Options.hitmarkersound == 2) {
					hitMarkerInfo.push_back({ g_GlobalVars->curtime + 0.8f, event->GetInt("dmg_health") });
					g_EngineClient->ExecuteClientCmd("playvol player/vo/balkan/disagree01 1.0");
				}
				if (g_Options.hitmarkersound == 3) {
					hitMarkerInfo.push_back({ g_GlobalVars->curtime + 0.8f, event->GetInt("dmg_health") });
					g_EngineClient->ExecuteClientCmd("playvol ambient/energy/spark5 1.0");
				}
				if (g_Options.hitmarkersound == 4) {
					hitMarkerInfo.push_back({ g_GlobalVars->curtime + 0.8f, event->GetInt("dmg_health") });
					g_EngineClient->ExecuteClientCmd("playvol buttons/blip1 1.0");
				}
				if (g_Options.hitmarkersound == 5) {
					hitMarkerInfo.push_back({ g_GlobalVars->curtime + 0.8f, event->GetInt("dmg_health") });
					g_EngineClient->ExecuteClientCmd("playvol buttons/button10 1.0");
				}
				if (g_Options.hitmarkersound == 6) {
					hitMarkerInfo.push_back({ g_GlobalVars->curtime + 0.8f, event->GetInt("dmg_health") });
					g_EngineClient->ExecuteClientCmd("playvol common/beep 1.0");
				}
				if (g_Options.hitmarkersound == 7) {
					hitMarkerInfo.push_back({ g_GlobalVars->curtime + 0.8f, event->GetInt("dmg_health") });
					g_EngineClient->ExecuteClientCmd("playvol weapons/hegrenade/beep 1.0");
				}
			}
			else
				Globals::hitplayer = false;
		}
	}
}

int HitMarkerEvent::GetEventDebugID(void)
{
	return EVENT_DEBUG_ID_INIT;
}

void HitMarkerEvent::RegisterSelf()
{
	g_GameEvents->AddListener(this, "player_hurt", false);
	g_GameEvents->AddListener(this, "player_death", false);
	g_GameEvents->AddListener(this, "round_freeze_end", false);
	g_GameEvents->AddListener(this, "round_prestart", false);
	g_GameEvents->AddListener(this, "server_spawn", false);	
	g_GameEvents->AddListener(this, "round_start", false);
}

void HitMarkerEvent::UnregisterSelf()
{
	g_GameEvents->RemoveListener(this);
}