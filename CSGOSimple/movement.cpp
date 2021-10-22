#include "movement.h"
#include "OldPrediction.h"
#include "menu.hpp"
#include "features/bhop.hpp"

class CHudChat
{
public:
    void ChatPrintf(int iPlayerIndex, int iFilter, const char* fmt, ...)
    {
        CallVFunction<void(__cdecl*)(void*, int, int, const char*, ...)>(this, 27)(this, iPlayerIndex, iFilter, fmt);
    }
};

static auto prediction = new PredictionSystem();

void movement::edgebug(CUserCmd* cmd) {
    auto backupflags = g_LocalPlayer->m_fFlags();

    g_CVar->FindVar("sv_min_jump_landing_sound")->SetValue("63464578");
        if (g_Options.ebmode == 0) {
            if (g_Options.edge_bug && GetAsyncKeyState(g_Options.edge_bug_key)) {
            if (g_LocalPlayer->m_fFlags() & FL_ONGROUND)
                cmd->buttons |= in_duck;
        }
    }

    if (g_Options.edge_bug && GetAsyncKeyState(g_Options.edge_bug_key)) {
        if (g_Options.ebmode == 1) {

            static bool edgebugging = false;
            static int edgebugging_tick = 0;

            if (!edgebugging) {
                float z_velocity = floor(g_LocalPlayer->m_vecVelocity().z);

                for (int i = 0; i < 64; i++) {
                    prediction->StartPrediction(cmd);
                    //Do it be ebing?!?!?!?
                    if (z_velocity < -7 && floor(g_LocalPlayer->m_vecVelocity().z) == -7 && (g_LocalPlayer->m_fFlags() & FL_ONGROUND) && g_LocalPlayer->m_nMoveType() != MOVETYPE_NOCLIP) {
                        edgebugging = true;
                        edgebugging_tick = cmd->tick_count + i;
                        break;
                    }
                    else {
                        z_velocity = floor(g_LocalPlayer->m_vecVelocity().z);
                        backupflags = g_LocalPlayer->m_fFlags();
                    }
                    prediction->EndPrediction();
                }
            }

            else {

                //Autostrafe that so that we be quick

                //Lockin movement like fort knox yuhnumsain
                cmd->sidemove = 0.f;
                cmd->forwardmove = 0.f;
                cmd->upmove = 0.f;
                cmd->mousedx = 0.f;
                cmd->mousedy = 0.f;

                //*yawn* is this eb done yet (cringe #fail)
                if (cmd->tick_count > edgebugging_tick) {
                    edgebugging = false;
                    edgebugging_tick = 0;
                }
                //Stompthem Kay Very AngY!
            }

            float max_radias = DirectX::XM_2PI;
            float step = max_radias / 128;
            float xThick = 23;
            bool did_jump;
            bool edgebugz;
            bool unduck;
            Vector pos = g_LocalPlayer->m_vecOrigin();

            for (float a = 0.f; a < max_radias; a += step)
            {
                Vector pt;
                pt.x = ((xThick - 20.f) * cos(a)) + pos.x;
                pt.y = ((xThick - 20.f) * sin(a)) + pos.y;
                pt.z = pos.z;

                Vector pt2 = pt;
                pt2.z -= 6;

                trace_t edgebug;
                trace_t fag;

                Ray_t ray;
                ray.Init(pt, pt2);

                CTraceFilter flt;
                flt.pSkip = g_LocalPlayer;
                g_EngineTrace->TraceRay(ray, MASK_SOLID_BRUSHONLY, &flt, &fag);

                if (ray.m_IsRay != 1.f && edgebug.fraction != 0.f)
                {
                    did_jump = true;
                    cmd->buttons |= in_duck;
                    BunnyHop::EBStrafe(cmd);
                    unduck = true;
                    edgebugz = true;
                }
            }
        }
    }
}


void movement::jumpbug(CUserCmd* pCmd) {
    if (g_Options.jump_bug && GetAsyncKeyState(g_Options.jump_bug_key)) {
        g_Options.misc_bhop2 = false;
        static bool bShouldJumpNext = false;

        if (bShouldJumpNext)
        {
            pCmd->buttons |= in_jump;
            bShouldJumpNext = false;
            return;
        }

        if ((g_LocalPlayer->m_fFlags() & FL_ONGROUND))
        {
            pCmd->buttons |= in_duck;
            pCmd->buttons &= ~in_jump;
            bShouldJumpNext = true;
        }
    }
    else
        g_Options.misc_bhop2 = true;
}