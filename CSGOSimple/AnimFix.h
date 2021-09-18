#pragma once
#include "hooks.hpp"

class AnimFix
{
public:
	void UpdateFakeState();
	void ApplyLocalPlayer();
private:
	CCSGOPlayerAnimState* RealAnimstate = nullptr;
	CCSGOPlayerAnimState* FakeAnimstate = nullptr;
	CCSGOPlayerAnimState* FakeAnimstate2 = nullptr;
	CCSGOPlayerAnimState* FakeAnimstate3 = nullptr;

	QAngle g_FakeAngle;
	Vector g_VisualAngle;
	matrix3x4_t fakematrix[128];
	float            m_real_poses[24];
	AnimationLayer m_real_layers[13];
	bool ShouldUpdate;
};

extern AnimFix* g_Animfix;