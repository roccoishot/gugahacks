#include "./features/Misc.hpp"
#include <algorithm>
#include "BetaAA.h"
#include "xor.h"
#ifdef ENABLE_XOR
#define XorStr _xor_ 
#else
#define XorStr
#endif
#pragma intrinsic(_ReturnAddress)  
#include <iostream>
#include <cstdio>
#include <ctime>
#include "menu.hpp"
#include "Globals.h"
#include "features/aimbot.hpp"
#include "backtrack.h"
#include "BetaAA.h"

#include<iostream>
#include<conio.h>
#include<stdio.h>
#include<string>
#include<cstring>

#include <stdio.h>

using namespace std;
#include <iostream>
#include <string>

void Misc::Sexdick(CUserCmd* cmd, bool& bSendPacket) {
    if (!g_EngineClient->IsInGame())
        return;

    if (!g_LocalPlayer)
        return;

    if (!g_LocalPlayer->IsAlive())
        return;

    C_BaseCombatWeapon* weapon = g_LocalPlayer->m_hActiveWeapon().Get();

    if (g_Options.nopacketonshot) {
        if (!weapon)
        {
            return;
        }
        if (g_LocalPlayer->m_hActiveWeapon()->m_iItemDefinitionIndex() != WEAPON_REVOLVER) {
            if (bSendPacket) {
                if (g_LocalPlayer->m_iShotsFired() >= 1) {
                    bSendPacket = false;
                }
            }
        }
        if (g_LocalPlayer->m_hActiveWeapon()->m_iItemDefinitionIndex() == WEAPON_REVOLVER) {
            if (bSendPacket) {
                if (g_LocalPlayer->m_iShotsFired() >= 2) {
                    bSendPacket = false;
                }
            }
        }
    }

    if (g_EngineClient->IsInGame() && g_LocalPlayer && g_LocalPlayer->IsAlive() && !Globals::valve) {
        if (g_Options.urrstuck) {
            if (GetAsyncKeyState(g_Options.urrstuckkey)) {
                cmd->tick_count = 0x7F7FFFFF;
                cmd->command_number = 0x00000;
            }
        }
    }

}

bool fding = false;

void Misc::FakeDuck(CUserCmd* cmd, bool& bSendPackets)
{
    static int fakeduckChokeAmount = 0;
    if (g_Options.fakeduck && GetAsyncKeyState(g_Options.fdkey) && !Globals::valve && !Globals::stepping)
    {
        if (fakeduckChokeAmount > 14)
        {
            fakeduckChokeAmount = 0;
            bSendPackets = true;
        }
        else {
            bSendPackets = false;
        }
        fakeduckChokeAmount++;
        auto choke = g_EngineClient->GetNetChannel()->m_nChokedPackets;
        if (g_LocalPlayer->m_fFlags() & FL_ONGROUND)
        {
            fding = true;
            if (choke >= 7) {
                cmd->buttons |= IN_DUCK;
            }
            else {
                cmd->buttons &= ~IN_DUCK;
            }
        }
    }
    else {
        fding = false;
        fakeduckChokeAmount = 0;
    }
}

void Misc::Fakelag(CUserCmd* cmd, bool& bSendPacket) {

    if (!g_Options.fakelag)
        return;

    bool johnseena;

    bool bedting = false;
    bool fart = false;

    if (GetKeyState(g_Options.aimbot.dthotkey) && g_Options.aimbot.dt && g_EngineClient->IsInGame() && g_LocalPlayer->IsAlive())
        bedting = true;
    else
        bedting = false;

    if (GetKeyState(g_Options.aimbot.hshotkey) && g_Options.aimbot.hs && g_EngineClient->IsInGame() && g_LocalPlayer->IsAlive())
        fart = true;
    else
        fart = false;

    auto i = 1; i <= g_EngineClient->GetMaxClients(); i++;

    if (!C_BasePlayer::GetPlayerByIndex(i))
        return;

    for (int c = 0; c <= 18; c++)
    johnseena = C_BasePlayer::GetPlayerByIndex(i)->CanSeePlayer(g_LocalPlayer, c);

    if (g_Options.flonpeek == false || !(C_BasePlayer::GetPlayerByIndex(i)->valid(true, true)))
        johnseena = false;

    Globals::dting = bedting;

    if (!g_EngineClient->IsInGame())
        return;

    if (!g_LocalPlayer)
        return;

    if (!g_LocalPlayer->IsAlive())
        return;

    auto nc = g_EngineClient->GetNetChannel();

    if (!nc)
        return;

    int movetype = g_LocalPlayer->m_nMoveType();

    const int choked_ticks = nc->m_nChokedPackets;
    int tochoke;

    if (fding)
        tochoke = 14;
    else
    {
        if (g_EngineClient->IsVoiceRecording() || Globals::freezetime || g_LocalPlayer->m_bGunGameImmunity() || g_LocalPlayer->m_fFlags() & FL_FROZEN || movetype == MOVETYPE_LADDER || movetype == MOVETYPE_FLY || movetype == MOVETYPE_NOCLIP || bedting || fart)
        {
            tochoke = 1;
            Globals::flcur = false;
        }
        else
        {
            if (g_Options.flonpeek)
            tochoke = johnseena ? g_Options.flop - 1 : g_Options.faketicks - 1;
            else
            tochoke = g_Options.faketicks - 1;
            Globals::flcur = true;
        }
    }

    if ((g_Options.faketicks - 1) > 5 && Globals::valve)
    {

        if (g_EngineClient->IsVoiceRecording() || Globals::freezetime || g_LocalPlayer->m_bGunGameImmunity() || g_LocalPlayer->m_fFlags() & FL_FROZEN || bedting || fart)
        {
            tochoke = 1;
            Globals::flcur = false;
        }
        else
        {
            tochoke = 5;

            if ((g_Options.flop - 1) > 5 && johnseena)
                tochoke = 5;

            Globals::flcur = true;
        }
    }
    else
    {
        if (g_EngineClient->IsVoiceRecording() || Globals::freezetime || g_LocalPlayer->m_bGunGameImmunity() || g_LocalPlayer->m_fFlags() & FL_FROZEN || bedting || fart)
        {
            tochoke = 1;
            Globals::flcur = false;
        }
        else
        {
            tochoke = johnseena ? g_Options.flop - 1 : g_Options.faketicks - 1;
            Globals::flcur = true;
        }
    }

    if (choked_ticks > tochoke)
        bSendPacket = true;
    else
    {
        bSendPacket = false;
        nc->m_nChokedPackets = 0;
        nc->m_nOutSequenceNr;
        nc->m_nChokedPackets = choked_ticks;
    }

}

void Misc::ClanTag()
{
    static auto removed = false;

    if (!g_Options.clantag && !removed)
    {
        removed = true;
        Utils::SetClantag(XorStr(""));
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

        if (main_time != time)
        {
            auto tag = XorStr("");

            if (g_Options.clantagtype == 0) {
                switch (main_time)
                {
                case 0:
                    tag = XorStr("GUGAHACKS"); //-V1037
                    break;
                case 1:
                    tag = XorStr("UGAHACKS");
                    break;
                case 2:
                    tag = XorStr("GAHACKS");
                    break;
                case 3:
                    tag = XorStr("AHACKS");
                    break;
                case 4:
                    tag = XorStr("HACKS");
                    break;
                case 5:
                    tag = XorStr("ACKS");
                    break;
                case 6:
                    tag = XorStr("CKS");
                    break;
                case 7:
                    tag = XorStr("KS");
                    break;
                case 8:
                    tag = XorStr("S");
                    break;
                case 9:
                    tag = XorStr(" ");
                    break;
                case 10:
                    tag = XorStr("S");
                    break;
                case 11:
                    tag = XorStr("KS");
                    break;
                case 13:
                    tag = XorStr("CKS");
                    break;
                case 14:
                    tag = XorStr("ACKS");
                    break;
                case 15:
                    tag = XorStr("HACKS");
                    break;
                case 16:
                    tag = XorStr("AHACKS");
                    break;
                case 17:
                    tag = XorStr("GAHACKS");
                    break;
                case 18:
                    tag = XorStr("UGAHACKS");
                    break;
                }
            }

            if (g_Options.clantagtype == 1) {
                tag = (XorStr("dc.gg/gugahacks"));
            }

            if (g_Options.clantagtype == 2) {
                tag = (XorStr("GUGAHACKS"));
            }

            if (g_Options.clantagtype == 3) {
                switch (main_time)
                {
                case 0:
                    tag = XorStr("SKCAHAGUG"); //-V1037
                    break;
                case 1:
                    tag = XorStr("KCAHAGUG");
                    break;
                case 2:
                    tag = XorStr("CAHAGUG");
                    break;
                case 3:
                    tag = XorStr("AHAGUG");
                    break;
                case 4:
                    tag = XorStr("HAGUG");
                    break;
                case 5:
                    tag = XorStr("AGUG");
                    break;
                case 6:
                    tag = XorStr("GUG");
                    break;
                case 7:
                    tag = XorStr("UG");
                    break;
                case 8:
                    tag = XorStr("G");
                    break;
                case 9:
                    tag = XorStr(" ");
                    break;
                case 10:
                    tag = XorStr("G");
                    break;
                case 11:
                    tag = XorStr("UG");
                    break;
                case 13:
                    tag = XorStr("GUG");
                    break;
                case 14:
                    tag = XorStr("AGUG");
                    break;
                case 15:
                    tag = XorStr("HAGUG");
                    break;
                case 16:
                    tag = XorStr("AHAGUG");
                    break;
                case 17:
                    tag = XorStr("CAHAGUG");
                    break;
                case 18:
                    tag = XorStr("KCAHAGUG");
                    break;
                }
            }

            if (g_Options.clantagtype == 4) {
                tag = (XorStr("\xE2\x80\xAE"));
            }

            if (g_Options.clantagtype == 5) {
                tag = (XorStr("\xE2\x80\xA9\xE2\x80\xA9\xE2\x80\xA9\xE2\x80\xA9"));
            }

            if (g_Options.clantagtype == 6) {
                tag = (XorStr("EMAIL ME!"));
            }

            Utils::SetClantag(tag);
            time = main_time;

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

    break_lby = false;

    if (next_update <= server_time) {
        next_update = server_time + 1.1;
        break_lby = true;
    }

    if (!(g_LocalPlayer->m_fFlags() & FL_ONGROUND))
        break_lby = false;

    if (!g_Options.breaklby)
        return;

    if (!g_LocalPlayer || !g_LocalPlayer->IsAlive())
    {
        return;
    }

    if (Globals::freezetime || g_LocalPlayer->m_bGunGameImmunity() || g_LocalPlayer->m_fFlags() & FL_FROZEN)
        return;

    int movetype = g_LocalPlayer->m_nMoveType();

        if (movetype == MOVETYPE_FLY || movetype == MOVETYPE_NOCLIP || cmd->buttons & IN_ATTACK || cmd->buttons & IN_USE || cmd->buttons & IN_GRENADE1 || cmd->buttons & IN_GRENADE2)
            return;

    C_BaseCombatWeapon* weapon = g_LocalPlayer->m_hActiveWeapon().Get();

    if (!weapon)
        return;

    if (movetype == MOVETYPE_LADDER)
        return;

    if (!weapon->IsGun() && !weapon->IsZeus())
        return;

    if (weapon->IsGrenade())
        return;

        float fakepreset = 0.f;

        bool balls = false;

        if (GetKeyState(g_Options.invertaa))
            balls = true;
        else if (!(GetKeyState(g_Options.invertaa)))
            balls = false;

        if (!g_EngineClient->IsInGame() && !g_LocalPlayer->IsAlive())
            balls = false;

        if (break_lby)
        {
            if (g_Options.breakertp == 0) {
                if (!g_Options.ragebot_antiaim_desync) {
                    bSendPacket = false;
                    cmd->viewangles.yaw += balls ? 120.f : -120.f;
                }
                if (g_Options.ragebot_antiaim_desync) {
                    bSendPacket = false;
                    cmd->viewangles.yaw += balls ? -120.f : 120.f;
                }
            }
            if (g_Options.breakertp == 1) {
                if (!g_Options.ragebot_antiaim_desync) {
                    bSendPacket = false;
                    cmd->viewangles.yaw += balls ? -120.f : 120.f;
                }
                if (g_Options.ragebot_antiaim_desync) {
                    bSendPacket = false;
                    cmd->viewangles.yaw += balls ? 120.f : -120.f;
                }
            }
            if (g_Options.breakertp == 2) {
                bSendPacket = false;
                cmd->viewangles.yaw += g_LocalPlayer->m_flLowerBodyYawTarget();
            }
            if (g_Options.breakertp == 3) {
                cmd->sidemove = cmd->tick_count % 2 ? 1.10 : -1.10;
            }
        }
}

void Misc::SilentWalk(CUserCmd* cmd)
{

    if (!g_LocalPlayer)
        return;

    if (!g_LocalPlayer->IsAlive())
        return;

    if (!g_Options.quickstop)
        return;

    if (!(g_LocalPlayer->m_fFlags() & FL_ONGROUND))
        return;

    auto pressed_move_key = cmd->buttons & IN_FORWARD || cmd->buttons & IN_MOVELEFT || cmd->buttons & IN_BACK || cmd->buttons & IN_MOVERIGHT || cmd->buttons & IN_JUMP;

    if (pressed_move_key)
        return;

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

Vector multipoints[128];
int multipointCount = 0;

bool Misc::cl_move_dt(CUserCmd* m_pcmd)
{

    if (!g_Options.aimbot.enabled)
        return false;

    if (!g_EngineClient->GetNetChannel())
        return false;

    if (!g_EngineClient->IsInGame())
        return false;

    if (!g_LocalPlayer)
        return false;

    if (!g_LocalPlayer->IsAlive())
        return false;

    if (Globals::freezetime || g_LocalPlayer->m_bGunGameImmunity() || g_LocalPlayer->m_fFlags() & FL_FROZEN || fding)
        return false;

    auto weapon = g_LocalPlayer->m_hActiveWeapon().Get();

    static auto lastdoubletaptime = 0;

    if (!g_Options.aimbot.dt)
    {
        Globals::cuhbedointhat = false;
        Globals::ticks_allowed = 0;
        Globals::tochargeamount = g_Options.aimbot.dtticks + 2;
        return false;
    }

    if (g_Options.aimbot.dthotkey <= KEY_NONE || g_Options.aimbot.dthotkey >= KEY_MAX)
    {
        Globals::cuhbedointhat = false;
        Globals::ticks_allowed = 0;
        Globals::tochargeamount = g_Options.aimbot.dtticks + 2;
        return false;
    }

    static bool firing_dt = false;
    static bool was_in_dt = false;

    if (!GetKeyState(g_Options.aimbot.dthotkey) || g_LocalPlayer->m_bGunGameImmunity() || Globals::freezetime)
    {
        Globals::cuhbedointhat = false;

        if (!firing_dt && was_in_dt)
        {
            was_in_dt = false;
        }

        Globals::ticks_allowed = 0;

        return false;
    }

    if (!GetKeyState(g_Options.aimbot.dthotkey)) {
        Globals::shift_ticks = Globals::tocharge;
        return false;
    }

    if (g_LocalPlayer->m_iShotsFired() <= 0 && Globals::tocharge < g_Options.aimbot.dtticks + 2 && g_LocalPlayer->m_nTickBase() - lastdoubletaptime > TIME_TO_TICKS(0.75f)) {
        Globals::startcharge = true;
        Globals::tochargeamount = g_Options.aimbot.dtticks + 2;
    }
    else {
        Globals::startcharge = false;
    }

    if (Globals::tocharge > g_Options.aimbot.dtticks + 2)
        Globals::shift_ticks = Globals::tocharge - g_Options.aimbot.dtticks + 2;

    float dtticks;

    if (Globals::valve)
    {
        if (g_Options.aimbot.dtticks > 6)
            dtticks = 6;
        else if (g_Options.aimbot.dtticks == 6 || g_Options.aimbot.dtticks < 6)
            dtticks = g_Options.aimbot.dtticks;
    }
    if (!Globals::valve)
    {
        dtticks = g_Options.aimbot.dtticks;
    }

    if (!weapon->can_double_tap() && weapon->m_iItemDefinitionIndex() != WEAPON_SSG08)
        return false;

    if (weapon && (g_LocalPlayer->m_iShotsFired() == 1 || (m_pcmd->buttons & IN_ATTACK2 && weapon->IsKnife())) && Globals::tocharge == g_Options.aimbot.dtticks + 2) {
        lastdoubletaptime = g_LocalPlayer->m_nTickBase();
        Globals::shift_ticks = dtticks;

        firing_dt = true;

    }
    return true;
}

bool Misc::hide_shots(CUserCmd* m_pcmd, bool should_work)
{
    if (!g_Options.aimbot.enabled)
        return false;

    if (!g_EngineClient->GetNetChannel())
        return false;

    if (!g_EngineClient->IsInGame())
        return false;

    if (!g_LocalPlayer)
        return false;

    if (!g_LocalPlayer->IsAlive())
        return false;

    if (Globals::freezetime || g_LocalPlayer->m_bGunGameImmunity() || g_LocalPlayer->m_fFlags() & FL_FROZEN || fding)
        return false;

    auto weapon = g_LocalPlayer->m_hActiveWeapon().Get();

    bool hide_shots_enabled = true;
    int hide_shots_key = -1;
    bool double_tap_key = GetKeyState(g_Options.aimbot.dthotkey);

    if (!g_Options.aimbot.hs)
    {
        hide_shots_enabled = false;
        hide_shots_key = false;

        if (should_work)
        {
            Globals::ticks_allowed = 0;
            Globals::shift_ticks = 0;
        }

        return false;
    }

    if (g_Options.aimbot.hshotkey <= KEY_NONE || g_Options.aimbot.hshotkey >= KEY_MAX)
    {
        hide_shots_enabled = false;
        hide_shots_key = false;

        if (should_work)
        {
            Globals::ticks_allowed = 0;
            Globals::shift_ticks = 0;
        }
        return false;
    }

    if (!should_work && double_tap_key)
    {
        hide_shots_enabled = false;
        hide_shots_key = false;
        return false;
    }

    if (!hide_shots_key)
    {
        hide_shots_enabled = false;
        Globals::ticks_allowed = 0;
        Globals::shift_ticks = 0;
        return false;
    }

    if (g_LocalPlayer->m_bGunGameImmunity() || g_LocalPlayer->m_fFlags() & FL_FROZEN)
    {
        hide_shots_enabled = false;
        Globals::ticks_allowed = 0;
        Globals::shift_ticks = 0;
        return false;
    }

    if (fding)
    {
        hide_shots_enabled = false;
        Globals::ticks_allowed = 0;
        Globals::shift_ticks = 0;
        return false;
    }

    float tickbase = -1;

    tickbase = Globals::valve ? 6 : 9;

    auto revolver_shoot = weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER && (m_pcmd->buttons & IN_ATTACK || m_pcmd->buttons & IN_ATTACK2);
    auto weapon_shoot = m_pcmd->buttons & IN_ATTACK && weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER || m_pcmd->buttons & IN_ATTACK2 && weapon->IsKnife() || revolver_shoot;

    if (!weapon->IsGrenade() && weapon_shoot)
        Globals::shift_ticks = tickbase;
}

void Misc::fuck(CUserCmd* cmd)
{
    if (!g_LocalPlayer->IsAlive()) //-V807
        return;

    if (g_LocalPlayer->m_nMoveType() == MOVETYPE_LADDER)
        return;

    if (!(g_LocalPlayer->m_fFlags() & FL_ONGROUND))
        return;

    if (!g_LocalPlayer->m_bGunGameImmunity() && g_LocalPlayer->m_fFlags() != FL_FROZEN && g_Options.slidebruh)
    {
        if (cmd->forwardmove > 0.0f)
        {
            cmd->buttons |= IN_BACK;
            cmd->buttons &= ~IN_FORWARD;
        }
        else if (cmd->forwardmove < 0.0f)
        {
            cmd->buttons |= IN_FORWARD;
            cmd->buttons &= ~IN_BACK;
        }

        if (cmd->sidemove > 0.0f)
        {
            cmd->buttons |= IN_MOVELEFT;
            cmd->buttons &= ~IN_MOVERIGHT;
        }
        else if (cmd->sidemove < 0.0f)
        {
            cmd->buttons |= IN_MOVERIGHT;
            cmd->buttons &= ~IN_MOVELEFT;
        }
    }
    else
    {
        auto buttons = cmd->buttons & ~(IN_MOVERIGHT | IN_MOVELEFT | IN_BACK | IN_FORWARD);

        if (g_Options.slidebruh)
        {
            if (cmd->forwardmove <= 0.0f)
                buttons |= IN_BACK;
            else
                buttons |= IN_FORWARD;

            if (cmd->sidemove > 0.0f)
                goto LABEL_15;
            else if (cmd->sidemove >= 0.0f)
                goto LABEL_18;

            goto LABEL_17;
        }
        else
            goto LABEL_18;

        if (cmd->forwardmove <= 0.0f) //-V779
            buttons |= IN_FORWARD;
        else
            buttons |= IN_BACK;

        if (cmd->sidemove > 0.0f)
        {
        LABEL_17:
            buttons |= IN_MOVELEFT;
            goto LABEL_18;
        }

        if (cmd->sidemove < 0.0f)
            LABEL_15:

        buttons |= IN_MOVERIGHT;

    LABEL_18:
        cmd->buttons = buttons;
    }
}

void Misc::SlowWalk(CUserCmd* cmd) {

    if (!g_Options.slowwalk)
        return;

    if (g_Options.ragebot_slowwalk_amt <= 0 || !GetAsyncKeyState(g_Options.ragebot_slowwalk_key))
        return;

    auto weapon_handle = g_LocalPlayer->m_hActiveWeapon().Get();

    if (!weapon_handle)
        return;

    if (g_LocalPlayer->m_nMoveType() != MOVETYPE_LADDER && g_LocalPlayer->m_fFlags() & FL_ONGROUND)
    {
        float amount = 0.0034f * g_Options.ragebot_slowwalk_amt; //max 100

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
}

CInput* m_input2() {
    if (!g_Input)
        g_Input = *reinterpret_cast<CInput**>(Utils::PatternScan2(XorStr("client.dll"), "B9 ? ? ? ? F3 0F 11 04 24 FF 50 10") + 0x1);

    return g_Input;
}

void Misc::SetThirdpersonAngles(ClientFrameStage_t stage, CUserCmd* cmd)
{
    if (stage != ClientFrameStage_t::FRAME_RENDER_START)
        return;

    if (g_EngineClient->IsInGame() && g_LocalPlayer && g_LocalPlayer->IsAlive())
    {
        if (m_input2()->m_fCameraInThirdPerson) {

            if (CAntiAim::Get().CanDesync(cmd) && g_Options.ragebot_antiaim_desync && g_Options.chams_fake_enabled)
            {
                g_LocalPlayer->SetVAngles(QAngle(Globals::fake.pitch, Globals::fake.yaw, Globals::fake.roll));
            }
            else if (CAntiAim::Get().CanDesync(cmd) && g_Options.ragebot_antiaim_desync && g_Options.chams_fake_enabled == false)
            {
                g_LocalPlayer->SetVAngles(QAngle(Globals::real.pitch, Globals::real.yaw, Globals::real.roll));
            }
            if (!CAntiAim::Get().CanDesync(cmd) || g_Options.ragebot_antiaim_desync == false)
            {
                g_LocalPlayer->SetVAngles(cmd->viewangles);
            }
        }
    }
}

bool fresh_tick() {
    static int old_tick_count;


    if (old_tick_count != g_GlobalVars->tickcount) {
        old_tick_count = g_GlobalVars->tickcount;
        return true;
    }
}

void Misc::local_animfix(C_BasePlayer* nigga, CUserCmd* cmd, bool bSendPacket) {

    auto entity = nigga;

    if (!entity || !entity->IsAlive() || !cmd)
        return;

    auto proper_abs = entity->GetAbsAngles();
    static auto sent_pose_params = entity->m_flPoseParameter();
    static AnimationLayer backup_layers[15];
    const auto org_tmp = g_LocalPlayer->GetRenderOrigin();

    if (fresh_tick()) {
        std::memcpy(backup_layers, entity->GetAnimOverlays(), (sizeof(AnimationLayer) * entity->NumOverlays()));

        entity->UpdateAnimationState(entity->GetPlayerAnimState(), cmd->viewangles);

        if (entity->GetPlayerAnimState())
            entity->GetPlayerAnimState()->m_iLastClientSideAnimationUpdateFramecount = g_GlobalVars->framecount - 1;

        entity->UpdateClientSideAnimation();
        if (!bSendPacket) {
            proper_abs = entity->GetAbsAngles();
            sent_pose_params = entity->m_flPoseParameter();
        }
    }

    if (entity->GetPlayerAnimState()->m_bInHitGroundAnimation)
        Globals::headthing = true;
    else
        Globals::headthing = false;;

    if (fding && !Globals::valve)
        entity->GetPlayerAnimState()->m_fDuckAmount = 1.0;

    if (g_Options.ragebot_antiaim_desync && CAntiAim::Get().CanDesync(cmd))
    {
        if (entity->GetPlayerAnimState()->m_flGoalFeetYaw != Globals::fake.yaw)
            entity->GetPlayerAnimState()->m_flGoalFeetYaw = Globals::fake.yaw;

        if (entity->GetPlayerAnimState()->m_flEyePitch != Globals::fake.pitch)
            entity->GetPlayerAnimState()->m_flEyePitch = Globals::fake.pitch;
    }
    else
    {
        if (entity->GetPlayerAnimState()->m_flGoalFeetYaw != cmd->viewangles.yaw)
            entity->GetPlayerAnimState()->m_flGoalFeetYaw = cmd->viewangles.yaw;

        if (entity->GetPlayerAnimState()->m_flEyePitch != cmd->viewangles.pitch)
            entity->GetPlayerAnimState()->m_flEyePitch = cmd->viewangles.pitch;
    }

    entity->SetAbsAngles(proper_abs);
    std::memcpy(entity->GetAnimOverlays(), backup_layers, (sizeof(AnimationLayer) * entity->NumOverlays()));
    entity->m_flPoseParameter() = sent_pose_params;

}

void Misc::desyncchams(CUserCmd* cmd, bool bSendPacket) {


    if (!g_LocalPlayer || !g_LocalPlayer->IsAlive()) {
        m_should_update_fake = true;
        return;
    }



    if (m_fake_spawntime != g_LocalPlayer->m_flSpawnTime() || m_should_update_fake) {
        init_fake_anim = false;
        m_fake_spawntime = g_LocalPlayer->m_flSpawnTime();
        m_should_update_fake = false;
    }

    if (!init_fake_anim) {
        m_fake_state = reinterpret_cast<CCSGOPlayerAnimState*> (g_pMemAlloc->Alloc(sizeof(CCSGOPlayerAnimState)));

        if (m_fake_state != nullptr)
            g_LocalPlayer->CreateAnimationState(m_fake_state);

        init_fake_anim = true;
    }

    if (bSendPacket) {
        int OldFrameCount = g_GlobalVars->framecount;
        int OldTickCount = g_GlobalVars->tickcount;

        g_GlobalVars->framecount = TIME_TO_TICKS(g_LocalPlayer->m_flSimulationTime());
        g_GlobalVars->tickcount = TIME_TO_TICKS(g_LocalPlayer->m_flSimulationTime());

        std::memcpy(m_fake_layers.data(), g_LocalPlayer->GetAnimOverlays(), sizeof(m_fake_layers));
        std::memcpy(m_fake_poses.data(), g_LocalPlayer->m_flPoseParameter().data(), sizeof(m_fake_poses));

        g_LocalPlayer->UpdateAnimationState(m_fake_state, cmd->viewangles);

        auto backup_absangles = g_LocalPlayer->GetAbsAngles();

        /* invalidate bone cache */
        g_LocalPlayer->GetMostRecentModelBoneCounter() = 0;
        g_LocalPlayer->GetLastBoneSetupTime() = -FLT_MAX;

        m_got_fake_matrix = g_LocalPlayer->SetupBones(m_fake_matrix, MAXSTUDIOBONES, BONE_USED_BY_ANYTHING & ~BONE_USED_BY_ATTACHMENT, g_GlobalVars->curtime);

        memcpy(m_fake_position_matrix, m_fake_matrix, sizeof(m_fake_position_matrix));

        const auto org_tmp = g_LocalPlayer->GetRenderOrigin();

        if (m_got_fake_matrix) {
            for (auto& i : m_fake_matrix) {
                i[0][3] -= org_tmp.x;
                i[1][3] -= org_tmp.y;
                i[2][3] -= org_tmp.z;
            }
        }

        //vvs
        g_LocalPlayer->SetAbsAngles(backup_absangles);

        g_LocalPlayer->GetMostRecentModelBoneCounter() = 0;
        g_LocalPlayer->GetLastBoneSetupTime() = -FLT_MAX;

        std::memcpy(g_LocalPlayer->GetAnimOverlays(), m_fake_layers.data(), sizeof(m_fake_layers));
        std::memcpy(g_LocalPlayer->m_flPoseParameter().data(), m_fake_poses.data(), sizeof(m_fake_poses));

        g_GlobalVars->framecount = OldFrameCount;
        g_GlobalVars->tickcount = OldTickCount;
    }

}

void Misc::ChatSpama(CUserCmd* cmd) {
    {

        if (!g_Options.misc_chatspam)
            return;

        if (Globals::stepping)
            return;

        if (g_Options.chatspamtype == 0) {
            static std::string chatspam[] =
            {
                XorStr("GUGAHACKS.KITCHEN | Probably Doin' You're Mother Currently."),
                XorStr("GUGAHACKS.KITCHEN | eat my penris"),
                XorStr("GUGAHACKS.KITCHEN | Me > You"),
                XorStr("GUGAHACKS.KITCHEN | Your IQ must be lower than your GPA"),
                XorStr("GUGAHACKS.KITCHEN | femboy fucked!!."),
                XorStr("GUGAHACKS.KITCHEN | Farting IS NOT Cumming"),
                XorStr("GUGAHACKS.KITCHEN | When I shit... I SHIT"),
                XorStr("GUGAHACKS.KITCHEN | Gugahacks > ALL"),
                XorStr("GUGAHACKS.KITCHEN | discord.gg/gugahacks"),
                XorStr("GUGAHACKS.KITCHEN | i.imgur.com/mucKbbq.gif"),
            };

            static auto lastspammed = 0;

            if (GetTickCount64() - lastspammed > 800)
            {
                lastspammed = GetTickCount64();

                srand(GetTickCount64());
                std::string msg = XorStr("say ") + chatspam[rand() % 10];

                g_EngineClient->ExecuteClientCmd(msg.c_str());
            }
        }

        if (g_Options.chatspamtype == 1) {
            static std::string chatspam[] =
            {
                XorStr("GUGAHACKS.KITCHEN | On February 12, 2019, the NAACP marked its 110th anniversary!"),
                XorStr("GUGAHACKS.KITCHEN | John Mercer Langston was the first Black man to become a lawyer!"),
                XorStr("GUGAHACKS.KITCHEN | Thurgood Marshall was the first African American ever appointed to the U.S. Supreme Court!"),
                XorStr("GUGAHACKS.KITCHEN | Shirley Chisholm was the first African American woman elected to the House of Representatives!"),
                XorStr("GUGAHACKS.KITCHEN | Hattie McDaniel was the first African American performer to win an Academy Award!"),
                XorStr("GUGAHACKS.KITCHEN | Robert Johnson became the first African American billionaire!"),
                XorStr("GUGAHACKS.KITCHEN | Phillis Wheatley was the first African American to publish a book of poetry!"),
                XorStr("GUGAHACKS.KITCHEN | Thurgood Marshall became the first African American to serve in the U.S. Supreme Court!"),
                XorStr("GUGAHACKS.KITCHEN | Nat King Cole was the first Black American to host a TV show!"),
                XorStr("GUGAHACKS.KITCHEN | Hattie McDaniel became the first Black person to win an Oscar!"),
            };

            static auto lastspammed = 0;

            if (GetTickCount64() - lastspammed > 800)
            {
                lastspammed = GetTickCount64();

                srand(GetTickCount64());
                std::string msg = XorStr("say ") + chatspam[rand() % 10];

                g_EngineClient->ExecuteClientCmd(msg.c_str());
            }
        }

    }
}