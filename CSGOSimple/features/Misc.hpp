#pragma once
#include "../valve_sdk/csgostructs.hpp"
#include "../helpers/math.hpp"
#include "../options.hpp"
#include <d3dx9math.h>

class C_BasePlayer;
class CUserCmd;

namespace Misc
{
	void MovementFixxa(CUserCmd* m_Cmd, QAngle wish_angle, QAngle old_angles);
	void MovementFix(QAngle vOldAngles, CUserCmd* pCmd, float fOldForward, float fOldSidemove);
	void Fakelag(CUserCmd* cmd, bool& bSendPacket);
	void UpdateLBY(CUserCmd* cmd, bool& bSendPacket);
	void ClanTag();
	void SilentWalk(CUserCmd* cmd);
	void SlowWalk(CUserCmd* cmd);
	void ChatSpama(CUserCmd* cmd);
	void SetThirdpersonAngles(ClientFrameStage_t stage, CUserCmd* cmd);
}