#pragma once

class C_BasePlayer;
class CUserCmd;

namespace BunnyHop
{
    void OnCreateMove(CUserCmd* cmd);
    void AutoStafe(CUserCmd* pCmd);
    void EBStrafe(CUserCmd* cmd);

}