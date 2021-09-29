#include "movement.h"

void movement::edgebug(CUserCmd* cmd) {
	if (g_LocalPlayer->IsAlive() && g_EngineClient->IsInGame())
	{
		if (g_Options.edge_bug && GetAsyncKeyState(g_Options.edge_bug_key)) {
			if (!(g_LocalPlayer->m_fFlags() & FL_ONGROUND))
			return;
		
			if (g_LocalPlayer->m_fFlags() & FL_ONGROUND)
			cmd->buttons |= IN_DUCK;
		}
	}
}

void movement::jumpbug(CUserCmd* cmd) {
    float max_radias = DirectX::XM_PI * 2;
    float step = max_radias / 128;
    float xThick = 23;
    static bool bDidJump = true;
    static bool unduck = true;
    if (g_Options.jump_bug && GetAsyncKeyState(g_Options.jump_bug_key)) {
        if (g_LocalPlayer->m_fFlags() & FL_ONGROUND) {
            g_Options.misc_bhop2 = false;
            if (unduck) {
                bDidJump = false;
                cmd->buttons &= ~in_duck; // duck
                cmd->buttons |= in_jump; // jump
                unduck = false;
            }
            Vector pos = g_LocalPlayer->abs_origin();
            for (float a = 0.f; a < max_radias; a += step) {
                Vector pt;
                pt.x = (xThick * cos(a)) + pos.x;
                pt.y = (xThick * sin(a)) + pos.y;
                pt.z = pos.z;


                Vector pt2 = pt;
                pt2.z -= 4;

                trace_t fag;

                Ray_t ray;
                ray.Init(pt, pt2);

                CTraceFilter flt;
                flt.pSkip = g_LocalPlayer;
                g_EngineTrace->TraceRay(ray, MASK_SOLID_BRUSHONLY, &flt, &fag);

                if (fag.fraction != 1.f && fag.fraction != 0.f) {
                    bDidJump = true;
                    cmd->buttons |= in_duck; // duck
                    cmd->buttons &= ~in_jump; // jump
                    unduck = true;
                }
            }
            for (float a = 0.f; a < max_radias; a += step) {
                Vector pt;
                pt.x = ((xThick - 2.f) * cos(a)) + pos.x;
                pt.y = ((xThick - 2.f) * sin(a)) + pos.y;
                pt.z = pos.z;

                Vector pt2 = pt;
                pt2.z -= 4;

                trace_t fag;

                Ray_t ray;
                ray.Init(pt, pt2);

                CTraceFilter flt;
                flt.pSkip = g_LocalPlayer;
                g_EngineTrace->TraceRay(ray, MASK_SOLID_BRUSHONLY, &flt, &fag);

                if (fag.fraction != 1.f && fag.fraction != 0.f) {
                    bDidJump = true;
                    cmd->buttons |= in_duck; // duck
                    cmd->buttons &= ~in_jump; // jump
                    unduck = true;
                }
            }
            for (float a = 0.f; a < max_radias; a += step) {
                Vector pt;
                pt.x = ((xThick - 20.f) * cos(a)) + pos.x;
                pt.y = ((xThick - 20.f) * sin(a)) + pos.y;
                pt.z = pos.z;

                Vector pt2 = pt;
                pt2.z -= 4;

                trace_t fag;

                Ray_t ray;
                ray.Init(pt, pt2);

                CTraceFilter flt;
                flt.pSkip = g_LocalPlayer;
                g_EngineTrace->TraceRay(ray, MASK_SOLID_BRUSHONLY, &flt, &fag);

                if (fag.fraction != 1.f && fag.fraction != 0.f) {
                    bDidJump = true;
                    cmd->buttons |= in_duck; // duck
                    cmd->buttons &= ~in_jump; // jump
                    unduck = true;
                }
            }
        }
    }
    else g_Options.misc_bhop2 = true;
}