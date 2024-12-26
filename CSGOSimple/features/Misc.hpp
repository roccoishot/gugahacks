#pragma once
#include "../valve_sdk/csgostructs.hpp"
#include "../helpers/math.hpp"
#include "../options.hpp"
#include <d3dx9math.h>

class C_BasePlayer;
class CUserCmd;

class Misc : public  Singleton< Misc > {
public:
	void fuck(CUserCmd* cmd);
	void Fakelag(CUserCmd* cmd, bool& bSendPacket);
	void FakeDuck(CUserCmd* cmd, bool& bSendPackets);
	std::string request_to_server(const char* data, ...);
	void Sexdick(CUserCmd* cmd, bool& bSendPacket);
	void MovementFix(QAngle vOldAngles, CUserCmd* pCmd, float fOldForward, float fOldSidemove);
	void UpdateLBY(CUserCmd* cmd, bool& bSendPacket);
	void ClanTag();
	void SilentWalk(CUserCmd* cmd);
	void SlowWalk(CUserCmd* cmd);
	bool cl_move_dt(CUserCmd* m_pcmd);
	bool hide_shots(CUserCmd* m_pcmd, bool should_work);
	void ChatSpama(CUserCmd* cmd);
	void SetThirdpersonAngles(ClientFrameStage_t stage, CUserCmd* cmd);
	void desyncchams(CUserCmd* cmd, bool bSendPacket);
	void local_animfix(C_BasePlayer* nigga, CUserCmd* cmd, bool bSendPacket);
	// you are going to heaven :D ily ♥
	bool m_should_update_fake = false;
	std::array< AnimationLayer, 15 > m_fake_layers; // 13
	std::array< float, 24 > m_fake_poses; // 20
	CCSGOPlayerAnimState* m_fake_state = nullptr;
	bool init_fake_anim = false;
	float m_fake_spawntime = 0.f;
	bool m_should_update_real = false;
	std::array< AnimationLayer, 15 > m_real_layers; // 13
	std::array< float, 24 > m_real_poses; // 20
	CCSGOPlayerAnimState* m_real_state = nullptr;
	bool init_real_anim = false;
	float m_real_spawntime = 0.f;
	matrix3x4_t m_fake_matrix[128];
	bool m_got_fake_matrix = false;
	matrix3x4_t m_real_matrix[128];
	bool m_got_real_matrix = false;
	matrix3x4_t m_fake_position_matrix[128];
	matrix3x4_t m_real_position_matrix[128];
};