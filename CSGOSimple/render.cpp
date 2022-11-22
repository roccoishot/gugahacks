#include "render.hpp"

#include <mutex>

#include "features/visuals.hpp"
#include "valve_sdk/csgostructs.hpp"
#include "helpers/input.hpp"
#include "menu.hpp"
#include "options.hpp"
#include "helpers/math.hpp"
#include <ctime>
#include "Globals.h"
#include "xor.h"
#include "./features/Misc.hpp"

#ifdef ENABLE_XOR
#define XorStr _xor_ 
#else
#define XorStr
#endif
#pragma intrinsic(_ReturnAddress)  

ImFont* g_Menufont;
ImFont* g_pDefaultFont;
ImFont* g_VeloFont;
ImFont* g_Cum;
ImFont* g_SpectatorListFont;
ImFont* g_MenuFont;
ImFont* uhFont;
ImFont* g_Frotnite;
ImFont* Cummed;
ImFont* SHitted;
ImDrawListSharedData _data;

std::mutex render_mutex;

void Render::Initialize()
{
	ImGui::CreateContext();


	ImGui_ImplWin32_Init(InputSys::Get().GetMainWindow());
	ImGui_ImplDX9_Init(g_D3DDevice9);

	draw_list = new ImDrawList(ImGui::GetDrawListSharedData());
	draw_list_act = new ImDrawList(ImGui::GetDrawListSharedData());
	draw_list_rendering = new ImDrawList(ImGui::GetDrawListSharedData());

	GetFonts();
}

void Render::GetFonts() {

	// menu font
	ImGui::GetIO().Fonts->AddFontDefault(NULL);

	ImFontConfig font_config;
	font_config.OversampleH = 1; //or 2 is the same
	font_config.OversampleV = 1;
	font_config.PixelSnapH = 1;

	static const ImWchar ranges[] =
	{
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x0400, 0x044F, // Cyrillic
		0,
	};
	ImGuiIO& io = ImGui::GetIO();
	g_SpectatorListFont = io.Fonts->AddFontFromFileTTF(XorStr("C:\\Windows\\Fonts\\corbelb.ttf"), 48.f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());

	g_MenuFont = io.Fonts->AddFontFromFileTTF(XorStr("C:\\Windows\\Fonts\\corbelb.ttf"), 15.f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
	uhFont = io.Fonts->AddFontFromFileTTF(XorStr("C:\\Windows\\Fonts\\corbelb.ttf"), 18.f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());

	SHitted = io.Fonts->AddFontFromFileTTF(XorStr("C:\\Windows\\Fonts\\corbelb.ttf"), 24.f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());

	//#include "./Roccofont2-Regular.c"
	//g_pDefaultFont = io.Fonts->AddFontFromMemoryTTF(roccofont, 48.f, NULL, nullptr);

	// esp font
	g_pDefaultFont = io.Fonts->AddFontFromFileTTF(XorStr("C:\\Windows\\Fonts\\corbelb.ttf"), 48.f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());

	// font for watermark; just example
	g_VeloFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(XorStr(u8"C:\\Windows\\Fonts\\verdanab.ttf"), 48.f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());

	g_Cum = ImGui::GetIO().Fonts->AddFontFromFileTTF(XorStr(u8"C:\\Windows\\Fonts\\bahnschrift.ttf"), 48.f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());

	Cummed = ImGui::GetIO().Fonts->AddFontFromFileTTF(XorStr(u8"C:\\Windows\\Fonts\\corbelb.ttf"), 48.f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
}

void Render::ClearDrawList() {
	render_mutex.lock();
	draw_list_act->Clear();
	render_mutex.unlock();
}

void Render::BeginScene() {
	int a = g_Options.aimbot.fov;
	int b = 11;
	long long c = a * b;
	int screenWidth, screenHeight;
	g_EngineClient->GetScreenSize(screenWidth, screenHeight);
	draw_list->Clear();
	draw_list->PushClipRectFullScreen();

	std::string watermark = (XorStr("| GUGAHACKS.KITCHEN |"));

	if (g_EngineClient->IsInGame())
	{
		std::string negro = g_EngineClient->GetNetChannelInfo()->GetAddress();
		if (negro.find(XorStr("[")) != std::string::npos)
			Globals::valve = true;
		else
			Globals::valve = false;
		if (negro.find(XorStr("74.91.124.24:27015")) != std::string::npos)
			Globals::stepping = true;
		else
			Globals::stepping = false;
	}

	if (g_Options.misc_watermark) {
		if (g_EngineClient->IsInGame())
		{
			int ping;

			auto nci = g_EngineClient->GetNetChannelInfo();

			if (nci)
			{
				auto latency = g_EngineClient->IsPlayingDemo() ? 0.0f : nci->GetAvgLatency(FLOW_OUTGOING);

				if (latency) //-V550
				{
					static auto cl_updaterate = g_CVar->FindVar(XorStr("cl_updaterate"));
					latency -= 0.5f / cl_updaterate->GetFloat();
				}

				ping = (int)(max(0.0f, latency) * 1000.0f);
			}
			auto negronuts = std::to_string((int)ping);

			std::string negro = g_EngineClient->GetNetChannelInfo()->GetAddress();
			auto server = g_EngineClient->GetNetChannelInfo()->GetAddress();
			if (!strcmp(server, (XorStr("loopback"))))
				server = (XorStr("Local server"));
			if (negro.find(XorStr("[")) != std::string::npos)
				server = (XorStr("Valve Server"));
			if (!strcmp(server, (XorStr("74.91.124.24:27015"))))
				server = (XorStr("Big Steppa"));
			if (!strcmp(server, (XorStr("192.223.26.36:27015"))))
				server = (XorStr("Luckys Gay Ass Server"));
			std::string cum = (XorStr(" | "));
			auto tickrate = std::to_string((int)(1.0f / g_GlobalVars->interval_per_tick));
			watermark = (XorStr("| GUGAHACKS.KITCHEN")) + cum + server + cum + XorStr("Ping: ") + negronuts + cum;
		}

		Render::Get().RenderText(watermark, 10, 5, 16.f, g_Options.menucolor, false, true, SHitted);
	}

	const char* exploit = " ";

	if (g_EngineClient->IsInGame() && g_LocalPlayer->IsAlive()) {
		if (GetKeyState(g_Options.aimbot.dthotkey) && g_Options.aimbot.dt)
			exploit = "DT Be On";
		else if (GetKeyState(g_Options.aimbot.hshotkey) && g_Options.aimbot.hs)
			exploit = "Where Yo Shots At";

		Render::Get().RenderText(XorStr(exploit), 10, 40, 16.f, g_Options.menucolor, false, true, SHitted);
	}
	else
	{
		Render::Get().RenderText(exploit, 10, 40, 16.f, g_Options.menucolor, false, true, SHitted);
	}

	if (g_EngineClient->IsInGame() && g_LocalPlayer && g_LocalPlayer->IsAlive() && g_Options.drawfov && c > 0)
		Render::Get().RenderCircle(screenWidth / 2, screenHeight / 2, c, 48, g_Options.menucolor, 1.3f);

	if (g_EngineClient->IsInGame() && g_LocalPlayer)
		Visuals::Get().AddToDrawList();

	if (g_Options.noscope && g_EngineClient->IsInGame() && g_LocalPlayer->m_bIsScoped() && g_LocalPlayer->m_hActiveWeapon().Get() && !g_LocalPlayer->m_hActiveWeapon()->IsBallistic()) {
		Render::Get().RenderLine(screenWidth / 2, 0, screenWidth / 2, screenHeight, Color(0, 0, 0, 255), g_CVar->FindVar(XorStr("cl_crosshair_sniper_width"))->GetFloat());
		Render::Get().RenderLine(0, screenHeight / 2, screenWidth, screenHeight / 2, Color(0, 0, 0, 255), g_CVar->FindVar(XorStr("cl_crosshair_sniper_width"))->GetFloat());
	}

	render_mutex.lock();
	*draw_list_act = *draw_list;
	render_mutex.unlock();
}

ImDrawList* Render::RenderScene() {

	if (render_mutex.try_lock()) {
		*draw_list_rendering = *draw_list_act;
		render_mutex.unlock();
	}

	return draw_list_rendering;
}

float Render::RenderText(const std::string& text, ImVec2 pos, float size, Color color, bool center, bool outline, ImFont* pFont)
{
	ImVec2 textSize = pFont->CalcTextSizeA(size, FLT_MAX, 0.0f, text.c_str());
	if (!pFont->ContainerAtlas) return 0.f;
	draw_list->PushTextureID(pFont->ContainerAtlas->TexID);

	if (center)
		pos.x -= textSize.x / 2.0f;

	if (outline) {
		draw_list->AddText(pFont, size, ImVec2(pos.x + 1, pos.y + 1), GetU32(Color(0, 0, 0, color.a())), text.c_str());
		draw_list->AddText(pFont, size, ImVec2(pos.x - 1, pos.y - 1), GetU32(Color(0, 0, 0, color.a())), text.c_str());
		draw_list->AddText(pFont, size, ImVec2(pos.x + 1, pos.y - 1), GetU32(Color(0, 0, 0, color.a())), text.c_str());
		draw_list->AddText(pFont, size, ImVec2(pos.x - 1, pos.y + 1), GetU32(Color(0, 0, 0, color.a())), text.c_str());
	}

	draw_list->AddText(pFont, size, pos, GetU32(color), text.c_str());

	draw_list->PopTextureID();

	return pos.y + textSize.y;
}

void Render::RenderCircle3D(Vector position, float points, float radius, Color color)
{
	float step = (float)M_PI * 2.0f / points;

	for (float a = 0; a < (M_PI * 2.0f); a += step)
	{
		Vector start(radius * cosf(a) + position.x, radius * sinf(a) + position.y, position.z);
		Vector end(radius * cosf(a + step) + position.x, radius * sinf(a + step) + position.y, position.z);

		Vector start2d, end2d;
		if (g_DebugOverlay->ScreenPosition(start, start2d) || g_DebugOverlay->ScreenPosition(end, end2d))
			return;

		RenderLine(start2d.x, start2d.y, end2d.x, end2d.y, color);
	}
}
