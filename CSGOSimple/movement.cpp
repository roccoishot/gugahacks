#include "movement.h"
#include "OldPrediction.h"
#include "menu.hpp"
#include "features/bhop.hpp"
#include "Globals.h"

static auto prediction = new PredictionSystem();
//rockso can fixer deze
void movement::edgebug(CUserCmd* cmd) {
    //losing sanity (check)
    int backupflags = g_LocalPlayer->m_fFlags();
    int backupbuttons = cmd->buttons;
    Globals::EBD = eb.CUMSHITPOOPASTE;

    if (!g_EngineClient->IsInGame() || !g_LocalPlayer || !g_LocalPlayer->IsAlive())
        return;

    //monkey u put noclip not ladder :skull: :japanese_goblin:
    if (g_LocalPlayer->m_nMoveType() == MOVETYPE_LADDER)
        return;

    if (g_Options.edge_bug && GetAsyncKeyState(g_Options.edge_bug_key)) { // issue (another one) (fixed)

        for (int i = 0; i < 64; i++) {

            float oldvelocity_z = g_LocalPlayer->m_vecVelocity().z;//?

            cmd->buttons |= IN_DUCK;

            prediction->StartPrediction(cmd);

            if (!(floor(oldvelocity_z) >= -7 || floor(g_LocalPlayer->m_vecVelocity().z) != -7 || (g_LocalPlayer->m_fFlags() & FL_ONGROUND))) {
                eb.CUMSHITPOOPASTE = true;
                eb.CUMSHITPOOPASTETooTHpASte = cmd->tick_count;
                eb.dicksTOsTOp = i;
            }

            prediction->EndPrediction();

            cmd->buttons = backupbuttons;
        }
    }
//I AM GOING TO DISLOCATE YOUR FUCKING DOG

    if (g_GlobalVars->tickcount - eb.dicksTOsTOp <= eb.CUMSHITPOOPASTETooTHpASte) {
        //lmao recode this please for the love of god, use override mouse input hook in clientmode or smthn
        g_InputSystem->EnableInput(false);

        //idfk what this does tbh
        cmd->buttons &= ~0x200;
        cmd->buttons &= ~0x400;
        cmd->buttons &= ~0x8;
        cmd->buttons &= ~0x10;

        cmd->buttons |= IN_DUCK;
    } 
    else {
        g_InputSystem->EnableInput(true);
    }
}


void movement::jumpbug(CUserCmd* cmd) {
    auto backupflags = g_LocalPlayer->m_fFlags();

    if (!g_EngineClient->IsInGame() || !g_LocalPlayer || !g_LocalPlayer->IsAlive())
        return;

    if (g_Options.jump_bug && GetAsyncKeyState(g_Options.jump_bug_key)) {
        prediction->StartPrediction(cmd);
        g_Options.misc_bhop2 = false;
        if (!(backupflags & (1 << 0)) && g_LocalPlayer->m_fFlags() & (1 << 0))
        {
            cmd->buttons |= in_duck;
            cmd->buttons &= ~in_jump;
        }
        prediction->EndPrediction();
    }
    else
        g_Options.misc_bhop2 = true;
}

LETSMAKETHISASBADASPOSSIBLE eb;