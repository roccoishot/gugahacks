#include "bhop.hpp"
#include "../options.hpp"

#include "../valve_sdk/csgostructs.hpp"
#include "../helpers/math.hpp"
#include <filesystem>

void BunnyHop::OnCreateMove(CUserCmd* cmd)
{
	if (!g_Options.misc_bhop2)
		return;
	if (!g_LocalPlayer)
		return;

	if (!g_LocalPlayer->IsAlive())
		return;

	if (g_LocalPlayer->m_nMoveType() == MOVETYPE_LADDER || g_LocalPlayer->m_nMoveType() == MOVETYPE_NOCLIP)
		return;

	static auto last_jumped = false;
	static auto should_jump = false;

	if (!last_jumped && should_jump)
	{
		should_jump = false;
		cmd->buttons |= IN_JUMP;
	}
	else if (cmd->buttons & IN_JUMP)
	{
		if (g_LocalPlayer->m_fFlags() & FL_ONGROUND || !g_LocalPlayer->m_vecVelocity().z)
		{
			last_jumped = true;
			should_jump = true;
		}
		else
		{
			cmd->buttons &= ~IN_JUMP;
			last_jumped = false;
		}
	}
	else
	{
		last_jumped = false;
		should_jump = false;
	}
}

void BunnyHop::AutoStafe(CUserCmd* cmd)
{
	if (!g_Options.autostrafe)
		return;

	if (g_LocalPlayer->m_fFlags() & FL_ONGROUND)
		return;

	if (g_Options.autostrafemode == 0) {
		static auto side = 1.f;
		side = -side;

		auto velocity = g_LocalPlayer->m_vecVelocity();

		QAngle wish_angle = cmd->viewangles;

		auto speed = velocity.Length2D();
		auto ideal_strafe = std::clamp(RAD2DEG(atan(15.f / speed)), 0.f, 90.f);

		cmd->forwardmove = 0.f;

		static auto cl_sidespeed = g_CVar->FindVar("cl_sidespeed");

		static float old_yaw = 0.f;
		auto yaw_delta = std::remainderf(wish_angle.yaw - old_yaw, 360.f);
		auto abs_yaw_delta = abs(yaw_delta);
		old_yaw = wish_angle.yaw;

		const auto cl_sidespeed_value = cl_sidespeed->GetFloat();

		if (abs_yaw_delta <= ideal_strafe || abs_yaw_delta >= 30.f)
		{
			QAngle velocity_direction;
			Math::VectorAngles(velocity, velocity_direction);
			auto velocity_delta = std::remainderf(wish_angle.yaw - velocity_direction.yaw, 360.0f);
			auto retrack = std::clamp(RAD2DEG(atan(30.f / speed)), 0.f, 90.f) * 2.f;
			if (velocity_delta <= retrack || speed <= 15.f)
			{
				if (-retrack <= velocity_delta || speed <= 15.0f)
				{
					wish_angle.yaw += side * ideal_strafe;
					cmd->sidemove = cl_sidespeed_value * side;
				}
				else
				{
					wish_angle.yaw = velocity_direction.yaw - retrack;
					cmd->sidemove = cl_sidespeed_value;
				}
			}
			else
			{
				wish_angle.yaw = velocity_direction.yaw + retrack;
				cmd->sidemove = -cl_sidespeed_value;
			}

			Math::CorrectMovement(cmd, wish_angle, cmd->viewangles);
		}
		else if (yaw_delta > 0.f)
			cmd->sidemove = -cl_sidespeed_value;
		else
			cmd->sidemove = cl_sidespeed_value;
	}
}

void BunnyHop::EBStrafe(CUserCmd* cmd)
{

	if (!GetAsyncKeyState(g_Options.edge_bug_key))
		return;

	static auto side = 1.f;
	side = -side;

	auto velocity = g_LocalPlayer->m_vecVelocity();

	QAngle wish_angle = cmd->viewangles;

	auto speed = velocity.Length2D();
	auto ideal_strafe = std::clamp(RAD2DEG(atan(15.f / speed)), 0.f, 90.f);

	cmd->forwardmove = 0.f;

	static auto cl_sidespeed = g_CVar->FindVar("cl_sidespeed");

	static float old_yaw = 0.f;
	auto yaw_delta = std::remainderf(wish_angle.yaw - old_yaw, 360.f);
	auto abs_yaw_delta = abs(yaw_delta);
	old_yaw = wish_angle.yaw;

	const auto cl_sidespeed_value = cl_sidespeed->GetFloat();

	if (abs_yaw_delta <= ideal_strafe || abs_yaw_delta >= 30.f)
	{
		QAngle velocity_direction;
		Math::VectorAngles(velocity, velocity_direction);
		auto velocity_delta = std::remainderf(wish_angle.yaw - velocity_direction.yaw, 360.0f);
		auto retrack = std::clamp(RAD2DEG(atan(30.f / speed)), 0.f, 90.f) * 2.f;
		if (velocity_delta <= retrack || speed <= 15.f)
		{
			if (-retrack <= velocity_delta || speed <= 15.0f)
			{
				wish_angle.yaw += side * ideal_strafe;
				cmd->sidemove = cl_sidespeed_value * side;
			}
			else
			{
				wish_angle.yaw = velocity_direction.yaw - retrack;
				cmd->sidemove = cl_sidespeed_value;
			}
		}
		else
		{
			wish_angle.yaw = velocity_direction.yaw + retrack;
			cmd->sidemove = -cl_sidespeed_value;
		}

		Math::CorrectMovement(cmd, wish_angle, cmd->viewangles);
	}
	else if (yaw_delta > 0.f)
		cmd->sidemove = -cl_sidespeed_value;
	else
		cmd->sidemove = cl_sidespeed_value;
}