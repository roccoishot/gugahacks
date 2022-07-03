#pragma once
#include "./valve_sdk/csgostructs.hpp"

enum class YawAntiAims : int
{
	NONE,
	BACKWARDS,
	SPINBOT,
	LOWER_BODY,
	FREESTAND,
	CUSTOM,
	JITTER,
	AFRICANAMERICANS
};

enum class PitchAntiAims : int
{
	NONE,
	EMOTION,
	DOWN,
	UP,
	ZERO,
	FUP,
	FDOWN,
	FZERO

};

class CAntiAim : public Singleton<CAntiAim>
{
public:
	void CreateMove(CUserCmd* cmd, bool& bSendPacket);
	bool CanDesync(CUserCmd* cmd);
	bool inverted;
	float sexyboodyballs;

private:
	void DoAntiAim(CUserCmd* cmd, bool& bSendPacket);
	void Pitch(CUserCmd* cmd, bool fake);
	void Yaw(CUserCmd* cmd, bool fake);
};