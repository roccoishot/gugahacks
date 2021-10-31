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