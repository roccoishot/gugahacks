// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "DirectStrafe.hpp"
#include "./helpers/math.hpp"
#include "xor.h"
#ifdef ENABLE_XOR
#define XorStr _xor_ 
#else
#define XorStr
#endif
#pragma intrinsic(_ReturnAddress)  
#define CheckIfNonValidNumber(x) (fpclassify(x) == FP_INFINITE || fpclassify(x) == FP_NAN || fpclassify(x) == FP_SUBNORMAL)

void airstrafe::create_move(CUserCmd* m_pcmd) //-V2008
{
	if (!g_LocalPlayer)
		return;

	if (!g_LocalPlayer->IsAlive())
		return;

	if (!g_Options.autostrafe)
		return;

	if (g_LocalPlayer->m_fFlags() & FL_ONGROUND)
		return;

	if (g_Options.autostrafemode == 1) {
		static auto cl_sidespeed = g_CVar->FindVar(XorStr("cl_sidespeed"));
		auto side_speed = cl_sidespeed->GetFloat();
		static auto old_yaw = 0.0f;

		auto get_velocity_degree = [](float velocity)
		{
			auto tmp = RAD2DEG(atan(30.0f / velocity));

			if (CheckIfNonValidNumber(tmp) || tmp > 90.0f)
				return 90.0f;

			else if (tmp < 0.0f)
				return 0.0f;
			else
				return tmp;
		};

		if (g_LocalPlayer->m_nMoveType() != MOVETYPE_WALK)
			return;

		auto velocity = g_LocalPlayer->m_vecVelocity();
		velocity.z = 0.0f;

		auto forwardmove = m_pcmd->forwardmove;
		auto sidemove = m_pcmd->sidemove;

		if (velocity.Length2D() < 5.0f && !forwardmove && !sidemove)
			return;

		static auto flip = false;
		flip = !flip;

		auto turn_direction_modifier = flip ? 1.0f : -1.0f;
		auto viewangles = m_pcmd->viewangles;

		if (forwardmove || sidemove)
		{
			m_pcmd->forwardmove = 0.0f;
			m_pcmd->sidemove = 0.0f;

			auto turn_angle = atan2(-sidemove, forwardmove);
			viewangles.yaw += turn_angle * M_RADPI;
		}
		else if (forwardmove) //-V550
			m_pcmd->forwardmove = 0.0f;

		auto strafe_angle = RAD2DEG(atan(15.0f / velocity.Length2D()));

		if (strafe_angle > 90.0f)
			strafe_angle = 90.0f;
		else if (strafe_angle < 0.0f)
			strafe_angle = 0.0f;

		auto temp = Vector(0.0f, viewangles.yaw - old_yaw, 0.0f);
		temp.y = Math::normalize_yaw(temp.y);

		auto yaw_delta = temp.y;
		old_yaw = viewangles.yaw;

		auto abs_yaw_delta = fabs(yaw_delta);

		if (abs_yaw_delta <= strafe_angle || abs_yaw_delta >= 30.0f)
		{
			Vector velocity_angles;
			Math::vector_angles(velocity, velocity_angles);

			temp = Vector(0.0f, viewangles.yaw - velocity_angles.y, 0.0f);
			temp.y = Math::normalize_yaw(temp.y);

			auto velocityangle_yawdelta = temp.y;
			auto velocity_degree = get_velocity_degree(velocity.Length2D());

			if (velocityangle_yawdelta <= velocity_degree || velocity.Length2D() <= 15.0f)
			{
				if (-velocity_degree <= velocityangle_yawdelta || velocity.Length2D() <= 15.0f)
				{
					viewangles.yaw += strafe_angle * turn_direction_modifier;
					m_pcmd->sidemove = side_speed * turn_direction_modifier;
				}
				else
				{
					viewangles.yaw = velocity_angles.y - velocity_degree;
					m_pcmd->sidemove = side_speed;
				}
			}
			else
			{
				viewangles.yaw = velocity_angles.y + velocity_degree;
				m_pcmd->sidemove = -side_speed;
			}
		}
		else if (yaw_delta > 0.0f)
			m_pcmd->sidemove = -side_speed;
		else if (yaw_delta < 0.0f)
			m_pcmd->sidemove = side_speed;

		auto move = Vector(m_pcmd->forwardmove, m_pcmd->sidemove, 0.0f);
		auto speed = move.Length();

		Vector angles_move;
		Math::vector_angles(move, angles_move);

		auto normalized_x = fmod(m_pcmd->viewangles.pitch + 180.0f, 360.0f) - 180.0f;
		auto normalized_y = fmod(m_pcmd->viewangles.yaw + 180.0f, 360.0f) - 180.0f;

		auto yaw = DEG2RAD(normalized_y - viewangles.yaw + angles_move.y);

		if (normalized_x >= 90.0f || normalized_x <= -90.0f || m_pcmd->viewangles.pitch >= 90.0f && m_pcmd->viewangles.pitch <= 200.0f || m_pcmd->viewangles.pitch <= -90.0f && m_pcmd->viewangles.pitch <= 200.0f) //-V648
			m_pcmd->forwardmove = -cos(yaw) * speed;
		else
			m_pcmd->forwardmove = cos(yaw) * speed;

		m_pcmd->sidemove = sin(yaw) * speed;
	}
}