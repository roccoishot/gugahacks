#include "./features/Misc.hpp"
#include <algorithm>
#include "BetaAA.h"
#include "crypt_str.h"
#include <iostream>
#include <cstdio>
#include <ctime>

void Misc::Sexdick(CUserCmd* cmd, bool& bSendPacket) {
    if (!g_Options.sexdick.enabled)
        return;

    if (g_Options.sexdick.fuckthepitch) {
            if (g_LocalPlayer->m_fFlags() & FL_ONGROUND) //&& g_LocalPlayer->GetPlayerAnimState()->m_bInHitGroundAnimation && g_LocalPlayer->GetPlayerAnimState()->m_flHeadHeightOrOffsetFromHittingGroundAnimation)
                g_Options.ragebot_antiaim_pitch = 4;
            else
                g_Options.ragebot_antiaim_pitch = 2;
    }

    if (g_Options.sexdick.nopacketonshot) {
        if (cmd->buttons & IN_ATTACK || g_LocalPlayer->m_iShotsFired() >= 1) {
            bSendPacket = false;
        }
    }

    if (g_Options.sexdick.urrstuck) {
        if (GetAsyncKeyState(g_Options.sexdick.urrstuckkey)) {
            cmd->tick_count = 0x7F7FFFFF;
            cmd->command_number = 0x00000;
        }
    }
}

void Misc::FakeLag(CUserCmd* cmd, bool& bSendPacket)
{
    if (!g_EngineClient->IsInGame() && g_LocalPlayer) 
        return;

    int choked_commands = g_EngineClient->GetNetChannel()->chokedcommands + 1;
    static bool WasLastInFakelag = false;

    bool Moving = g_LocalPlayer->m_vecVelocity().Length2D() > 0.1f && (cmd->sidemove != 0.f && cmd->forwardmove != 0.f);
    bool InAir = !(g_LocalPlayer->m_fFlags() & FL_ONGROUND);
    bool Standing = !Moving && !InAir;
    int ticks = 16;
    int mode = 1;

    if (ticks == 0)
        return;

    switch (mode)
    {
    case 0:
        if (choked_commands <= ticks)
        {
            WasLastInFakelag = true;
            bSendPacket = false;
        }
        else
        {
            WasLastInFakelag = false;
        }
        break;
    case 1:
        int PacketsToChoke = 0;
        if (g_LocalPlayer->m_vecVelocity().Length() > 0.f)
        {
            PacketsToChoke = (int)(64.f / g_GlobalVars->interval_per_tick / g_LocalPlayer->m_vecVelocity().Length()) + 1;
            if (PacketsToChoke >= 16)
            {
                PacketsToChoke = 15;
            }

            if (PacketsToChoke >= ticks)
            {
                PacketsToChoke = ticks;
            }
        }

        if (choked_commands <= PacketsToChoke)
        {
            WasLastInFakelag = true;
            bSendPacket = false;
        }
        else
        {
            WasLastInFakelag = false;
        }
        g_EngineClient->GetNetChannel()->m_nChokedPackets = PacketsToChoke;
        break;

    }
}

void Misc::ClanTag()
{
    static auto removed = false;

    if (!g_Options.clantag && !removed)
    {
        removed = true;
        Utils::SetClantag(crypt_str(""));
        return;
    }

    if (g_Options.clantag)
    {
        auto nci = g_EngineClient->GetNetChannelInfo();

        if (!nci)
            return;

        static auto time = -1;

        auto ticks = TIME_TO_TICKS(nci->GetAvgLatency(FLOW_OUTGOING)) + (float)g_GlobalVars->tickcount; //-V807
        auto intervals = 0.5f / g_GlobalVars->interval_per_tick;

        auto main_time = (int)(ticks / intervals) % 22;

        if (main_time != time && !g_EngineClient->GetNetChannel()->chokedcommands)
        {
            auto tag = crypt_str("");

            "Animated",
                "Discord",
                "Static",
                "Reverse";
                if (g_Options.clantagtype == 0) {
                    switch (main_time)
                    {
                    case 0:
                        tag = crypt_str("GUGAHACKS.SU "); //-V1037
                        break;
                    case 1:
                        tag = crypt_str("UGAHACKS.SU ");
                        break;
                    case 2:
                        tag = crypt_str("GAHACKS.SU ");
                        break;
                    case 3:
                        tag = crypt_str("AHACKS.SU ");
                        break;
                    case 4:
                        tag = crypt_str("HACKS.SU ");
                        break;
                    case 5:
                        tag = crypt_str("ACKS.SU ");
                        break;
                    case 6:
                        tag = crypt_str("CKS.SU ");
                        break;
                    case 7:
                        tag = crypt_str("KS.SU ");
                        break;
                    case 8:
                        tag = crypt_str("S.SU ");
                        break;
                    case 9:
                        tag = crypt_str(".SU ");
                        break;
                    case 10:
                        tag = crypt_str("SU");
                        break;
                    case 11:
                        tag = crypt_str("U");
                        break;
                    case 13:
                        tag = crypt_str("SU");
                        break;
                    case 14:
                        tag = crypt_str(".SU");
                        break;
                    case 15:
                        tag = crypt_str("S.SU");
                        break;
                    case 16:
                        tag = crypt_str("KS.SU");
                        break;
                    case 17:
                        tag = crypt_str("CKS.SU");
                        break;
                    case 18:
                        tag = crypt_str("ACKS.SU");
                        break;
                    case 19:
                        tag = crypt_str("HACKS.SU");
                        break;
                    case 20:
                        tag = crypt_str("AHACKS.SU");
                        break;
                    case 21:
                        tag = crypt_str("GAHACKS.SU");
                        break;
                    case 22:
                        tag = crypt_str("UGAHACKS.SU");
                        break;
                    case 23:
                        tag = crypt_str("GUGAHACKS.SU ");
                        break;
                    }

                    Utils::SetClantag(tag);
                    time = main_time;
                }

                if (g_Options.clantagtype == 1) {
                    Utils::SetClantag("dc.gg/URvWEkvYc5");
                    time = main_time;
                }

                if (g_Options.clantagtype == 2) {
                    Utils::SetClantag("GUGAHACKS");
                    time = main_time;
                }

                if (g_Options.clantagtype == 3) {
                    switch (main_time)
                    {
                    case 0:
                        tag = crypt_str("US.SKCAHAGUG "); //-V1037
                        break;
                    case 1:
                        tag = crypt_str("S.SKCAHAGUG ");
                        break;
                    case 2:
                        tag = crypt_str(".SKCAHAGUG ");
                        break;
                    case 3:
                        tag = crypt_str("SKCAHAGUG ");
                        break;
                    case 4:
                        tag = crypt_str("KCAHAGUG ");
                        break;
                    case 5:
                        tag = crypt_str("CAHAGUG ");
                        break;
                    case 6:
                        tag = crypt_str("AHAGUG ");
                        break;
                    case 7:
                        tag = crypt_str("HAGUG ");
                        break;
                    case 8:
                        tag = crypt_str("AGUG ");
                        break;
                    case 9:
                        tag = crypt_str("GUG ");
                        break;
                    case 10:
                        tag = crypt_str("UG");
                        break;
                    case 11:
                        tag = crypt_str("G");
                        break;
                    case 13:
                        tag = crypt_str("UG");
                        break;
                    case 14:
                        tag = crypt_str("GUG");
                        break;
                    case 15:
                        tag = crypt_str("AGUG");
                        break;
                    case 16:
                        tag = crypt_str("HAGUG");
                        break;
                    case 17:
                        tag = crypt_str("AHAGUG");
                        break;
                    case 18:
                        tag = crypt_str("CAHAGUG");
                        break;
                    case 19:
                        tag = crypt_str("KCAHAGUG");
                        break;
                    case 20:
                        tag = crypt_str("SKCAHAGUG");
                        break;
                    case 21:
                        tag = crypt_str(".SKCAHAGUG");
                        break;
                    case 22:
                        tag = crypt_str("S.SKCAHAGUG");
                        break;
                    case 23:
                        tag = crypt_str("US.SKCAHAGUG ");
                        break;
                    }

                    Utils::SetClantag(tag);
                    time = main_time;
                }


            removed = false;
        }
    }
}

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

void Misc::AutoStop(CUserCmd* cmd) {
    /*Vector velocity = g_LocalPlayer->m_vecVelocity();
    QAngle direction;

    Math::VectorAngles(velocity, direction);

    float speed = velocity.Length2D();

    direction.yaw = cmd->viewangles.yaw - direction.yaw;

    Vector forward;

    Math::AngleVectors(direction, forward);

    Vector source = forward * -speed;

    if (!(cmd->buttons & IN_MOVERIGHT)) {
        cmd->sidemove = source.y;
    }
    if (!(cmd->buttons & IN_MOVELEFT)) {
        cmd->sidemove = source.y;
    }
    if (!(cmd->buttons & IN_FORWARD)) {
        cmd->forwardmove = source.x;
    }
    if (!(cmd->buttons & IN_BACK)) {
        cmd->forwardmove = source.x;
    }*/
}

void Misc::SilentWalk(CUserCmd* cmd)
{
        Vector moveDir = Vector(0.f, 0.f, 0.f);
        float maxSpeed = 130.f;
        int movetype = g_LocalPlayer->m_nMoveType();
        bool InAir = !(g_LocalPlayer->m_fFlags() & FL_ONGROUND);
        if (movetype == MOVETYPE_FLY || movetype == MOVETYPE_NOCLIP || InAir || cmd->buttons & IN_DUCK || !(cmd->buttons & IN_SPEED))
            return;
        moveDir.x = cmd->sidemove;
        moveDir.y = cmd->forwardmove;
        moveDir = Math::ClampMagnitude(moveDir, maxSpeed);
        cmd->sidemove = moveDir.x;
        cmd->forwardmove = moveDir.y;
        if (!(g_LocalPlayer->m_vecVelocity().Length2D() > maxSpeed + 1))
            cmd->buttons &= ~IN_SPEED;
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

void Misc::MovementFixxa(CUserCmd* m_Cmd, QAngle wish_angle, QAngle old_angles) {
    if (old_angles.pitch != wish_angle.pitch || old_angles.yaw != wish_angle.yaw || old_angles.roll != wish_angle.roll) {
        Vector wish_forward, wish_right, wish_up, cmd_forward, cmd_right, cmd_up;

        auto viewangles = old_angles;
        auto movedata = Vector(m_Cmd->forwardmove, m_Cmd->sidemove, m_Cmd->upmove);
        viewangles.Normalize();

        if (!(g_LocalPlayer->m_fFlags() & FL_ONGROUND) && viewangles.roll != 0.f)
            movedata.y = 0.f;

        Math::AngleVectors(wish_angle, wish_forward, wish_right, wish_up);
        Math::AngleVectors(viewangles, cmd_forward, cmd_right, cmd_up);

        auto v8 = sqrt(wish_forward.x * wish_forward.x + wish_forward.y * wish_forward.y), v10 = sqrt(wish_right.x * wish_right.x + wish_right.y * wish_right.y), v12 = sqrt(wish_up.z * wish_up.z);

        Vector wish_forward_norm(1.0f / v8 * wish_forward.x, 1.0f / v8 * wish_forward.y, 0.f),
            wish_right_norm(1.0f / v10 * wish_right.x, 1.0f / v10 * wish_right.y, 0.f),
            wish_up_norm(0.f, 0.f, 1.0f / v12 * wish_up.z);

        auto v14 = sqrt(cmd_forward.x * cmd_forward.x + cmd_forward.y * cmd_forward.y), v16 = sqrt(cmd_right.x * cmd_right.x + cmd_right.y * cmd_right.y), v18 = sqrt(cmd_up.z * cmd_up.z);

        Vector cmd_forward_norm(1.0f / v14 * cmd_forward.x, 1.0f / v14 * cmd_forward.y, 1.0f / v14 * 0.0f),
            cmd_right_norm(1.0f / v16 * cmd_right.x, 1.0f / v16 * cmd_right.y, 1.0f / v16 * 0.0f),
            cmd_up_norm(0.f, 0.f, 1.0f / v18 * cmd_up.z);

        auto v22 = wish_forward_norm.x * movedata.x, v26 = wish_forward_norm.y * movedata.x, v28 = wish_forward_norm.z * movedata.x, v24 = wish_right_norm.x * movedata.y, v23 = wish_right_norm.y * movedata.y, v25 = wish_right_norm.z * movedata.y, v30 = wish_up_norm.x * movedata.z, v27 = wish_up_norm.z * movedata.z, v29 = wish_up_norm.y * movedata.z;

        Vector correct_movement;
        correct_movement.x = cmd_forward_norm.x * v24 + cmd_forward_norm.y * v23 + cmd_forward_norm.z * v25 + (cmd_forward_norm.x * v22 + cmd_forward_norm.y * v26 + cmd_forward_norm.z * v28) + (cmd_forward_norm.y * v30 + cmd_forward_norm.x * v29 + cmd_forward_norm.z * v27);
        correct_movement.y = cmd_right_norm.x * v24 + cmd_right_norm.y * v23 + cmd_right_norm.z * v25 + (cmd_right_norm.x * v22 + cmd_right_norm.y * v26 + cmd_right_norm.z * v28) + (cmd_right_norm.x * v29 + cmd_right_norm.y * v30 + cmd_right_norm.z * v27);
        correct_movement.z = cmd_up_norm.x * v23 + cmd_up_norm.y * v24 + cmd_up_norm.z * v25 + (cmd_up_norm.x * v26 + cmd_up_norm.y * v22 + cmd_up_norm.z * v28) + (cmd_up_norm.x * v30 + cmd_up_norm.y * v29 + cmd_up_norm.z * v27);

        correct_movement.x = std::clamp(correct_movement.x, -450.f, 450.f);
        correct_movement.y = std::clamp(correct_movement.y, -450.f, 450.f);
        correct_movement.z = std::clamp(correct_movement.z, -320.f, 320.f);

        m_Cmd->forwardmove = correct_movement.x;
        m_Cmd->sidemove = correct_movement.y;
        m_Cmd->upmove = correct_movement.z;

        m_Cmd->buttons &= ~(IN_MOVERIGHT | IN_MOVELEFT | IN_BACK | IN_FORWARD);
        if (m_Cmd->sidemove != 0.0) {
            if (m_Cmd->sidemove <= 0.0)
                m_Cmd->buttons |= IN_MOVELEFT;
            else
                m_Cmd->buttons |= IN_MOVERIGHT;
        }

        if (m_Cmd->forwardmove != 0.0) {
            if (m_Cmd->forwardmove <= 0.0)
                m_Cmd->buttons |= IN_BACK;
            else
                m_Cmd->buttons |= IN_FORWARD;
        }
    }
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

void Misc::SetThirdpersonAngles(ClientFrameStage_t stage, CUserCmd* cmd)
{
            if (stage != FRAME_RENDER_START)
                return;

            if (g_EngineClient->IsInGame() && g_LocalPlayer)
            {
                if (g_LocalPlayer->IsAlive() && g_Input->m_fCameraInThirdPerson)
                    g_LocalPlayer->SetVAngles(cmd->viewangles);
            }
    }

void Misc::ChatSpama(CUserCmd* cmd) {
    {

        if (!g_Options.misc_chatspam)
            return;

        static std::string chatspam[] =
        {
            crypt_str("GUGAHACKS.SU | Probably Doin' You're Mother Currently."),
            crypt_str("GUGAHACKS.SU | eat my penris"),
            crypt_str("GUGAHACKS.SU | Me > You"),
            crypt_str("GUGAHACKS.SU | Your IQ must be lower than your GPA"),
            crypt_str("GUGAHACKS.SU | femboy fucked!!."),
            crypt_str("GUGAHACKS.SU | Farting IS NOT Cumming"),
            crypt_str("GUGAHACKS.SU | When I shit... I SHIT"),
            crypt_str("GUGAHACKS.SU | Gugahacks > ALL"),
            crypt_str("GUGAHACKS.SU | discord.gg/qukzNNda75"),
            crypt_str("GUGAHACKS.SU | i.imgur.com/mucKbbq.gif"),
        };

        static auto lastspammed = 0;

        if (GetTickCount() - lastspammed > 800)
        {
            lastspammed = GetTickCount();

            srand(GetTickCount());
            std::string msg = crypt_str("say ") + chatspam[rand() % 10];

            g_EngineClient->ExecuteClientCmd(msg.c_str());
        }
    }
}