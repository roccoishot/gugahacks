#include "./features/Misc.hpp"
#include <algorithm>
#include "BetaAA.h"

bool break_lby = false;
float next_update = 0;
void Misc::UpdateLBY(CUserCmd* cmd, bool& bSendPacket) {
    float server_time = g_GlobalVars->interval_per_tick * g_LocalPlayer->m_nTickBase();
    float speed = g_LocalPlayer->m_vecVelocity().LengthSqr();

   if (speed > 0.1)
        next_update = server_time + 0.22;

    break_lby = g_Options.breaklby;

    if (next_update <= server_time) {
        next_update = server_time + 1.1;
        break_lby = g_Options.breaklby;
    }

    if (!(g_LocalPlayer->m_fFlags() & FL_ONGROUND))
        break_lby = g_Options.breaklby;

    if (break_lby)
    {
        bSendPacket = false;
        cmd->viewangles.yaw += g_LocalPlayer->m_flLowerBodyYawTarget();//120.f;
    }
}
//--------------------------------------------------------------------------------
void Misc::Fakelag(CUserCmd* cmd, bool& bSendPacket) {
    if (g_EngineClient->IsVoiceRecording())
        return;
    if (!g_LocalPlayer)
        return;

    int chockepack = 0;
    auto NetChannel = g_EngineClient->GetNetChannel();
    if (!NetChannel)
        return;
    bSendPacket = true;

    chockepack = g_Options.faketicks;
    bSendPacket = (NetChannel->m_nChokedPackets >= chockepack);
}
//--------------------------------------------------------------------------------
void Misc::Desync(CUserCmd* cmd, bool& bSendPacket) {
    auto weapon = g_LocalPlayer->m_hActiveWeapon().Get();
    if (!weapon) return;
    if (!weapon->GetCSWeaponData()) return;
    if (weapon->IsGrenade()) return;
        if (cmd->buttons & IN_USE || cmd->buttons & IN_ATTACK || cmd->buttons & IN_JUMP)
            return;
    static QAngle LastRealAngle = QAngle(0, 0, 0);
    if (!bSendPacket) {
        if (g_Options.misc_desync)
            cmd->viewangles.yaw += 120.f;
        else
            cmd->viewangles.yaw -= 120.f;
        }
        else
           LastRealAngle = cmd->viewangles;
}

void Misc::MovementFix(QAngle vOldAngles, CUserCmd* pCmd, float fOldForward, float fOldSidemove) {
    float deltaView = pCmd->viewangles.yaw - vOldAngles.yaw;
    float f1;
    float f2;

    if (vOldAngles.yaw < 0.f)
    {
        f1 = 360.0f + vOldAngles.yaw;
    }
    else
    {
        f1 = vOldAngles.yaw;
    }

    if (pCmd->viewangles.yaw < 0.0f)
    {
        f2 = 360.0f + pCmd->viewangles.yaw;
    }
    else
    {
        f2 = pCmd->viewangles.yaw;
    }

    if (f2 < f1)
    {
        deltaView = abs(f2 - f1);
    }
    else
    {
        deltaView = 360.0f - abs(f1 - f2);
    }
    deltaView = 360.0f - deltaView;

    pCmd->forwardmove = cos(DEG2RAD(deltaView)) * fOldForward + cos(DEG2RAD(deltaView + 90.f)) * fOldSidemove;
    pCmd->sidemove = sin(DEG2RAD(deltaView)) * fOldForward + sin(DEG2RAD(deltaView + 90.f)) * fOldSidemove;

    pCmd->buttons &= ~IN_MOVERIGHT;
    pCmd->buttons &= ~IN_MOVELEFT;
    pCmd->buttons &= ~IN_FORWARD;
    pCmd->buttons &= ~IN_BACK;
}

void Misc::SlowWalk(CUserCmd* cmd) {
    if (g_Options.ragebot_slowwalk_amt <= 0 || !GetAsyncKeyState(g_Options.ragebot_slowwalk_key))
        return;

    auto weapon_handle = g_LocalPlayer->m_hActiveWeapon();

    if (!weapon_handle)
        return;

    float amount = 0.0034f * g_Options.ragebot_slowwalk_amt/*options.misc.slow_walk_amount*/; //max 100

    Vector velocity = g_LocalPlayer->m_vecVelocity();
    QAngle direction;

    Math::VectorAngles(velocity, direction);

    float speed = velocity.Length2D();

    direction.yaw = cmd->viewangles.yaw - direction.yaw;

    Vector forward;

    Math::AngleVectors(direction, forward);

    Vector source = forward * -speed;

    if (speed >= (250 * amount))
    {
        cmd->forwardmove = source.x;
        cmd->sidemove = source.y;

    }
}

void Misc::SetThirdpersonAngles(ClientFrameStage_t stage)
{
        //CArlos was herer
        if (g_Options.ragebot_antiaim_yaw == 1 && g_Options.ragebot_antiaim_pitch == 1) {
            QAngle LastAngle = QAngle(82, -180, 0);

            if (stage != FRAME_RENDER_START)
                return;

            if (g_EngineClient->IsInGame() && g_LocalPlayer)
            {
                if (g_LocalPlayer->IsAlive() && g_Input->m_fCameraInThirdPerson)
                    g_LocalPlayer->SetVAngles(LastAngle);
            }
        }

        if (g_Options.ragebot_antiaim_yaw == 1 && g_Options.ragebot_antiaim_pitch == 2) {
            QAngle LastAngle = QAngle(90, -180, 0);

            if (stage != FRAME_RENDER_START)
                return;

            if (g_EngineClient->IsInGame() && g_LocalPlayer)
            {
                if (g_LocalPlayer->IsAlive() && g_Input->m_fCameraInThirdPerson)
                    g_LocalPlayer->SetVAngles(LastAngle);
            }
        }

        if (g_Options.ragebot_antiaim_yaw == 1 && g_Options.ragebot_antiaim_pitch == 3) {
            QAngle LastAngle = QAngle(-90, -180, 0);

            if (stage != FRAME_RENDER_START)
                return;

            if (g_EngineClient->IsInGame() && g_LocalPlayer)
            {
                if (g_LocalPlayer->IsAlive() && g_Input->m_fCameraInThirdPerson)
                    g_LocalPlayer->SetVAngles(LastAngle);
            }
        }

        if (g_Options.ragebot_antiaim_yaw == 1 && g_Options.ragebot_antiaim_pitch == 4) {
            QAngle LastAngle = QAngle(0, -180, 0);

            if (stage != FRAME_RENDER_START)
                return;

            if (g_EngineClient->IsInGame() && g_LocalPlayer)
            {
                if (g_LocalPlayer->IsAlive() && g_Input->m_fCameraInThirdPerson)
                    g_LocalPlayer->SetVAngles(LastAngle);
            }
        }

        //Spinning (FRFR!!!)
        if (g_Options.ragebot_antiaim_yaw == 2 && g_Options.ragebot_antiaim_pitch == 1) {
            QAngle LastAngle = QAngle(82, fmodf(g_GlobalVars->tickcount * 10.f, 360.f) * g_Options.spinspeed, 0);

            if (stage != FRAME_RENDER_START)
                return;

            if (g_EngineClient->IsInGame() && g_LocalPlayer)
            {
                if (g_LocalPlayer->IsAlive() && g_Input->m_fCameraInThirdPerson)
                    g_LocalPlayer->SetVAngles(LastAngle);
            }
        }

        if (g_Options.ragebot_antiaim_yaw == 2 && g_Options.ragebot_antiaim_pitch == 2) {
            QAngle LastAngle = QAngle(90, fmodf(g_GlobalVars->tickcount * 10.f, 360.f) * g_Options.spinspeed, 0);

            if (stage != FRAME_RENDER_START)
                return;

            if (g_EngineClient->IsInGame() && g_LocalPlayer)
            {
                if (g_LocalPlayer->IsAlive() && g_Input->m_fCameraInThirdPerson)
                    g_LocalPlayer->SetVAngles(LastAngle);
            }
        }

        if (g_Options.ragebot_antiaim_yaw == 2 && g_Options.ragebot_antiaim_pitch == 3) {
            QAngle LastAngle = QAngle(-90, fmodf(g_GlobalVars->tickcount * 10.f, 360.f) * g_Options.spinspeed, 0);

            if (stage != FRAME_RENDER_START)
                return;

            if (g_EngineClient->IsInGame() && g_LocalPlayer)
            {
                if (g_LocalPlayer->IsAlive() && g_Input->m_fCameraInThirdPerson)
                    g_LocalPlayer->SetVAngles(LastAngle);
            }
        }

        if (g_Options.ragebot_antiaim_yaw == 2 && g_Options.ragebot_antiaim_pitch == 4) {
            QAngle LastAngle = QAngle(0, fmodf(g_GlobalVars->tickcount * 10.f, 360.f) * g_Options.spinspeed, 0);

            if (stage != FRAME_RENDER_START)
                return;

            if (g_EngineClient->IsInGame() && g_LocalPlayer)
            {
                if (g_LocalPlayer->IsAlive() && g_Input->m_fCameraInThirdPerson)
                    g_LocalPlayer->SetVAngles(LastAngle);
            }
        }
    }