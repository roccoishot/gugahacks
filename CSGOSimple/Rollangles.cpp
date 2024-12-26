#include "Rollangles.h"
#include "BetaAA.h"
#include "Globals.h"

void RollAngles::Roll(CUserCmd* cmd, bool& bSendPacket)
{
	bool balls = false;

	if (GetKeyState(g_Options.invertaa))
		balls = true;
	else if (!(GetKeyState(g_Options.invertaa)))
		balls = false;

	if (!g_EngineClient->IsInGame() && !g_LocalPlayer->IsAlive())
		balls = false;

	float realroll = g_Options.realroll;
	float roll = g_Options.roll;

	if (Globals::valve) {
		if (g_Options.roll > 40)
			roll = 39;
		if (g_Options.realroll > 40)
			realroll = 39;
		if (g_Options.roll < -40)
			roll = -39;
		if (g_Options.realroll < -40)
			realroll = -39;
	}
	else
	{
		realroll = g_Options.realroll;
		roll = g_Options.roll;
	}

	if (g_Options.spinroll2)
	{
		roll = (fmodf(cmd->tick_count * 3.f, 160.f) * g_Options.spinroll2speed);
	}
	else
	{
		roll = g_Options.roll;
	}

	if (g_Options.spinroll)
	{
		realroll = (fmodf(cmd->tick_count * 3.f, 160.f) * g_Options.spinrollspeed);
	}
	else
	{
		realroll = g_Options.realroll;
	}

	if (!Globals::freezetime && !g_LocalPlayer->m_bGunGameImmunity() && g_LocalPlayer->m_fFlags() != FL_FROZEN && g_LocalPlayer->m_fFlags() & FL_ONGROUND && g_LocalPlayer->m_vecVelocity().Length2D() < 0.1f)
	{
		if (CAntiAim::Get().CanDesync(cmd))
		{
			if (!bSendPacket)
			{
				cmd->viewangles.roll = balls ? -roll : roll;
				Globals::roll = cmd->viewangles.roll;
			}

			if (bSendPacket)
			{
				cmd->viewangles.roll = balls ? -realroll : realroll;
				Globals::realroll = cmd->viewangles.roll;
			}
		}
		else
		{
			cmd->viewangles.roll = balls ? -realroll : realroll;
			Globals::realroll = cmd->viewangles.roll;
		}

	}
}