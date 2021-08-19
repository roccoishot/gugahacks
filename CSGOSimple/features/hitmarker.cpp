#include "./hitmarker.hpp"
#include "../features/visuals.hpp"
#include "../options.hpp"
#pragma comment(lib, "winmm.lib")

void HitMarkerEvent::FireGameEvent(IGameEvent* event)
{
	if (!event)
		return;

	if (g_Options.misc_hitmarker)
	{
		if (g_EngineClient->GetPlayerForUserID(event->GetInt("attacker")) == g_EngineClient->GetLocalPlayer() &&
			g_EngineClient->GetPlayerForUserID(event->GetInt("userid")) != g_EngineClient->GetLocalPlayer())
		{
			if (g_Options.hitmetallic) {
				hitMarkerInfo.push_back({ g_GlobalVars->curtime + 0.8f, event->GetInt("dmg_health") });
				g_EngineClient->ExecuteClientCmd("playvol buttons/arena_switch_press_02 0.6");
			}
			if (g_Options.hitchicken) {
				hitMarkerInfo.push_back({ g_GlobalVars->curtime + 0.8f, event->GetInt("dmg_health") });
				g_EngineClient->ExecuteClientCmd("playvol ambient/creatures/chicken_death_01 0.6");
			}
			if (g_Options.hitdog) {
				hitMarkerInfo.push_back({ g_GlobalVars->curtime + 0.8f, event->GetInt("dmg_health") });
				g_EngineClient->ExecuteClientCmd("playvol ambient/creatures/dog_bark_close_04 0.6");
			}
			if (g_Options.hitspark) {
				hitMarkerInfo.push_back({ g_GlobalVars->curtime + 0.8f, event->GetInt("dmg_health") });
				g_EngineClient->ExecuteClientCmd("playvol ambient/energy/spark5 0.6");
			}
			if (g_Options.hitfranz) {
				hitMarkerInfo.push_back({ g_GlobalVars->curtime + 0.8f, event->GetInt("dmg_health") });
				g_EngineClient->ExecuteClientCmd("playvol buttons/blip1 0.6");
			}
			if (g_Options.hitfail) {
				hitMarkerInfo.push_back({ g_GlobalVars->curtime + 0.8f, event->GetInt("dmg_health") });
				g_EngineClient->ExecuteClientCmd("playvol buttons/button10 0.6");
			}
			if (g_Options.hitbeep) {
				hitMarkerInfo.push_back({ g_GlobalVars->curtime + 0.8f, event->GetInt("dmg_health") });
				g_EngineClient->ExecuteClientCmd("playvol common/beep 0.6");
			}
			if (g_Options.hithurt) {
				hitMarkerInfo.push_back({ g_GlobalVars->curtime + 0.8f, event->GetInt("dmg_health") });
				g_EngineClient->ExecuteClientCmd("playvol player/damage3 0.6");
			}
			if (g_Options.hitsnap) {
				hitMarkerInfo.push_back({ g_GlobalVars->curtime + 0.8f, event->GetInt("dmg_health") });
				g_EngineClient->ExecuteClientCmd("playvol player/neck_snap_01 0.6");
			}
			if (g_Options.hitdisagree) {
				hitMarkerInfo.push_back({ g_GlobalVars->curtime + 0.8f, event->GetInt("dmg_health") });
				g_EngineClient->ExecuteClientCmd("playvol player/vo/balkan/disagree01 0.6");
			}
			if (g_Options.hitagree) {
				hitMarkerInfo.push_back({ g_GlobalVars->curtime + 0.8f, event->GetInt("dmg_health") });
				g_EngineClient->ExecuteClientCmd("playvol player/vo/balkan/thanks03 0.6");
			}
			if (g_Options.hitfoundgame) {
				hitMarkerInfo.push_back({ g_GlobalVars->curtime + 0.8f, event->GetInt("dmg_health") });
				g_EngineClient->ExecuteClientCmd("playvol weapons/hegrenade/beep 0.6");
			}
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
}

void HitMarkerEvent::UnregisterSelf()
{
	g_GameEvents->RemoveListener(this);
}