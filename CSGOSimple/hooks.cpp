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
#include "features/Misc.hpp"
#include "backtrack.h"
#include "BetaAA.h"
#include "OldPrediction.h"
#include "Globals.h"
#include "movement.h"
#include "xor.h"
#include "./features/skins.h"
#ifdef ENABLE_XOR
#define XorStr _xor_ 
#else
#define XorStr
#endif
#pragma intrinsic(_ReturnAddress)  
#include "blockbot.hpp"
#include "resolver.h"
#include "FixSkyboxes.h"
#include "ColorMod.h"
#include "DirectStrafe.hpp"
#include "features/glow.hpp"
#include "detours.h"
#include "sexynutssexyfuckdickshitnigga.h"
#include "Rollangles.h"

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
		
		static auto cl_move = (DWORD)(Utils::PatternScan(GetModuleHandleA(XorStr("engine.dll")), XorStr("55 8B EC 81 EC ? ? ? ? 53 56 8A F9")));
		original_clmove = (DWORD)DetourFunction((PBYTE)cl_move, (PBYTE)hkClMove);

		/*using Hooked_FindOrAddFileNamefn = void* (__thiscall*)(IFileSystem*, char const*);
		FindOrAddFileName_hook.setup(g_FileSystem);*/

		gameevents_hook.setup(g_GameEvents);
		gameevents_hook.hook_index(index::FireEvent, hkFireEvent);
		direct3d_hook.hook_index(index::EndScene, hkEndScene);
		direct3d_hook.hook_index(index::Reset, hkReset);
		hlclient_hook.hook_index(index::FrameStageNotify, hkFrameStageNotify);
		hlclient_hook.hook_index(index::CreateMove, hkCreateMove_Proxy); // uncomment when fixed lmao!
		//cm_hook->hook_function(reinterpret_cast<uintptr_t>(hkCreateMove), 24);
		vguipanel_hook.hook_index(index::PaintTraverse, hkPaintTraverse);
		sound_hook.hook_index(index::EmitSound1, hkEmitSound1);
		vguisurf_hook.hook_index(index::LockCursor, hkLockCursor);
		mdlrender_hook.hook_index(index::DrawModelExecute, hkDrawModelExecute);
		clientmode_hook.hook_index(index::DoPostScreenSpaceEffects, hkDoPostScreenEffects);
		cm_hook->hook_function(reinterpret_cast<uintptr_t>(hkOverrideView), 18);
		sv_cheats.hook_index(index::SvCheatsGetBool, hkSvCheatsGetBool);
		stdrender_hook.hook_index(index::DrawModelExecute2, hkDrawModelExecute2);
		//hk_netchannel.hook_index(46, hkSendDatagram);
		viewrender_hook.hook_index(index::RenderSmokeOverlay, RenderSmokeOverlay);
		//netgraphtext_hook.hook_index(index::NetGraph, hkNetGraph);

		/*auto g_pFileSystem = **reinterpret_cast<void***>(Utils::PatternScan(x("engine.dll"), x("8B 0D ? ? ? ? 8D 95 ? ? ? ? 6A 00 C6")) + 0x2);
		if (g_pFileSystem)
		{
			filesystem_hook->set_base(g_pFileSystem);
			filesystem_hook->hook_index(128, hkLooseFileAllowed);
		}*/

		/*bool __fastcall hkLooseFileAllowed(void* ecx, void* edx)
		{
			return true;
		}*/
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
		hk_netchannel.unhook_all();
		sv_cheats.unhook_all();

		sequence_hook->~recv_prop_hook();

		//Glow::Get().Shutdown();

	}

	bool __fastcall hkNetGraph(void* ecx, void* edx)
	{
		static auto ofunc = netgraphtext_hook.get_original<netgraphtextFn>(index::NetGraph);

		void* returna = _ReturnAddress();
		static auto v1 = (DWORD)Utils::PatternScan(GetModuleHandleA(XorStr("client.dll")), XorStr("85 C0 0F 84 ? ? ? ? A1 ? ? ? ? 0F 57 C0 F3 0F 10 48"));
		static auto cnetgraph = **(uintptr_t**)(Utils::PatternScan(GetModuleHandleA(XorStr("client.dll")), XorStr("89 1D ? ? ? ? 8B C3 5B 8B E5 5D C2 04")) + 2);
		if (reinterpret_cast<DWORD>(returna) == (v1))
			*(float*)(cnetgraph + 0x131B8) = (0 / 1000.0f); //fps

		return ofunc(ecx);
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

		return oFireEvent(g_GameEvents, pEvent);
	}

	/*void __stdcall hkSuppressLists(int a2, bool a3) {
		static auto ofunc = partition_hook.GetOriginal< SuppressLists >();

		static auto OnRenderStart_Return = Utils::PatternScan(GetModuleHandleA("client.dll"), "FF 50 40 8B 1D ? ? ? ?") + 0x3;
		static auto FrameNetUpdateEnd_Return = Utils::PatternScan(GetModuleHandleA("client.dll"), "5F 5E 5D C2 04 00 83 3D ? ? ? ? ?");

		if (g_LocalPlayer && g_LocalPlayer->IsAlive()) {
			if (_ReturnAddress() == OnRenderStart_Return) {

			}
			else if (_ReturnAddress() == FrameNetUpdateEnd_Return) {
				skins::on_frame_stage_notify(true);
			}
		}

		ofunc(g_SpatialPartition, a2, a3);
	}

	void __fastcall hkReadPacketEntities(void* pClientState, void* pEDX, void* pEntInfo) {
		static auto original = clientstate_hook.GetOriginal<ReadPacketEntities>();

		original(pClientState, pEntInfo);
		skins::on_frame_stage_notify(false);
	}*/

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

		DWORD colorwrite, srgbwrite;
		IDirect3DVertexDeclaration9* vert_dec = nullptr;
		IDirect3DVertexShader9* vert_shader = nullptr;
		DWORD dwOld_D3DRS_COLORWRITEENABLE = NULL;
		pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &colorwrite);
		pDevice->GetRenderState(D3DRS_SRGBWRITEENABLE, &srgbwrite);

		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
		//removes the source engine color correction
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);

		pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &dwOld_D3DRS_COLORWRITEENABLE);
		pDevice->GetVertexDeclaration(&vert_dec);
		pDevice->GetVertexShader(&vert_shader);
		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_SRGBTEXTURE, NULL);


		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();


		auto esp_drawlist = Render::Get().RenderScene();

		Menu::Get().Render();
		Menu::Get().SpecList();

		ImGui::Render(esp_drawlist);

		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, srgbwrite);
		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, dwOld_D3DRS_COLORWRITEENABLE);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, true);
		pDevice->SetVertexDeclaration(vert_dec);
		pDevice->SetVertexShader(vert_shader);

		return oEndScene(pDevice);
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
	DWORD original_clmove;

	typedef void(__vectorcall* cl_move_t)(float, bool);
	void __vectorcall hkClMove(float accumulated_extra_samples, bool bFinalTick)
	{
		if (g_Options.fdkey != 0) {
			if (g_Options.fakeduck && GetAsyncKeyState(g_Options.fdkey))
			{
				Globals::tochargeamount = g_Options.aimbot.dtticks + 2;
				return (cl_move_t(original_clmove)(accumulated_extra_samples, bFinalTick));
			}
		}

		if (g_ClientState->iChokedCommands > 1)
		{
			Globals::tochargeamount = g_Options.aimbot.dtticks + 2;
			return (cl_move_t(original_clmove)(accumulated_extra_samples, bFinalTick));
		}
		
		if (Globals::startcharge && Globals::tocharge < Globals::tochargeamount)
		{
			Globals::tocharge++;
			Globals::ticks_allowed = Globals::tocharge;
			g_GlobalVars->interpolation_amount = 0.f;
			return;
		}

		(cl_move_t(original_clmove)(accumulated_extra_samples, bFinalTick));


		while (Globals::shift_ticks)
		{
			Globals::is_shifting = true;
			Globals::shift_ticks--;
			Globals::tocharge--;
			(cl_move_t(original_clmove)(accumulated_extra_samples, bFinalTick));
		}
		Globals::is_shifting = false;
	}

	CInput* m_input() {
		if (!g_Input)
			g_Input = *reinterpret_cast<CInput**>(Utils::PatternScan2(XorStr("client.dll"), "B9 ? ? ? ? F3 0F 11 04 24 FF 50 10") + 0x1);

		return g_Input;
	}

	//--------------------------------------------------------------------------------
	__declspec(naked) void __stdcall hkCreateMove_Proxy(int sequence_number, float input_sample_frametime, bool active) {
		__asm
		{
			push ebp
			mov  ebp, esp
			push ebx
			lea  ecx, [esp]
			push ecx
			push dword ptr[active]
			push dword ptr[input_sample_frametime]
			push dword ptr[sequence_number]
			call Hooks::hkCreateMove
			pop  ebx
			pop  ebp
			retn 0Ch
		}
	}
	using CreateMove_t = bool(__thiscall*)(IClientMode*, float, CUserCmd*);
	using CreateMove = void(__thiscall*)(IBaseClientDLL*, int, float, bool);
	void __stdcall hkCreateMove(int sequence_number, float input_sample_frametime, bool active, bool& bSendPacket)
	{
		auto oCreateMove = hlclient_hook.get_original<CreateMove>(index::CreateMove);
		oCreateMove(g_CHLClient, sequence_number, input_sample_frametime, active);

		if (Globals::is_shifting)
			return;

		auto cmd = m_input()->GetUserCmd(sequence_number);
		auto verified = m_input()->GetVerifiedUserCmd(sequence_number);

		// i fucking hate u rocco its so messy
		if (!cmd || !cmd->command_number)
			return;

		QAngle oldAngle = cmd->viewangles;

		IGameEvent* event;

		Globals::m_cmd = cmd;
		Globals::bSendPacket = bSendPacket;

		if (g_Options.misc_chatspam) {
			Misc::Get().ChatSpama(cmd);
		}

		Misc::Get().cl_move_dt(cmd);

		auto backup_ticks_allowed = Globals::ticks_allowed;

	//	if (Misc::Get().cl_move_dt(cmd))
			//Misc::Get().hide_shots(cmd, false);
		//else
	//	{
		//	Globals::ticks_allowed = backup_ticks_allowed;
		//	Misc::Get().hide_shots(cmd, true);
		//}

		Misc::Get().SilentWalk(cmd);
		Misc::Get().Fakelag(cmd, bSendPacket);
		TimeWarp().Get().CreateMove(cmd);
		//TimeWarp().Get().ForwardTrack(cmd);

		if (Menu::Get().IsVisible())
			cmd->buttons &= ~IN_ATTACK;

		if (g_Options.misc_bhop)
			BunnyHop::OnCreateMove(cmd);
		if (g_Options.AutoStafe_key != 0)
		{
			if (GetAsyncKeyState(g_Options.AutoStafe_key)) {
				BunnyHop::AutoStafe(cmd);
				airstrafe::Get().create_move(cmd);
			}
		}
		if (g_Options.AutoStafe_key == 0)
		{
			BunnyHop::AutoStafe(cmd);
			airstrafe::Get().create_move(cmd);
		}


		if (g_LocalPlayer->m_nMoveType() != MOVETYPE_LADDER)
		{
			movement::jumpbug(cmd);
			movement::edgebug(cmd);
		}

		Misc::Get().SlowWalk(cmd);
		CAntiAim::Get().CreateMove(cmd, bSendPacket);

		if (g_Options.fakeduck)
			Misc::Get().FakeDuck(cmd, bSendPacket);

		Misc::Get().ClanTag();
		Misc::Get().Sexdick(cmd, bSendPacket);
		Misc::Get().UpdateLBY(cmd, bSendPacket);
		movement_recorder::Get().run(cmd);

		if (bSendPacket == false)
		{
			Globals::fake = cmd->viewangles;
		}
		if (bSendPacket == true)
		{
			Globals::real = cmd->viewangles;
		}

		static auto prediction = new PredictionSystem();
		auto flags = g_LocalPlayer->m_fFlags();
		prediction->StartPrediction(cmd);
		g_Legitbot->Run(cmd);
		prediction->EndPrediction();
		QAngle LastAngle = QAngle(0, 0, 0);
		Math::Normalize3(cmd->viewangles);
		Math::ClampAngles(cmd->viewangles);
		LastAngle = cmd->viewangles;
		float oldForward;
		float oldSideMove;
		g_EngineClient->GetViewAngles(&oldAngle);
		oldForward = cmd->forwardmove;
		oldSideMove = cmd->sidemove;
		Math::Normalize3(LastAngle);
		if (g_LocalPlayer->IsAlive() && g_Options.nocool && !Globals::valve && !Globals::stepping)
			cmd->buttons |= IN_BULLRUSH;

		//RollAngles::Get().Roll(cmd, bSendPacket);
		//Misc::Get().NoSpread(cmd);
		if (g_LocalPlayer->m_nMoveType() != MOVETYPE_LADDER)
		{
			Misc::Get().MovementFix(oldAngle, cmd, oldForward, oldSideMove);
		}

		Misc::Get().fuck(cmd);

		static bool wasinair = false;

		if (g_LocalPlayer->m_nMoveType() != MOVETYPE_LADDER)
		{
			if (g_Options.edgejump.enabled)
			{
				if (g_Options.edgejump.hotkey == 0) {
					if (!(g_LocalPlayer->m_fFlags() & FL_ONGROUND) && (flags & FL_ONGROUND))
						cmd->buttons |= IN_JUMP;
				}
				else {
					if (GetAsyncKeyState(g_Options.edgejump.hotkey) && g_Options.edgejump.hotkey != 0)
					{
						if (!(g_LocalPlayer->m_fFlags() & FL_ONGROUND) && (flags & FL_ONGROUND))
							cmd->buttons |= IN_JUMP;
					}
				}

			}

			if (g_Options.longjump)
			{
				bool dothatshitcuh = false;
				bool uno = false;
				bool two = false;

				if (GetAsyncKeyState(g_Options.ljkey) && g_Options.ljkey != 0)
				{
					dothatshitcuh = true;
				}
				if (dothatshitcuh)
				{

					if (!(g_LocalPlayer->m_fFlags() & FL_ONGROUND) && (flags & FL_ONGROUND))
					{
						cmd->buttons |= IN_DUCK;
						cmd->buttons |= IN_JUMP;
						uno = true;
					}

					if (!(g_LocalPlayer->m_fFlags() & FL_ONGROUND) && (!(flags & FL_ONGROUND)))
					{
						if (cmd->buttons & IN_DUCK)
							cmd->buttons |= ~IN_DUCK;

						if (cmd->buttons & IN_FORWARD)
							cmd->buttons |= ~IN_FORWARD;

						if (cmd->buttons & IN_BACK)
							cmd->buttons |= ~IN_BACK;

						two = true;
					}

					if (uno && two)
						dothatshitcuh = false;
				}
				else
				{
					uno = false;
					two = false;
				}
			}
		}

		if (g_LocalPlayer->m_nMoveType() != MOVETYPE_LADDER) {
			if (g_Options.diarkey == 0) {
				if (!(g_LocalPlayer->m_fFlags() & FL_ONGROUND) && g_Options.ducknair && !(cmd->buttons |= IN_DUCK))
					cmd->buttons |= IN_DUCK;
			}
			else {
				if (GetAsyncKeyState(g_Options.diarkey) && g_Options.diarkey != 0) {
					if (!(g_LocalPlayer->m_fFlags() & FL_ONGROUND) && g_Options.ducknair && !(cmd->buttons |= IN_DUCK))
						cmd->buttons |= IN_DUCK;
				}
			}
		}

		if (g_Options.rankreveal && cmd->buttons & IN_SCORE)
			g_CHLClient->DispatchUserMessage(CS_UM_ServerRankRevealAll, 0, 0, nullptr);

		Misc::Get().local_animfix(g_LocalPlayer, cmd, bSendPacket);
		Misc::Get().desyncchams(cmd, bSendPacket);

		// https://github.com/spirthack/CSGOSimple/issues/69
		if (g_Options.sniper_xhair && !g_LocalPlayer->m_bIsScoped())
			g_CVar->FindVar(XorStr("weapon_debug_spread_show"))->SetValue(3);
		if (g_Options.sniper_xhair && g_LocalPlayer->m_bIsScoped())
			g_CVar->FindVar(XorStr("weapon_debug_spread_show"))->SetValue(0);
		if (!g_Options.sniper_xhair)
			g_CVar->FindVar(XorStr("weapon_debug_spread_show"))->SetValue(0);

		if (g_Options.bbkey != 0) {
			if (g_Options.blockbot && GetAsyncKeyState(g_Options.bbkey)) {
				g_BlockBot->cmove(cmd);
				g_BlockBot->draw();
			}
		}

		Globals::bSendPacket = bSendPacket;

		verified->m_cmd = *cmd;
		verified->m_crc = cmd->GetChecksum();
	}
	//--------------------------------------------------------------------------------

	//--------------------------------------------------------------------------------
	void __fastcall hkPaintTraverse(void* _this, int edx, vgui::VPANEL panel, bool forceRepaint, bool allowForce)
	{
		static auto panelId = vgui::VPANEL{ 0 };
		static auto oPaintTraverse = vguipanel_hook.get_original<decltype(&hkPaintTraverse)>(index::PaintTraverse);

		if (g_Options.noscope)
			if (!strcmp("HudZoom", g_VGuiPanel->GetName(panel))) return;

		oPaintTraverse(g_VGuiPanel, edx, panel, forceRepaint, allowForce);

		if (!panelId) {
			const auto panelName = g_VGuiPanel->GetName(panel);
			if (!strcmp(panelName, XorStr("FocusOverlayPanel"))) {
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


		if (g_Options.autoaccept && !strcmp(pSoundEntry, XorStr("UIPanorama.popup_accept_match_beep"))) {
			static auto fnAccept = reinterpret_cast<bool(__stdcall*)(const char*)>(Utils::PatternScan(GetModuleHandleA(XorStr("client.dll")), XorStr("55 8B EC 83 E4 F8 8B 4D 08 BA ? ? ? ? E8 ? ? ? ? 85 C0 75 12")));

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

		if (!Globals::inprediction)
			ofunc(g_EngineSound, edx, filter, iEntIndex, iChannel, pSoundEntry, nSoundEntryHash, pSample, flVolume, nSeed, flAttenuation, iFlags, iPitch, pOrigin, pDirection, pUtlVecOrigins, bUpdatePositions, soundtime, speakerentity, unk);

	}
	//--------------------------------------------------------------------------------
	int __fastcall hkDoPostScreenEffects(void* _this, int edx, int a1)
	{
		static auto oDoPostScreenEffects = clientmode_hook.get_original<decltype(&hkDoPostScreenEffects)>(index::DoPostScreenSpaceEffects);

		//if (g_LocalPlayer && g_Options.glow_enabled)
			//Glow::Get().Run();

		return oDoPostScreenEffects(g_ClientMode, edx, a1);
	}

	/*int __fastcall hkSendDatagram(void* net_channel, void*, void* datagram) {

		if (!g_Options.aimbot.backtrack || !g_EngineClient->IsInGame())
			return oSendDatagram(net_channel, datagram);

		auto* channel = reinterpret_cast<INetChannel*>(net_channel);

		int instate = channel->m_nInReliableState;
		int insequencenr = channel->m_nInSequenceNr;

		TimeWarp::Get().AddLatency(channel, 500.f / 1000.f);

		int ret = oSendDatagram(channel, datagram);

		channel->m_nInReliableState = instate;
		channel->m_nInSequenceNr = insequencenr;

		return ret;
	}*/

	//--------------------------------------------------------------------------------
	void __fastcall hkFrameStageNotify(void* _this, int edx, ClientFrameStage_t stage)
	{
		static auto ofunc = hlclient_hook.get_original<decltype(&hkFrameStageNotify)>(index::FrameStageNotify);

		QAngle* aim_punch = nullptr;
		QAngle* view_punch = nullptr;
		if (stage == ClientFrameStage_t::FRAME_RENDER_START)
		{

			if (g_Options.aimbot.rageresolver && Globals::m_cmd)
				resolver::Get().Resolve(Globals::m_cmd);

			if (g_Options.edge_bug && (!g_EngineClient->IsInGame() || !g_LocalPlayer || !g_LocalPlayer->IsAlive()))
				g_InputSystem->ResetInputState();

			if (Globals::m_cmd)
			{
				Misc::Get().SetThirdpersonAngles(stage, Globals::m_cmd);
				//Misc::Get().local_animfix(g_LocalPlayer, Globals::m_cmd, Globals::bSendPacket);
			}

			if (g_EngineClient->IsInGame() && g_LocalPlayer && g_LocalPlayer->IsAlive())
			{

				if (g_Options.fatassmf)
				{
					aim_punch = &g_LocalPlayer->m_aimPunchAngle();
					view_punch = &g_LocalPlayer->m_viewPunchAngle();

					Globals::aim_punch_old = *aim_punch;
					Globals::view_punch_old = *view_punch;

					*aim_punch = QAngle(0, 0, 0);
					*view_punch = QAngle(0, 0, 0);
				}
			}
		}

		if (stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START)
		{
			skins::on_frame_stage_notify(false);
			//TimeWarp::Get().UpdateIncomingSequences();
		}
		else if (stage == FRAME_NET_UPDATE_POSTDATAUPDATE_END)
			skins::on_frame_stage_notify(true);

		static int originalIdx = 0;

		if (!g_LocalPlayer) {
			originalIdx = 0;
			return;
		}

		Chams::Get().RemovePlayerPatches();

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

			const auto idx = stage == ClientFrameStage_t::FRAME_RENDER_END && originalIdx ? originalIdx : g_MdlInfo->GetModelIndex(model);

			g_LocalPlayer->setModelIndex(idx);

			if (const auto ragdoll = g_LocalPlayer->get_entity_from_handle(g_LocalPlayer->m_hRagdoll()))
				ragdoll->setModelIndex(idx);
		}

		if (stage == ClientFrameStage_t::FRAME_RENDER_END)
		{

			static auto setthemcheat = g_CVar->FindVar(XorStr("sv_cheats"));
			if (setthemcheat->GetInt() != 1)
				setthemcheat->SetValue(1);

			auto server = g_EngineClient->GetNetChannelInfo()->GetAddress();
			bool localserver = false;
			if (!strcmp(server, (XorStr("loopback"))))
				localserver = true;
			else
				localserver = false;

			static auto rapingbabies = g_CVar->FindVar(XorStr("cl_extrapolate"));
			if (rapingbabies->GetInt() != 0 && Globals::valve == false || localserver == false)
				rapingbabies->SetValue(0);
			else
				rapingbabies->SetValue(1);

			static auto fullbright = g_CVar->FindVar(XorStr("mat_fullbright"));
			fullbright->SetValue(g_Options.fullbright);

			//Metallic Props
			static auto metallica = g_CVar->FindVar(XorStr("r_showenvcubemap"));
			metallica->m_fnChangeCallbacks.m_Size = 0;
			metallica->SetValue(g_Options.metallica);

			//Matrix Mode
			static auto matrix = g_CVar->FindVar(XorStr("mat_luxels"));
			matrix->m_fnChangeCallbacks.m_Size = 0;
			matrix->SetValue(g_Options.matrix);

			//Disable Blur
			static auto disableblur = g_CVar->FindVar(XorStr("@panorama_disable_blur"));
			disableblur->m_fnChangeCallbacks.m_Size = 0;
			disableblur->SetValue(g_Options.removeblur);

			//Aspect Ratio
			if (g_Options.aspectchange == true) {
				static auto r_aspectratio = g_CVar->FindVar(XorStr("r_aspectratio"));
				r_aspectratio->m_fnChangeCallbacks.m_Size = 0;
				r_aspectratio->SetValue(g_Options.aspectratio);
			}
			else if (g_Options.aspectchange == false) {
				static auto r_aspectratio = g_CVar->FindVar(XorStr("r_aspectratio"));
				r_aspectratio->m_fnChangeCallbacks.m_Size = 0;
				r_aspectratio->SetValue(XorStr("0"));
			}

			//Fake Ping
			if (g_Options.fakepingkey == 0) {
				if (g_Options.fakeping == true && g_EngineClient->IsInGame()) {
					static auto fakeping = g_CVar->FindVar(XorStr("net_fakelag"));
					fakeping->m_fnChangeCallbacks.m_Size = 0;
					fakeping->SetValue(g_Options.fakepingzzz);
				}
				if (g_Options.fakeping == false) {
					static auto fakeping = g_CVar->FindVar(XorStr("net_fakelag"));
					fakeping->m_fnChangeCallbacks.m_Size = 0;
					fakeping->SetValue(0);
				}
			}
			if (g_Options.fakepingkey != 0) {
				if (GetAsyncKeyState(g_Options.fakepingkey)) {
					if (g_Options.fakeping && g_EngineClient->IsInGame()) {
						static auto fakeping = g_CVar->FindVar(XorStr("net_fakelag"));
						fakeping->m_fnChangeCallbacks.m_Size = 0;
						fakeping->SetValue(g_Options.fakepingzzz);
					}
				}
				else {
					static auto fakeping = g_CVar->FindVar(XorStr("net_fakelag"));
					fakeping->m_fnChangeCallbacks.m_Size = 0;
					fakeping->SetValue(0);
				}
			}

			//Nade Prediction
			static auto pnade = g_CVar->FindVar(XorStr("cl_grenadepreview"));
			if (pnade->GetInt() != g_Options.pnade)
				pnade->SetValue(g_Options.pnade);

			bool sex = false;

			//Ragdoll Gravity
			if (g_EngineClient->IsInGame()) {
				if (g_Options.ragfloat == true) {
					static auto levito = g_CVar->FindVar(XorStr("cl_ragdoll_gravity"));
					levito->SetValue(XorStr("-1"));
					sex = true;
				}
				if (g_Options.ragfloat == false && sex == true) {
					static auto levito = g_CVar->FindVar(XorStr("cl_ragdoll_gravity"));
					levito->SetValue(XorStr("600"));
				}
			}
			else
			{
				sex = false;
			}

			//Recoil Crosshair
			static auto recoilcrosshaii = g_CVar->FindVar(XorStr("cl_crosshair_recoil"));
			recoilcrosshaii->SetValue(g_Options.rcross);

			static auto GETITOUT = g_CVar->FindVar(XorStr("mat_postprocess_enable"));
			GETITOUT->SetValue(!g_Options.disablepro);

			//Far Esp
			static auto faresp = g_CVar->FindVar(XorStr("r_drawallrenderables"));
			if (faresp->GetInt() != g_Options.faresp)
				faresp->SetValue(g_Options.faresp);

			//World Glow
			static auto mat_ambient_light_r = g_CVar->FindVar(XorStr("mat_ambient_light_r"));
			static auto mat_ambient_light_g = g_CVar->FindVar(XorStr("mat_ambient_light_g"));
			static auto mat_ambient_light_b = g_CVar->FindVar(XorStr("mat_ambient_light_b"));

			if (g_Options.color_worldglow.r() > 0 || g_Options.color_worldglow.g() > 0 || g_Options.color_worldglow.b() > 0)
			{
				if (mat_ambient_light_r->GetFloat() != g_Options.color_worldglow.r() / 255.f)
					mat_ambient_light_r->SetValue(g_Options.color_worldglow.r() / 255.f);
				if (mat_ambient_light_g->GetFloat() != g_Options.color_worldglow.g() / 255.f)
					mat_ambient_light_g->SetValue(g_Options.color_worldglow.g() / 255.f);
				if (mat_ambient_light_b->GetFloat() != g_Options.color_worldglow.b() / 255.f)
					mat_ambient_light_b->SetValue(g_Options.color_worldglow.b() / 255.f);
			}

			if (g_Options.amibence > FLT_MIN) {
				static auto r_modelAmbientMin = g_CVar->FindVar(XorStr("r_modelAmbientMin"));
				if (g_Options.amibence && r_modelAmbientMin->GetFloat() != g_Options.amibence * 0.05f) //-V550
					r_modelAmbientMin->SetValue(g_Options.amibence * 0.05f);
				else if ((g_Options.amibence < 2.f) && r_modelAmbientMin->GetFloat())
					r_modelAmbientMin->SetValue(XorStr("0"));
			}

			if (g_Options.viewmodel_fov)
			{
				auto viewFOV = (float)g_Options.viewmodel_fov + 68.0f;
				static auto viewFOVcvar = g_CVar->FindVar(XorStr("viewmodel_fov"));

				if (viewFOVcvar->GetFloat() != viewFOV) //-V550
				{
					*(float*)((DWORD)&viewFOVcvar->m_fnChangeCallbacks + 0xC) = 0.0f;
					viewFOVcvar->SetValue(viewFOV);
				}
			}

			if (g_Options.viewmodel_offset_x)
			{
				auto viewX = (float)g_Options.viewmodel_offset_x / 2.5f;
				static auto viewXcvar = g_CVar->FindVar(XorStr("viewmodel_offset_x")); //-V807

				if (viewXcvar->GetFloat() != viewX) //-V550
				{
					*(float*)((DWORD)&viewXcvar->m_fnChangeCallbacks + 0xC) = 0.0f;
					viewXcvar->SetValue(viewX);
				}
			}

			if (g_Options.viewmodel_offset_y)
			{
				auto viewY = (float)g_Options.viewmodel_offset_y / 2.0f;
				static auto viewYcvar = g_CVar->FindVar(XorStr("viewmodel_offset_y"));

				if (viewYcvar->GetFloat() != viewY) //-V550
				{
					*(float*)((DWORD)&viewYcvar->m_fnChangeCallbacks + 0xC) = 0.0f;
					viewYcvar->SetValue(viewY);
				}
			}

			if (g_Options.viewmodel_offset_z)
			{
				auto viewZ = (float)g_Options.viewmodel_offset_z / 2.0f;
				static auto viewZcvar = g_CVar->FindVar(XorStr("viewmodel_offset_z"));

				if (viewZcvar->GetFloat() != viewZ) //-V550
				{
					*(float*)((DWORD)&viewZcvar->m_fnChangeCallbacks + 0xC) = 0.0f;
					viewZcvar->SetValue(viewZ);
				}
			}

			//Sky Changer
			if (g_Options.sky_changer) {
				if (g_Options.skyshitsssss == 0) {
					static auto buttsex = g_CVar->FindVar(XorStr("sv_skyname"));
					buttsex->SetValue(XorStr("sky_csgo_night02"));
				}
				if (g_Options.skyshitsssss == 1) {
					static auto buttsex = g_CVar->FindVar(XorStr("sv_skyname"));
					buttsex->SetValue(XorStr("cs_baggage_skybox_"));
				}
				if (g_Options.skyshitsssss == 2) {
					static auto buttsex = g_CVar->FindVar(XorStr("sv_skyname"));
					buttsex->SetValue(XorStr("cs_tibet"));
				}
				if (g_Options.skyshitsssss == 3) {
					static auto buttsex = g_CVar->FindVar(XorStr("sv_skyname"));
					buttsex->SetValue(XorStr("vietnam"));
				}
				if (g_Options.skyshitsssss == 4) {
					static auto buttsex = g_CVar->FindVar(XorStr("sv_skyname"));
					buttsex->SetValue(XorStr("sky_lunacy"));
				}
				if (g_Options.skyshitsssss == 5) {
					static auto buttsex = g_CVar->FindVar(XorStr("sv_skyname"));
					buttsex->SetValue(XorStr("embassy"));
				}
				if (g_Options.skyshitsssss == 6) {
					static auto buttsex = g_CVar->FindVar(XorStr("sv_skyname"));
					buttsex->SetValue(XorStr("italy"));
				}
				if (g_Options.skyshitsssss == 7) {
					static auto buttsex = g_CVar->FindVar(XorStr("sv_skyname"));
					buttsex->SetValue(XorStr("jungle"));
				}
				if (g_Options.skyshitsssss == 8) {
					static auto buttsex = g_CVar->FindVar(XorStr("sv_skyname"));
					buttsex->SetValue(XorStr("office"));
				}
				if (g_Options.skyshitsssss == 9) {
					static auto buttsex = g_CVar->FindVar(XorStr("sv_skyname"));
					buttsex->SetValue(XorStr("sky_cs15_daylight02_hdr"));
				}
				if (g_Options.skyshitsssss == 10) {
					static auto buttsex = g_CVar->FindVar(XorStr("sv_skyname"));
					buttsex->SetValue(XorStr("sky_csgo_cloudy01"));
				}
				if (g_Options.skyshitsssss == 11) {
					static auto buttsex = g_CVar->FindVar(XorStr("sv_skyname"));
					buttsex->SetValue(XorStr("sky_dust"));
				}
			}

			//Fog
			static auto fog_override = g_CVar->FindVar(XorStr("fog_override")); //-V807
			if (!g_Options.fogoverride)
			{
				if (fog_override->GetBool())
					fog_override->SetValue(FALSE);
			}
			if (!fog_override->GetBool())
				fog_override->SetValue(TRUE);
			static auto fog_start = g_CVar->FindVar(XorStr("fog_start"));
			if (fog_start->GetInt())
				fog_start->SetValue(0);
			static auto fog_end = g_CVar->FindVar(XorStr("fog_end"));
			if (fog_end->GetInt() != g_Options.fogfardamn)
				fog_end->SetValue(g_Options.fogfardamn);
			static auto fog_maxdensity = g_CVar->FindVar(XorStr("fog_maxdensity"));
			if (fog_maxdensity->GetFloat() != (float)g_Options.fogdens * 0.01f) //-V550
				fog_maxdensity->SetValue((float)g_Options.fogdens * 0.01f);
			char buffer_color[12];
			sprintf_s(buffer_color, 12, "%i %i %i", g_Options.fogcoluh.r(), g_Options.fogcoluh.g(), g_Options.fogcoluh.b());
			static auto fog_color = g_CVar->FindVar(XorStr("fog_color"));
			if (strcmp(fog_color->GetString(), buffer_color)) //-V526
				fog_color->SetValue(buffer_color);

			Fixed::Get().PerformNightmode();
			
			if (g_Options.colormodulate)
				CNightmode::Get().PerformNightmode();
			else
				CNightmode::Get().GtfoNight();
		}

		static DWORD* death_notice = nullptr;

		if (g_Options.drugs) {
			if (g_LocalPlayer->IsAlive() && !Globals::stepping)
			{
				if (!death_notice)
					death_notice = FindHudElement <DWORD>(XorStr("CCSGO_HudDeathNotice"));

				if (death_notice)
				{
					auto local_death_notice = (float*)((uintptr_t)death_notice + 0x50);

					if (local_death_notice)
						*local_death_notice = g_Options.drugs ? FLT_MAX : 1.5f;;

					if (Globals::clear)
					{
						Globals::clear = false;

						using Fn = void(__thiscall*)(uintptr_t);
						static auto clear_notices = (Fn)Utils::PatternScan2(XorStr("client.dll"), XorStr("55 8B EC 83 EC 0C 53 56 8B 71 58"));

						clear_notices((uintptr_t)death_notice - 0x14);
					}
				}
			}
			else
				death_notice = 0;
		}

		ofunc(g_CHLClient, edx, stage);
	}

	using OverrideView_t = void(__stdcall*)(CViewSetup*);

	//--------------------------------------------------------------------------------
	void __stdcall hkOverrideView(CViewSetup* vsView)
	{
		static auto ofunc = cm_hook->get_func_address <OverrideView_t>(18);

		auto local = ((C_BasePlayer*)g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer()), true);

		if (!vsView)
			return ofunc(vsView);

		auto weapon = g_LocalPlayer->m_hActiveWeapon().Get();

		if (local)
		{
			Visuals::Get().ThirdPerson();

			static auto fov_cs_debug = g_CVar->FindVar(XorStr("fov_cs_debug"));
			float newfov = 90 + g_Options.fovchangaaa;

			if (!g_Options.viewmodelinscope) {

				if (g_Options.fovscope) {
					if (g_LocalPlayer)
						fov_cs_debug->SetValue(newfov);
					if (g_Options.fovchangaaa == 0)
						fov_cs_debug->SetValue(XorStr("90.01"));
				}
				if (!g_Options.fovscope) {
					if (g_LocalPlayer)
						fov_cs_debug->SetValue(newfov);
					if (g_LocalPlayer && g_LocalPlayer->m_bIsScoped())
						fov_cs_debug->SetValue(XorStr("0"));
				}
			}

			if (g_Options.viewmodelinscope) {
				if (g_Options.fovscope) {
					if (g_LocalPlayer)
					{
						vsView->fov += g_Options.fovchangaaa;
						fov_cs_debug->SetValue(XorStr("90"));
					}
				}
				if (!g_Options.fovscope) {
					fov_cs_debug->SetValue(XorStr("0"));
					if (g_LocalPlayer)
						vsView->fov += g_Options.fovchangaaa;
					if (g_LocalPlayer && g_LocalPlayer->m_bIsScoped())
						vsView->fov += 0;
				}
			}

			if (g_LocalPlayer && g_LocalPlayer->IsAlive()) {

				if (weapon && !weapon->IsBallistic())
				{
					auto viewmodel = (C_BaseEntity*)g_EntityList->GetClientEntityFromHandle(g_LocalPlayer->m_hViewModel());

					if (viewmodel)
					{
						auto eyeAng = vsView->angles;
						eyeAng.z -= (float)g_Options.viewmodel_offset_roll;

						viewmodel->set_abs_angles(eyeAng);
					}
				}

				if (weapon && weapon->IsBallistic())
				{
					if (!g_LocalPlayer->m_bIsScoped() && !g_Input->m_fCameraInThirdPerson) {
						auto viewmodel = (C_BaseEntity*)g_EntityList->GetClientEntityFromHandle(g_LocalPlayer->m_hViewModel());

						if (viewmodel)
						{
							auto eyeAng = vsView->angles;
							eyeAng.z -= (float)g_Options.viewmodel_offset_roll;

							viewmodel->set_abs_angles(eyeAng);
						}
					}
				}

				if (g_Options.fakeduck && GetAsyncKeyState(g_Options.fdkey) && g_LocalPlayer && g_LocalPlayer->IsAlive() && g_Input->m_fCameraInThirdPerson)
					vsView->origin.z = g_LocalPlayer->abs_origin().z + 64.f;

			}
			else
			{
				fov_cs_debug->SetValue(XorStr("0"));
			}

		}
		else
			return ofunc(vsView);

		ofunc(vsView);

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

		static IMaterial* Half = g_MatSystem->FindMaterial(XorStr("materials/models/inventory_items/dogtags/dogtags_lightray"), nullptr); //Like half and half but for shit
		static IMaterial* pulse = g_MatSystem->FindMaterial(XorStr("models/inventory_items/dogtags/dogtags_outline"), nullptr); //Pulse Shit
		static IMaterial* shine = g_MatSystem->FindMaterial(XorStr("models/inventory_items/cologne_prediction/cologne_prediction_glass"), nullptr); // P SHINE
		static IMaterial* othershine = g_MatSystem->FindMaterial(XorStr("models/inventory_items/trophy_majors/gloss"), nullptr); //Light Shine
		static IMaterial* velvet = g_MatSystem->FindMaterial(XorStr("models/inventory_items/trophy_majors/velvet"), nullptr); //Interium Glow
		static IMaterial* shit = (Chams::Get().CreateMaterial(true, R"#("VertexLitGeneric"
		    {
	"$additive" "1"
	"$envmap" "models/effects/cube_white"
	"$envmaptint" "[1 1 1]"
	"$envmapfresnel" "1"
	"$envmapfresnelminmaxexp" "[0 1 2]"
	"$alpha" "1.0"
		    }
		)#")); //The actual glow shit
		static IMaterial* intellect = (Chams::Get().CreateMaterial(true, R"#("VertexLitGeneric"
		    {
	"$basetexture" "models/inventory_items/dreamhack_trophies/dreamhack_star_blur"
    "$wireframe" "1"
    "$alpha" "0.6"
    "$additive" "1"
    "proxies"
     {
        "texturescroll"
        {
            "texturescrollvar" "$basetexturetransform"
            "texturescrollrate" "0.2"
            "texturescrollangle" "90"
        }
    }
		    }
		)#")); //intellect wireframe
		static IMaterial* dubble = g_MatSystem->FindMaterial(XorStr("dev/glow_armsrace"), nullptr); //Double material like an outline
		static IMaterial* animated2 = (Chams::Get().CreateMaterial(true, R"#("VertexLitGeneric"
{
    "$basetexture" "blade_mideffect_mask"
    "$texture2" "blade_mideffect"
     "$model" 1
    "$nocull" "1"
    "$additive" "1"
    "$color2" "[1 1 1]"


    "Proxies"
    {
        "TextureScroll"
        {
            "texturescrollvar" "$baseTextureTransform"
            "texturescrollrate" 4
            "texturescrollangle" 5
        }
        "Sine"
        {
            "resultVar" "$alpha"
            "sineperiod" 1
            "sinemin" 0.2
            "sinemax" 1.0
        }
        "Sine"
        {
            "resultVar" "$color[0]"
            "sineperiod" 1.3
            "sinemin" 0.8
            "sinemax" 1.4
        }
    }

}
		)#")); //this is the advancedaim lightning shit
		static IMaterial* regular = g_MatSystem->FindMaterial(XorStr("debug/debugambientcube"), nullptr); //Textured :weary:
		static IMaterial* flat = g_MatSystem->FindMaterial(XorStr("debug/debugdrawflat"), nullptr); //Flat like my girl chest :sob:
		static IMaterial* pocoglow = g_MatSystem->FindMaterial(XorStr("sprites/light_glow04"), nullptr); //Its like a glow but lighter and in bulbs

		C_BasePlayer* e = (C_BasePlayer*)g_EntityList->GetClientEntity(pInfo.entity_index);
		if (e == g_LocalPlayer && g_Options.chams_fake_enabled && CAntiAim::Get().CanDesync(Globals::m_cmd)) {
			if (Misc::Get().m_got_fake_matrix) {

				for (auto& i : Misc::Get().m_fake_matrix) {
					i[0][3] += pInfo.origin.x;
					i[1][3] += pInfo.origin.y;
					i[2][3] += pInfo.origin.z;
				}

				static IMaterial* player_enemies_type = nullptr;
				if (g_Options.player_material == 0)
					player_enemies_type = regular;
				if (g_Options.player_material == 1)
					player_enemies_type = flat;
				if (g_Options.player_material > 1)
					player_enemies_type = regular;
				if (g_Options.chams_fake_enabled)
				{
					player_enemies_type->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
					Chams::Get().modulate(g_Options.color_chams_fake, player_enemies_type);
					ofunc(_this, edx, ctx, state, pInfo, Misc::Get().m_fake_matrix);
					g_MdlRender->ForcedMaterialOverride(player_enemies_type);

					if (g_Options.player_material == 2)
					{
						shine->AlphaModulate(g_Options.player_enemy_visible_shine[3] / 255.f);
						bool bFound = false;
						auto pVar = shine->FindVar(XorStr("$envmaptint"), &bFound);
						if (bFound)
						{
							(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.player_enemy_visible_shine[0] / 255.f, g_Options.player_enemy_visible_shine[1] / 255.f, g_Options.player_enemy_visible_shine[2] / 255.f);
						}
						g_MdlRender->ForcedMaterialOverride(shine);
					}

					if (g_Options.player_material == 3)
					{
						velvet->AlphaModulate(g_Options.color_chams_fake[3] / 255.f);
						bool bFound = false;
						auto pVar = velvet->FindVar(XorStr("$envmaptint"), &bFound);
						if (bFound)
						{
							(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.color_chams_fake[0] / 255.f, g_Options.color_chams_fake[1] / 255.f, g_Options.color_chams_fake[2] / 255.f);
						}
						g_MdlRender->ForcedMaterialOverride(velvet);
					}

					if (g_Options.player_material == 4)
					{
						shit->AlphaModulate(g_Options.color_chams_fake[3] / 255.f);
						bool bFound = false;
						auto pVar = shit->FindVar(XorStr("$envmaptint"), &bFound);
						if (bFound)
						{
							(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.color_chams_fake[0] / 255.f, g_Options.color_chams_fake[1] / 255.f, g_Options.color_chams_fake[2] / 255.f);
						}
						g_MdlRender->ForcedMaterialOverride(shit);
					}

					if (g_Options.player_material == 5)
					{
						dubble->AlphaModulate(g_Options.color_chams_fake[3] / 255.f);
						bool bFound = false;
						auto pVar = dubble->FindVar(XorStr("$envmaptint"), &bFound);
						if (bFound)
						{
							(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.color_chams_fake[0] / 255.f, g_Options.color_chams_fake[1] / 255.f, g_Options.color_chams_fake[2] / 255.f);
						}
						g_MdlRender->ForcedMaterialOverride(dubble);
					}

					for (auto& i : Misc::Get().m_fake_matrix) {
						i[0][3] -= pInfo.origin.x;
						i[1][3] -= pInfo.origin.y;
						i[2][3] -= pInfo.origin.z;
					}

				}

			}
		}

		ofunc(_this, edx, ctx, state, pInfo, pCustomBoneToWorld);

		g_MdlRender->ForcedMaterialOverride(nullptr);

	}

	bool __fastcall hkSvCheatsGetBool(PVOID pConVar, void* edx)
	{
		static auto dwCAM_Think = Utils::PatternScan(GetModuleHandleW(L"client.dll"), XorStr("85 C0 75 30 38 86"));
		static auto ofunc = sv_cheats.get_original<bool(__thiscall*)(PVOID)>(13);
		if (!ofunc)
			return false;

		if (reinterpret_cast<DWORD>(_ReturnAddress()) == reinterpret_cast<DWORD>(dwCAM_Think))
			return true;
		return ofunc(pConVar);
	}

	void __fastcall hkDrawModelExecute2(void* _this, int, void* pResults, DrawModelInfo_t* pInfo, matrix3x4_t* pBoneToWorld, float* flpFlexWeights, float* flpFlexDelayedWeights, Vector& vrModelOrigin, int32_t iFlags)
	{
		static auto ofunc = stdrender_hook.get_original<decltype(&hkDrawModelExecute2)>(index::DrawModelExecute2);

		static auto flash = g_MatSystem->FindMaterial(XorStr("effects/flashbang"), TEXTURE_GROUP_CLIENT_EFFECTS);
		static auto flash_white = g_MatSystem->FindMaterial(XorStr("effects/flashbang_white"), TEXTURE_GROUP_CLIENT_EFFECTS);
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
		//CNightmode::Get().OnDME(pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);
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