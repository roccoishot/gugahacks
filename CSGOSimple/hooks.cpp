#include "hooks.hpp"
#include <intrin.h>  

#include "render.hpp"
#include "menu.hpp"
#include "options.hpp"
#include "helpers/input.hpp"
#include "helpers/utils.hpp"
#include "features/bhop.hpp"
#include "features/chams.hpp"
#include "features/visuals.hpp"
#include "features/skins.h"
#include "features/Misc.hpp"
#include "backtrack.h"
#include "./xor.h"
#include "BetaAA.h"
#include "OldPrediction.h"
#include "Globals.h"
#include "Colormodulation.h"
#include "movement.h"
#include "crypt_str.h"
#include "blockbot.hpp"
#include "lagcompensation.h"
#include "FixSkyboxes.h"
#include "ragebot/ragebot.hpp"
#ifdef ENABLE_XOR
#define XorStr _xor_ 
#else
#define XorStr
#endif
#pragma intrinsic(_ReturnAddress)  

namespace Hooks {

	void Initialize()
	{
		hlclient_hook.setup(g_CHLClient);
		direct3d_hook.setup(g_D3DDevice9);
		vguipanel_hook.setup(g_VGuiPanel);
		vguisurf_hook.setup(g_VGuiSurface);
		sound_hook.setup(g_EngineSound);
		mdlrender_hook.setup(g_MdlRender);
		clientmode_hook.setup(g_ClientMode);
		cm_hook = new vmthook(reinterpret_cast<DWORD**>(g_ClientMode));
		stdrender_hook.setup(g_StudioRender);
		viewrender_hook.setup(g_ViewRender);

		ConVar* sv_cheats_con = g_CVar->FindVar("sv_cheats");
		sv_cheats.setup(sv_cheats_con);


		gameevents_hook.setup(g_GameEvents);
		gameevents_hook.hook_index(index::FireEvent, hkFireEvent);
		direct3d_hook.hook_index(index::EndScene, hkEndScene);
		direct3d_hook.hook_index(index::Reset, hkReset);
		hlclient_hook.hook_index(index::FrameStageNotify, hkFrameStageNotify);
		//hlclient_hook.hook_index(index::CreateMove, hkCreateMove_Proxy); // uncomment when fixed lmao!
		cm_hook->hook_function(reinterpret_cast<uintptr_t>(hkCreateMove), 24);
		vguipanel_hook.hook_index(index::PaintTraverse, hkPaintTraverse);
		sound_hook.hook_index(index::EmitSound1, hkEmitSound1);
		vguisurf_hook.hook_index(index::LockCursor, hkLockCursor);
		mdlrender_hook.hook_index(index::DrawModelExecute, hkDrawModelExecute);
		clientmode_hook.hook_index(index::DoPostScreenSpaceEffects, hkDoPostScreenEffects);
		clientmode_hook.hook_index(index::OverrideView, hkOverrideView);
		sv_cheats.hook_index(index::SvCheatsGetBool, hkSvCheatsGetBool);
		stdrender_hook.hook_index(index::DrawModelExecute2, hkDrawModelExecute2);
		viewrender_hook.hook_index(index::RenderSmokeOverlay, RenderSmokeOverlay);

		sequence_hook = new recv_prop_hook(C_BaseViewModel::m_nSequence(), RecvProxy);
	}
	//--------------------------------------------------------------------------------
	void Shutdown()
	{
		hlclient_hook.unhook_all();
		direct3d_hook.unhook_all();
		vguipanel_hook.unhook_all();
		vguisurf_hook.unhook_all();
		mdlrender_hook.unhook_all();
		clientmode_hook.unhook_all();
		sound_hook.unhook_all();
		sv_cheats.unhook_all();

		sequence_hook->~recv_prop_hook();

	}

	bool __fastcall send_net_msg(void* ecx, void* edx, INetMessage* msg, bool reliable, bool voice)
	{
		static auto oFireEvent = hk_netchannel.get_original<sendnetmsg_fn>(40);

		if (!msg)
			return original_sendnetmsg(ecx, msg, reliable, voice);


		if (msg->GetType() == 14)
			return false;

		return oFireEvent(ecx, msg, reliable, voice);
	}

	bool __stdcall hkFireEvent(IGameEvent* pEvent) {
		static auto oFireEvent = gameevents_hook.get_original<FireEvent>(index::FireEvent);

		if (!pEvent)
			return oFireEvent(g_GameEvents, pEvent);

		const char* szEventName = pEvent->GetName();


		if (!strcmp(szEventName, "server_spawn"))
		{

			const auto net_channel = g_EngineClient->GetNetChannelInfo();

			if (net_channel != nullptr)
			{
				const auto index = 40;
				Hooks::hk_netchannel.setup(net_channel);
				Hooks::hk_netchannel.hook_index(index, send_net_msg);
			}
		}

		return oFireEvent(g_GameEvents, pEvent);
	}
	//--------------------------------------------------------------------------------
	long __stdcall hkEndScene(IDirect3DDevice9* pDevice)
	{
		static auto oEndScene = direct3d_hook.get_original<decltype(&hkEndScene)>(index::EndScene);

		static uintptr_t gameoverlay_return_address = 0;

		if (!gameoverlay_return_address) {
			MEMORY_BASIC_INFORMATION info;
			VirtualQuery(_ReturnAddress(), &info, sizeof(MEMORY_BASIC_INFORMATION));

			char mod[MAX_PATH];
			GetModuleFileNameA((HMODULE)info.AllocationBase, mod, MAX_PATH);

			if (strstr(mod, XorStr("gameoverlay")))
				gameoverlay_return_address = (uintptr_t)(_ReturnAddress());
		}

		if (gameoverlay_return_address != (uintptr_t)(_ReturnAddress()) && g_Options.antiobs)
			return oEndScene(pDevice);

		//Viewmodel Sheeeesh 
		//Viewmodel FOV
		if (g_Options.enablechanger) {
			static auto viewmodel_fov = g_CVar->FindVar("viewmodel_fov");
			viewmodel_fov->m_fnChangeCallbacks.m_Size = 0;
			viewmodel_fov->SetValue(g_Options.viewmodel_fov);
			//Viewmodel X
			static auto viewmodel_offset_x = g_CVar->FindVar("viewmodel_offset_x");
			viewmodel_offset_x->m_fnChangeCallbacks.m_Size = 0;
			viewmodel_offset_x->SetValue(g_Options.viewmodel_offset_x);
			//Viewmodel Y
			static auto viewmodel_offset_y = g_CVar->FindVar("viewmodel_offset_y");
			viewmodel_offset_y->m_fnChangeCallbacks.m_Size = 0;
			viewmodel_offset_y->SetValue(g_Options.viewmodel_offset_y);
			//Viewmodel Z
			static auto viewmodel_offset_z = g_CVar->FindVar("viewmodel_offset_z");
			viewmodel_offset_z->m_fnChangeCallbacks.m_Size = 0;
			viewmodel_offset_z->SetValue(g_Options.viewmodel_offset_z);
		}

		//sv_cheats force
		static auto cheatsforcer = g_CVar->FindVar("sv_cheats");
		cheatsforcer->m_fnChangeCallbacks.m_Size = 0;
		cheatsforcer->SetValue(1);

		if (g_Options.fullbright == true) {
			static auto fullbright = g_CVar->FindVar("mat_fullbright");
			fullbright->m_fnChangeCallbacks.m_Size = 0;
			fullbright->SetValue(1);
		}
		if (g_Options.fullbright == false) {
			static auto fullbright = g_CVar->FindVar("mat_fullbright");
			fullbright->m_fnChangeCallbacks.m_Size = 0;
			fullbright->SetValue(0);
		}

		//Metallic Props
		if (g_Options.metallica == true) {
			static auto metallica = g_CVar->FindVar("r_showenvcubemap");
			metallica->m_fnChangeCallbacks.m_Size = 0;
			metallica->SetValue(1);
		}
		if (g_Options.metallica == false) {
			static auto metallica = g_CVar->FindVar("r_showenvcubemap");
			metallica->m_fnChangeCallbacks.m_Size = 0;
			metallica->SetValue(0);
		}

		static auto r_modelAmbientMin = g_CVar->FindVar("r_modelAmbientMin");
		if (g_Options.amibence && r_modelAmbientMin->GetFloat() != g_Options.amibence * 0.05f) //-V550
			r_modelAmbientMin->SetValue(g_Options.amibence * 0.05f);
		else if ((g_Options.amibence < 2.f) && r_modelAmbientMin->GetFloat())
			r_modelAmbientMin->SetValue(0.0f);


		//Shot Info
		if (g_Options.shotinfo == false) {
			static auto shotinfo = g_CVar->FindVar("cl_weapon_debug_show_accuracy");
			shotinfo->m_fnChangeCallbacks.m_Size = 0;
			shotinfo->SetValue(0);
		}
		if (g_Options.shotinfo == true) {
			static auto shotinfo = g_CVar->FindVar("cl_weapon_debug_show_accuracy");
			shotinfo->m_fnChangeCallbacks.m_Size = 0;
			shotinfo->SetValue(2);
		}

		//Matrix Mode
		if (g_Options.matrix == true) {
			static auto matrix = g_CVar->FindVar("mat_luxels");
			matrix->m_fnChangeCallbacks.m_Size = 0;
			matrix->SetValue(1);
		}
		if (g_Options.matrix == false) {
			static auto matrix = g_CVar->FindVar("mat_luxels");
			matrix->m_fnChangeCallbacks.m_Size = 0;
			matrix->SetValue(0);
		}

		//Disable Blur
		static auto disableblur = g_CVar->FindVar("@panorama_disable_blur");
		disableblur->m_fnChangeCallbacks.m_Size = 0;
		disableblur->SetValue(g_Options.removeblur);

		//aspectratio
		if (g_Options.aspectchange == true) {
			static auto r_aspectratio = g_CVar->FindVar("r_aspectratio");
			r_aspectratio->m_fnChangeCallbacks.m_Size = 0;
			r_aspectratio->SetValue(g_Options.aspectratio);
		}
		else if (g_Options.aspectchange == false) {
			static auto r_aspectratio = g_CVar->FindVar("r_aspectratio");
			r_aspectratio->m_fnChangeCallbacks.m_Size = 0;
			r_aspectratio->SetValue("0");
		}

		//Fake Ping Exploits
		if (g_Options.fakepingkey == 0) {
			if (g_Options.fakeping == true && g_EngineClient->IsInGame()) {
				static auto fakeping = g_CVar->FindVar("net_fakelag");
				fakeping->m_fnChangeCallbacks.m_Size = 0;
				fakeping->SetValue(g_Options.fakepingzzz);
				static auto unlag = g_CVar->FindVar("sv_maxunlag");
				unlag->m_fnChangeCallbacks.m_Size = 0;
				unlag->SetValue(g_Options.fakepingzzz);
			}
			if (g_Options.fakeping == false) {
				static auto fakeping = g_CVar->FindVar("net_fakelag");
				fakeping->m_fnChangeCallbacks.m_Size = 0;
				fakeping->SetValue(0);
				static auto unlag = g_CVar->FindVar("sv_maxunlag");
				unlag->m_fnChangeCallbacks.m_Size = 0;
				unlag->SetValue("0.200");
			}
		}
		if (!g_Options.fakepingkey == 0) {
			if (GetAsyncKeyState(g_Options.fakepingkey)) {
				if (g_Options.fakeping && g_EngineClient->IsInGame()) {
					static auto fakeping = g_CVar->FindVar("net_fakelag");
					fakeping->m_fnChangeCallbacks.m_Size = 0;
					fakeping->SetValue(g_Options.fakepingzzz);
					static auto unlag = g_CVar->FindVar("sv_maxunlag");
					unlag->m_fnChangeCallbacks.m_Size = 0;
					unlag->SetValue(g_Options.fakepingzzz);
				}
			}
			else {
				static auto fakeping = g_CVar->FindVar("net_fakelag");
				fakeping->m_fnChangeCallbacks.m_Size = 0;
				fakeping->SetValue(0);
				static auto unlag = g_CVar->FindVar("sv_maxunlag");
				unlag->m_fnChangeCallbacks.m_Size = 0;
				unlag->SetValue("0.200");
			}
		}

		//Nade Prediction
		static auto pnade = g_CVar->FindVar("cl_grenadepreview");
		pnade->SetValue(g_Options.pnade);

		//Ragdoll Gravity
		if (g_Options.ragfloat == true) {
			static auto levito = g_CVar->FindVar("cl_ragdoll_gravity");
			levito->SetValue("-1");
		}
		if (g_Options.ragfloat == false) {
			static auto levito = g_CVar->FindVar("cl_ragdoll_gravity");
			levito->SetValue("600");
		}

		//Sky Changer
		if (g_Options.sky_changer) {
			if (g_Options.skyshitsssss == 0) {
				static auto buttsex = g_CVar->FindVar("sv_skyname");
				buttsex->SetValue("sky_csgo_night02");
			}
			if (g_Options.skyshitsssss == 1) {
				static auto buttsex = g_CVar->FindVar("sv_skyname");
				buttsex->SetValue("cs_baggage_skybox_");
			}
			if (g_Options.skyshitsssss == 2) {
				static auto buttsex = g_CVar->FindVar("sv_skyname");
				buttsex->SetValue("cs_tibet");
			}
			if (g_Options.skyshitsssss == 3) {
				static auto buttsex = g_CVar->FindVar("sv_skyname");
				buttsex->SetValue("vietnam");
			}
			if (g_Options.skyshitsssss == 4) {
				static auto buttsex = g_CVar->FindVar("sv_skyname");
				buttsex->SetValue("sky_lunacy");
			}
			if (g_Options.skyshitsssss == 5) {
				static auto buttsex = g_CVar->FindVar("sv_skyname");
				buttsex->SetValue("embassy");
			}
			if (g_Options.skyshitsssss == 6) {
				static auto buttsex = g_CVar->FindVar("sv_skyname");
				buttsex->SetValue("italy");
			}
			if (g_Options.skyshitsssss == 7) {
				static auto buttsex = g_CVar->FindVar("sv_skyname");
				buttsex->SetValue("italy");
			}
			if (g_Options.skyshitsssss == 8) {
				static auto buttsex = g_CVar->FindVar("sv_skyname");
				buttsex->SetValue("office");
			}
			if (g_Options.skyshitsssss == 9) {
				static auto buttsex = g_CVar->FindVar("sv_skyname");
				buttsex->SetValue("sky_cs15_daylight02_hdr");
			}
			if (g_Options.skyshitsssss == 10) {
				static auto buttsex = g_CVar->FindVar("sv_skyname");
				buttsex->SetValue("sky_csgo_cloudy01");
			}
			if (g_Options.skyshitsssss == 11) {
				static auto buttsex = g_CVar->FindVar("sv_skyname");
				buttsex->SetValue("sky_dust");
			}
		}

		//Recoil Crosshair
		if (g_Options.rcross == false) {
			static auto recoilcrosshaii = g_CVar->FindVar("cl_crosshair_recoil");
			recoilcrosshaii->SetValue(0);
		}
		else if (g_Options.rcross == true) {
			static auto recoilcrosshaii = g_CVar->FindVar("cl_crosshair_recoil");
			recoilcrosshaii->SetValue(1);
		}

		if (g_Options.disablepro == true) {
			static auto GETITOUT = g_CVar->FindVar("mat_postprocess_enable");
			GETITOUT->SetValue(0);
		}
		if (g_Options.disablepro == false) {
			static auto GETITOUT = g_CVar->FindVar("mat_postprocess_enable");
			GETITOUT->SetValue(1);
		}

		//Far Esp
		static auto faresp = g_CVar->FindVar("r_drawallrenderables");
		faresp->SetValue(g_Options.faresp);

		//Fog
		static auto fog_override = g_CVar->FindVar(crypt_str("fog_override")); //-V807
		if (!g_Options.fogchanga)
		{
			if (fog_override->GetBool())
				fog_override->SetValue(FALSE);
		}
		if (!fog_override->GetBool())
			fog_override->SetValue(TRUE);
		static auto fog_start = g_CVar->FindVar(crypt_str("fog_start"));
		if (fog_start->GetInt())
			fog_start->SetValue(0);
		static auto fog_end = g_CVar->FindVar(crypt_str("fog_end"));
		if (fog_end->GetInt() != g_Options.fogfardamn)
			fog_end->SetValue(g_Options.fogfardamn);
		static auto fog_maxdensity = g_CVar->FindVar(crypt_str("fog_maxdensity"));
		if (fog_maxdensity->GetFloat() != (float)g_Options.fogdens * 0.01f) //-V550
			fog_maxdensity->SetValue((float)g_Options.fogdens * 0.01f);
		char buffer_color[12];
		sprintf_s(buffer_color, 12, "%i %i %i", g_Options.fogcoluh.r(), g_Options.fogcoluh.g(), g_Options.fogcoluh.b());
		static auto fog_color = g_CVar->FindVar(crypt_str("fog_color"));
		if (strcmp(fog_color->GetString(), buffer_color)) //-V526
			fog_color->SetValue(buffer_color);
		//World Glow
		static auto mat_ambient_light_r = g_CVar->FindVar("mat_ambient_light_r");
		static auto mat_ambient_light_g = g_CVar->FindVar("mat_ambient_light_g");
		static auto mat_ambient_light_b = g_CVar->FindVar("mat_ambient_light_b");
		mat_ambient_light_r->SetValue(g_Options.worldglowr);
		mat_ambient_light_g->SetValue(g_Options.worldglowg);
		mat_ambient_light_b->SetValue(g_Options.worldglowb);
		DWORD colorwrite, srgbwrite;
		IDirect3DVertexDeclaration9* vert_dec = nullptr;
		IDirect3DVertexShader9* vert_shader = nullptr;
		DWORD dwOld_D3DRS_COLORWRITEENABLE = NULL;


		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();


		auto esp_drawlist = Render::Get().RenderScene();

		Menu::Get().Render();
		Menu::Get().SpecList();


		ImGui::Render(esp_drawlist);

		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

		return oEndScene(pDevice);
		IDirect3DStateBlock9* pixel_state = NULL; IDirect3DVertexDeclaration9* vertDec; IDirect3DVertexShader9* vertShader;


		void SaveState(IDirect3DDevice9 * device);
		{
			D3DVIEWPORT9 d3d_viewport;
			pDevice->GetViewport(&d3d_viewport);

			pDevice->CreateStateBlock(D3DSBT_ALL, &pixel_state);
			pixel_state->Capture();

			pDevice->SetVertexShader(nullptr);
			pDevice->SetPixelShader(nullptr);
			pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

			pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
			pDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
			pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
			pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

			pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
			pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
			pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
			pDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);

			pDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
			pDevice->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);

			pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
			pDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
			pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			pDevice->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_INVDESTALPHA);
			pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			pDevice->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);

			pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, FALSE);
			pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
		}
	}
	//--------------------------------------------------------------------------------
	long __stdcall hkReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		static auto oReset = direct3d_hook.get_original<decltype(&hkReset)>(index::Reset);

		Menu::Get().OnDeviceLost();

		auto hr = oReset(device, pPresentationParameters);

		if (hr >= 0)
			Menu::Get().OnDeviceReset();

		return hr;
	}
	//--------------------------------------------------------------------------------
	using CreateMove_t = bool(__thiscall*)(IClientMode*, float, CUserCmd*);
	void __stdcall hkCreateMove(float smt, CUserCmd* cmd)
	{
		//cm_hook->get_func_address< CreateMove_t >(24); // TODO: use this and rewrite this
		static auto oCreateMove = cm_hook->get_func_address< CreateMove_t >(24);

		oCreateMove(g_ClientMode, smt, cmd);

		//auto cmd = g_Input->GetUserCmd(sequence_number);
		//auto verified = g_Input->GetVerifiedCmd(sequence_number);

		uintptr_t* fp;
		__asm mov fp, ebp;
		bool* bSendPacket = (bool*)(*fp - 0x1C);

		//Desync
		Misc::Get().ClanTag();

		if (!cmd || !cmd->command_number)
			return;
		QAngle oldAngle = cmd->viewangles;

		IGameEvent* event;

		if (g_Options.sky_changer)
		Fixed::Get().PerformNightmode();

		static auto fov_cs_debug = g_CVar->FindVar("fov_cs_debug");

		if (g_Options.fovscope) {
			if (g_LocalPlayer)
				fov_cs_debug->SetValue(g_Options.fovchangaaa);
			if (g_Options.fovchangaaa == 0)
				fov_cs_debug->SetValue("90");
		}
		if (!g_Options.fovscope) {
			if (g_LocalPlayer)
				fov_cs_debug->SetValue(g_Options.fovchangaaa);
			if (g_LocalPlayer && g_LocalPlayer->m_bIsScoped())
				fov_cs_debug->SetValue(0);
		}

			if (g_LocalPlayer && !g_LocalPlayer->m_bIsScoped() && g_Options.noscope) {
				static auto noscopa = g_CVar->FindVar("cl_drawhud");
				noscopa->SetValue(1);
			}
			if (g_LocalPlayer && g_LocalPlayer->m_bIsScoped() && g_Options.noscope) {
				static auto noscopa = g_CVar->FindVar("cl_drawhud");
				noscopa->SetValue(0);
			}
		if (!g_Options.noscope) {
			static auto noscopa = g_CVar->FindVar("cl_drawhud");
			noscopa->SetValue(1);
		}

		Globals::m_cmd = cmd;

		if (g_Options.misc_backtrack) {
			TimeWarp().Get().CreateMove(cmd);
		}

		if (g_Options.misc_chatspam) {
			Misc::Get().ChatSpama(cmd);
		}

		if (g_Options.slidewalk)
			Misc::Get().SilentWalk(cmd);

		QAngle LastAngle = QAngle(0, 0, 0);
		Math::Normalize3(cmd->viewangles);
		Math::ClampAngles(cmd->viewangles);
		LastAngle = cmd->viewangles;
		float oldForward;
		float oldSideMove;
		g_EngineClient->GetViewAngles(&oldAngle);
		oldForward = cmd->forwardmove;
		oldSideMove = cmd->sidemove;
		if (g_LocalPlayer->m_nMoveType() != MOVETYPE_LADDER)
			Misc::Get().MovementFix(oldAngle, cmd, oldForward, oldSideMove);
		Math::Normalize3(LastAngle);
		if (g_Options.nocool)
			cmd->buttons |= IN_BULLRUSH;
		Math::CorrectMovement(cmd, oldAngle, cmd->viewangles);
		//verified->m_cmd = *cmd;
		//verified->m_crc = cmd->GetChecksum();

		if (!cmd || !cmd->command_number)
			return;

		if (Menu::Get().IsVisible())
			cmd->buttons &= ~IN_ATTACK;

		if (g_Options.misc_bhop)
			BunnyHop::OnCreateMove(cmd);
		if (GetAsyncKeyState(g_Options.AutoStafe_key))
			BunnyHop::AutoStafe(cmd);

		movement::edgebug(cmd);

		if (g_EngineClient->IsInGame())
			Visuals::Get().ThirdPerson();

		CRageBot::Get().PrecacheShit();

		CPredictionSystem::Get().Start(cmd, g_LocalPlayer);
		{
			CRageBot::Get().CreateMove(cmd, *bSendPacket);
			Misc::Get().FakeLag(cmd, *bSendPacket);
			movement::jumpbug(cmd);
			Misc::Get().SlowWalk(cmd);
			CAntiAim::Get().CreateMove(cmd);
		}
		CPredictionSystem::Get().End(g_LocalPlayer);

		Misc::Get().Sexdick(cmd, *bSendPacket);

		Math::Normalize3(cmd->viewangles);
		Math::ClampAngles(cmd->viewangles);
		LastAngle = cmd->viewangles;
		g_EngineClient->GetViewAngles(&oldAngle);
		oldForward = cmd->forwardmove;
		oldSideMove = cmd->sidemove;
		if (g_LocalPlayer->m_nMoveType() != MOVETYPE_LADDER)
			Misc::Get().MovementFix(oldAngle, cmd, oldForward, oldSideMove);

		static auto prediction = new PredictionSystem();
		auto flags = g_LocalPlayer->m_fFlags();
		float eb = floor(g_LocalPlayer->m_vecVelocity().z);
		prediction->StartPrediction(cmd);
		g_Legitbot->Run(cmd);
		Visuals::Get().ebdetection(eb, flags);
		prediction->EndPrediction();
		if (g_Options.edgejump.enabled && GetAsyncKeyState(g_Options.edgejump.hotkey))
		{
			if (!(g_LocalPlayer->m_fFlags() & FL_ONGROUND) && (flags & FL_ONGROUND))
				cmd->buttons |= IN_JUMP;

		}

		if (!(g_LocalPlayer->m_fFlags() & FL_ONGROUND) && g_Options.ducknair && !(cmd->buttons |= IN_DUCK))
			cmd->buttons |= IN_DUCK;

		// faggot code sopmk e
		if (int(Globals::real_angle * 1000))
		{
			if (!bSendPacket)
				Globals::real_angle = cmd->viewangles.yaw;
			else
				Globals::fake_angle = cmd->viewangles.yaw;
		}
		else // reset. fuck rocco (consent)
			Globals::real_angle = cmd->viewangles.yaw;

		// https://github.com/spirthack/CSGOSimple/issues/69
		if (g_Options.sniper_xhair && !g_LocalPlayer->m_bIsScoped())
			g_CVar->FindVar("weapon_debug_spread_show")->SetValue(3);
		if (g_Options.sniper_xhair && g_LocalPlayer->m_bIsScoped())
			g_CVar->FindVar("weapon_debug_spread_show")->SetValue(0);
		if (!g_Options.sniper_xhair)
			g_CVar->FindVar("weapon_debug_spread_show")->SetValue(0);

		if (g_Options.blockbot && GetAsyncKeyState(g_Options.bbkey)) {
			g_BlockBot->cmove(cmd);
			g_BlockBot->draw();
		}
	}
	//--------------------------------------------------------------------------------
	__declspec(naked) void __fastcall hkCreateMove_Proxy(void* _this, int, int sequence_number, float input_sample_frametime, bool active)
	{
		__asm
		{
			push ebp
			mov  ebp, esp
			push ebx; not sure if we need this
			push esp
			push dword ptr[active]
			push dword ptr[input_sample_frametime]
			push dword ptr[sequence_number]
			call Hooks::hkCreateMove
			pop  ebx
			pop  ebp
			retn 0Ch
		}
	}
	//--------------------------------------------------------------------------------
	void __fastcall hkPaintTraverse(void* _this, int edx, vgui::VPANEL panel, bool forceRepaint, bool allowForce)
	{
		static auto panelId = vgui::VPANEL{ 0 };
		static auto oPaintTraverse = vguipanel_hook.get_original<decltype(&hkPaintTraverse)>(index::PaintTraverse);

		oPaintTraverse(g_VGuiPanel, edx, panel, forceRepaint, allowForce);

		if (g_Options.colormodulation)
			Misc::Get().NightmodeFix();

		if (!panelId) {
			const auto panelName = g_VGuiPanel->GetName(panel);
			if (!strcmp(panelName, "FocusOverlayPanel")) {
				panelId = panel;
			}
		}
		else if (panelId == panel)
		{
			//Ignore 50% cuz it called very often
			static bool bSkip = false;
			bSkip = !bSkip;

			if (bSkip)
				return;

			Render::Get().BeginScene();
		}
	}
	//--------------------------------------------------------------------------------
	void __fastcall hkEmitSound1(void* _this, int edx, IRecipientFilter& filter, int iEntIndex, int iChannel, const char* pSoundEntry, unsigned int nSoundEntryHash, const char* pSample, float flVolume, int nSeed, float flAttenuation, int iFlags, int iPitch, const Vector* pOrigin, const Vector* pDirection, void* pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity, int unk) {
		static auto ofunc = sound_hook.get_original<decltype(&hkEmitSound1)>(index::EmitSound1);


		if (g_Options.autoaccept && !strcmp(pSoundEntry, "UIPanorama.popup_accept_match_beep")) {
			static auto fnAccept = reinterpret_cast<bool(__stdcall*)(const char*)>(Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 83 E4 F8 8B 4D 08 BA ? ? ? ? E8 ? ? ? ? 85 C0 75 12"));

			if (fnAccept) {

				fnAccept("");

				//This will flash the CSGO window on the taskbar
				//so we know a game was found (you cant hear the beep sometimes cause it auto-accepts too fast)
				FLASHWINFO fi;
				fi.cbSize = sizeof(FLASHWINFO);
				fi.hwnd = InputSys::Get().GetMainWindow();
				fi.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
				fi.uCount = 0;
				fi.dwTimeout = 0;
				FlashWindowEx(&fi);
			}
		}

		ofunc(g_EngineSound, edx, filter, iEntIndex, iChannel, pSoundEntry, nSoundEntryHash, pSample, flVolume, nSeed, flAttenuation, iFlags, iPitch, pOrigin, pDirection, pUtlVecOrigins, bUpdatePositions, soundtime, speakerentity, unk);

	}
	//--------------------------------------------------------------------------------
	int __fastcall hkDoPostScreenEffects(void* _this, int edx, int a1)
	{
		static auto oDoPostScreenEffects = clientmode_hook.get_original<decltype(&hkDoPostScreenEffects)>(index::DoPostScreenSpaceEffects);

		return oDoPostScreenEffects(g_ClientMode, edx, a1);
	}
	//--------------------------------------------------------------------------------
	void __fastcall hkFrameStageNotify(void* _this, int edx, ClientFrameStage_t stage)
	{
		static auto ofunc = hlclient_hook.get_original<decltype(&hkFrameStageNotify)>(index::FrameStageNotify);

		if (g_EngineClient->IsInGame()) {
			nightmode::Get().clear_stored_materials();
			//Fix Thirdperson Angles
			if (Globals::m_cmd)
				Misc::Get().SetThirdpersonAngles(stage, Globals::m_cmd);

			QAngle* aim_punch = nullptr;
			QAngle* view_punch = nullptr;
			if (g_Options.fatassmf && stage == ClientFrameStage_t::FRAME_RENDER_START)
			{
				if (g_LocalPlayer && g_LocalPlayer->IsAlive())
				{
					aim_punch = &g_LocalPlayer->m_aimPunchAngle();
					view_punch = &g_LocalPlayer->m_viewPunchAngle();

					Globals::aim_punch_old = *aim_punch;
					Globals::view_punch_old = *view_punch;

					*aim_punch = QAngle(0, 0, 0);
					*view_punch = QAngle(0, 0, 0);
				}
			}

			//if (stage != ClientFrameStage_t::FRAME_RENDER_START && stage != ClientFrameStage_t::FRAME_RENDER_END)
			//	return;
			if (stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START)
				skins::on_frame_stage_notify(false);
			else if (stage == FRAME_NET_UPDATE_POSTDATAUPDATE_END)
				skins::on_frame_stage_notify(true);

			static int originalIdx = 0;

			if (!g_LocalPlayer) {
				originalIdx = 0;
				return;
			}

			constexpr auto getModel = [](int team) constexpr noexcept -> const char* {
				constexpr std::array models{
				"models/player/custom_player/legacy/ctm_diver_varianta.mdl", // Cmdr. Davida 'Goggles' Fernandez | SEAL Frogman
"models/player/custom_player/legacy/ctm_diver_variantb.mdl", // Cmdr. Frank 'Wet Sox' Baroud | SEAL Frogman
"models/player/custom_player/legacy/ctm_diver_variantc.mdl", // Lieutenant Rex Krikey | SEAL Frogman
"models/player/custom_player/legacy/ctm_fbi_varianth.mdl", // Michael Syfers | FBI Sniper
"models/player/custom_player/legacy/ctm_fbi_variantf.mdl", // Operator | FBI SWAT
"models/player/custom_player/legacy/ctm_fbi_variantb.mdl", // Special Agent Ava | FBI
"models/player/custom_player/legacy/ctm_fbi_variantg.mdl", // Markus Delrow | FBI HRT
"models/player/custom_player/legacy/ctm_gendarmerie_varianta.mdl", // Sous-Lieutenant Medic | Gendarmerie Nationale
"models/player/custom_player/legacy/ctm_gendarmerie_variantb.mdl", // Chem-Haz Capitaine | Gendarmerie Nationale
"models/player/custom_player/legacy/ctm_gendarmerie_variantc.mdl", // Chef d'Escadron Rouchard | Gendarmerie Nationale
"models/player/custom_player/legacy/ctm_gendarmerie_variantd.mdl", // Aspirant | Gendarmerie Nationale
"models/player/custom_player/legacy/ctm_gendarmerie_variante.mdl", // Officer Jacques Beltram | Gendarmerie Nationale
"models/player/custom_player/legacy/ctm_sas_variantg.mdl", // D Squadron Officer | NZSAS
"models/player/custom_player/legacy/ctm_sas_variantf.mdl", // B Squadron Officer | SAS
"models/player/custom_player/legacy/ctm_st6_variante.mdl", // Seal Team 6 Soldier | NSWC SEAL
"models/player/custom_player/legacy/ctm_st6_variantg.mdl", // Buckshot | NSWC SEAL
"models/player/custom_player/legacy/ctm_st6_varianti.mdl", // Lt. Commander Ricksaw | NSWC SEAL
"models/player/custom_player/legacy/ctm_st6_variantj.mdl", // 'Blueberries' Buckshot | NSWC SEAL
"models/player/custom_player/legacy/ctm_st6_variantk.mdl", // 3rd Commando Company | KSK
"models/player/custom_player/legacy/ctm_st6_variantl.mdl", // 'Two Times' McCoy | TACP Cavalry
"models/player/custom_player/legacy/ctm_st6_variantm.mdl", // 'Two Times' McCoy | USAF TACP
"models/player/custom_player/legacy/ctm_st6_variantn.mdl", // Primeiro Tenente | Brazilian 1st Battalion
"models/player/custom_player/legacy/ctm_swat_variante.mdl", // Cmdr. Mae 'Dead Cold' Jamison | SWAT
"models/player/custom_player/legacy/ctm_swat_variantf.mdl", // 1st Lieutenant Farlow | SWAT
"models/player/custom_player/legacy/ctm_swat_variantg.mdl", // John 'Van Healen' Kask | SWAT
"models/player/custom_player/legacy/ctm_swat_varianth.mdl", // Bio-Haz Specialist | SWAT
"models/player/custom_player/legacy/ctm_swat_varianti.mdl", // Sergeant Bombson | SWAT
"models/player/custom_player/legacy/ctm_swat_variantj.mdl", // Chem-Haz Specialist | SWAT
"models/player/custom_player/legacy/tm_professional_varj.mdl", // Getaway Sally | The Professionals
"models/player/custom_player/legacy/tm_professional_vari.mdl", // Number K | The Professionals
"models/player/custom_player/legacy/tm_professional_varh.mdl", // Little Kev | The Professionals
"models/player/custom_player/legacy/tm_professional_varg.mdl", // Safecracker Voltzmann | The Professionals
"models/player/custom_player/legacy/tm_professional_varf5.mdl", // Bloody Darryl The Strapped | The Professionals
"models/player/custom_player/legacy/tm_professional_varf4.mdl", // Sir Bloody Loudmouth Darryl | The Professionals
"models/player/custom_player/legacy/tm_professional_varf3.mdl", // Sir Bloody Darryl Royale | The Professionals
"models/player/custom_player/legacy/tm_professional_varf2.mdl", // Sir Bloody Skullhead Darryl | The Professionals
"models/player/custom_player/legacy/tm_professional_varf1.mdl", // Sir Bloody Silent Darryl | The Professionals
"models/player/custom_player/legacy/tm_professional_varf.mdl", // Sir Bloody Miami Darryl | The Professionals
"models/player/custom_player/legacy/tm_phoenix_varianti.mdl", // Street Soldier | Phoenix
"models/player/custom_player/legacy/tm_phoenix_varianth.mdl", // Soldier | Phoenix
"models/player/custom_player/legacy/tm_phoenix_variantg.mdl", // Slingshot | Phoenix
"models/player/custom_player/legacy/tm_phoenix_variantf.mdl", // Enforcer | Phoenix
"models/player/custom_player/legacy/tm_leet_variantj.mdl", // Mr. Muhlik | Elite Crew
"models/player/custom_player/legacy/tm_leet_varianti.mdl", // Prof. Shahmat | Elite Crew
"models/player/custom_player/legacy/tm_leet_varianth.mdl", // Osiris | Elite Crew
"models/player/custom_player/legacy/tm_leet_variantg.mdl", // Ground Rebel | Elite Crew
"models/player/custom_player/legacy/tm_leet_variantf.mdl", // The Elite Mr. Muhlik | Elite Crew
"models/player/custom_player/legacy/tm_jungle_raider_variantf2.mdl", // Trapper | Guerrilla Warfare
"models/player/custom_player/legacy/tm_jungle_raider_variantf.mdl", // Trapper Aggressor | Guerrilla Warfare
"models/player/custom_player/legacy/tm_jungle_raider_variante.mdl", // Vypa Sista of the Revolution | Guerrilla Warfare
"models/player/custom_player/legacy/tm_jungle_raider_variantd.mdl", // Col. Mangos Dabisi | Guerrilla Warfare
"models/player/custom_player/legacy/tm_jungle_raider_variantb2.mdl", // 'Medium Rare' Crasswater | Guerrilla Warfare
"models/player/custom_player/legacy/tm_jungle_raider_variantb.mdl", // Crasswater The Forgotten | Guerrilla Warfare
"models/player/custom_player/legacy/tm_jungle_raider_varianta.mdl", // Elite Trapper Solman | Guerrilla Warfare
"models/player/custom_player/legacy/tm_balkan_varianth.mdl", // 'The Doctor' Romanov | Sabre
"models/player/custom_player/legacy/tm_balkan_variantj.mdl", // Blackwolf | Sabre
"models/player/custom_player/legacy/tm_balkan_varianti.mdl", // Maximus | Sabre
"models/player/custom_player/legacy/tm_balkan_variantf.mdl", // Dragomir | Sabre
"models/player/custom_player/legacy/tm_balkan_variantg.mdl", // Rezan The Ready | Sabre
"models/player/custom_player/legacy/tm_balkan_variantk.mdl", // Rezan the Redshirt | Sabre
"models/player/custom_player/legacy/tm_balkan_variantl.mdl", // Dragomir | Sabre Footsoldier

				};

				switch (team) {
				case 2: return static_cast<std::size_t>(g_Options.playerModelT - 1) < models.size() ? models[g_Options.playerModelT - 1] : nullptr;
				case 3: return static_cast<std::size_t>(g_Options.playerModelCT - 1) < models.size() ? models[g_Options.playerModelCT - 1] : nullptr;
				default: return nullptr;
				}
			};

			if (const auto model = getModel(g_LocalPlayer->m_iTeamNum())) {
				if (stage == FRAME_RENDER_START)
					originalIdx = g_LocalPlayer->m_nModelIndex();

				const auto idx = stage == FRAME_RENDER_END && originalIdx ? originalIdx : g_MdlInfo->GetModelIndex(model);

				g_LocalPlayer->setModelIndex(idx);

				if (const auto ragdoll = g_LocalPlayer->get_entity_from_handle(g_LocalPlayer->m_hRagdoll()))
					ragdoll->setModelIndex(idx);
			}

			if (stage == FRAME_RENDER_END)
			{
				if (g_Options.colormodulation) {
				static auto r_drawspecificstaticprop = g_CVar->FindVar(crypt_str("r_drawspecificstaticprop")); //-V807
					if (g_Options.changemats)
					{
						if (r_drawspecificstaticprop->GetBool())
							r_drawspecificstaticprop->SetValue(g_Options.changemats);

						if (g_Options.colormodulate)
							nightmode::Get().apply();
						else
							nightmode::Get().remove();

						g_Options.changemats = false;
					}
				}
			}
		}
		ofunc(g_CHLClient, edx, stage);
	}
	//--------------------------------------------------------------------------------
	void __fastcall hkOverrideView(void* _this, int edx, CViewSetup* vsView)
	{
		static auto ofunc = clientmode_hook.get_original<decltype(&hkOverrideView)>(index::OverrideView);

		ofunc(g_ClientMode, edx, vsView);
	}
	//--------------------------------------------------------------------------------
	void __fastcall hkLockCursor(void* _this)
	{
		static auto ofunc = vguisurf_hook.get_original<decltype(&hkLockCursor)>(index::LockCursor);

		if (Menu::Get().IsVisible()) {
			g_VGuiSurface->UnlockCursor();
			g_InputSystem->ResetInputState();
			return;
		}
		ofunc(g_VGuiSurface);

	}
	//--------------------------------------------------------------------------------
	void __fastcall hkDrawModelExecute(void* _this, int edx, IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld)
	{
		static auto ofunc = mdlrender_hook.get_original<decltype(&hkDrawModelExecute)>(index::DrawModelExecute);

		if (g_MdlRender->IsForcedMaterialOverride() &&
			!strstr(pInfo.pModel->szName, "arms") &&
			!strstr(pInfo.pModel->szName, "weapons/v_")) {

		}


		ofunc(_this, edx, ctx, state, pInfo, pCustomBoneToWorld);

		g_MdlRender->ForcedMaterialOverride(nullptr);
	}

	bool __fastcall hkSvCheatsGetBool(PVOID pConVar, void* edx)
	{
		static auto dwCAM_Think = Utils::PatternScan(GetModuleHandleW(L"client.dll"), "85 C0 75 30 38 86");
		static auto ofunc = sv_cheats.get_original<bool(__thiscall*)(PVOID)>(13);
		if (!ofunc)
			return false;

		if (reinterpret_cast<DWORD>(_ReturnAddress()) == reinterpret_cast<DWORD>(dwCAM_Think))
			return true;
		return ofunc(pConVar);
	}

	void RecvProxy(const CRecvProxyData* pData, void* entity, void* output)
	{
		static auto ofunc = sequence_hook->get_original_function();

		if (g_LocalPlayer && g_LocalPlayer->IsAlive()) {
			const auto proxy_data = const_cast<CRecvProxyData*>(pData);
			const auto view_model = static_cast<C_BaseViewModel*>(entity);

			if (view_model && view_model->m_hOwner() && view_model->m_hOwner().IsValid()) {
				const auto owner = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntityFromHandle(view_model->m_hOwner()));
				if (owner == g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer())) {
					const auto view_model_weapon_handle = view_model->m_hWeapon();
					if (view_model_weapon_handle.IsValid()) {
						const auto view_model_weapon = static_cast<C_BaseAttributableItem*>(g_EntityList->GetClientEntityFromHandle(view_model_weapon_handle));
						if (view_model_weapon) {
							if (k_weapon_info.count(view_model_weapon->m_Item().m_iItemDefinitionIndex())) {
								auto original_sequence = proxy_data->m_Value.m_Int;
								const auto override_model = k_weapon_info.at(view_model_weapon->m_Item().m_iItemDefinitionIndex()).model;
								proxy_data->m_Value.m_Int = skins::GetNewAnimation(override_model, proxy_data->m_Value.m_Int);
							}
						}
					}
				}
			}

		}

		ofunc(pData, entity, output);
	}

	void __fastcall hkDrawModelExecute2(void* _this, int, void* pResults, DrawModelInfo_t* pInfo, matrix3x4_t* pBoneToWorld, float* flpFlexWeights, float* flpFlexDelayedWeights, Vector& vrModelOrigin, int32_t iFlags)
	{
		static auto ofunc = stdrender_hook.get_original<decltype(&hkDrawModelExecute2)>(index::DrawModelExecute2);



		if (g_StudioRender->IsForcedMaterialOverride())
			return ofunc(g_StudioRender, 0, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);

		static auto flash = g_MatSystem->FindMaterial("effects/flashbang", TEXTURE_GROUP_CLIENT_EFFECTS);
		static auto flash_white = g_MatSystem->FindMaterial("effects/flashbang_white", TEXTURE_GROUP_CLIENT_EFFECTS);
		flash->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, g_Options.no_flash);
		flash_white->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, g_Options.no_flash);
		std::vector<const char*> vistasmoke_mats =

		{
		"effects/overlaysmoke",
		"particle/beam_smoke_01",
		"particle/particle_smokegrenade",
		"particle/particle_smokegrenade1",
		"particle/particle_smokegrenade2",
		"particle/particle_smokegrenade3",
		"particle/particle_smokegrenade_sc",
		"particle/smoke1/smoke1",
		"particle/smoke1/smoke1_ash",
		"particle/smoke1/smoke1_nearcull",
		"particle/smoke1/smoke1_nearcull2",
		"particle/smoke1/smoke1_snow",
		"particle/smokesprites_0001",
		"particle/smokestack",
		"particle/vistasmokev1/vistasmokev1",
		"particle/vistasmokev1/vistasmokev1_emods",
		"particle/vistasmokev1/vistasmokev1_emods_impactdust",
		"particle/vistasmokev1/vistasmokev1_fire",
		"particle/vistasmokev1/vistasmokev1_nearcull",
		"particle/vistasmokev1/vistasmokev1_nearcull_fog",
		"particle/vistasmokev1/vistasmokev1_nearcull_nodepth",
		"particle/vistasmokev1/vistasmokev1_smokegrenade",
		"particle/vistasmokev1/vistasmokev4_emods_nocull",
		"particle/vistasmokev1/vistasmokev4_nearcull",
		"particle/vistasmokev1/vistasmokev4_nocull"
		};

		for (auto mat_s : vistasmoke_mats)
		{
			IMaterial* mat = g_MatSystem->FindMaterial(mat_s, TEXTURE_GROUP_OTHER);
			mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, g_Options.no_smoke);
		}

		Chams::Get().OnDrawModelExecute(pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);

		ofunc(g_StudioRender, 0, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);

		g_StudioRender->ForcedMaterialOverride(nullptr);
	}

	void __fastcall Hooks::RenderSmokeOverlay(void* _this, int, const bool unk)
	{
		static auto ofunc = viewrender_hook.get_original<decltype(&RenderSmokeOverlay)>(index::RenderSmokeOverlay);

		if (!g_Options.no_smoke)
			ofunc(g_ViewRender, 0, unk);
	}
}