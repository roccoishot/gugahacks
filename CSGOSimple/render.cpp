#include "render.hpp"

#include <mutex>

#include "features/visuals.hpp"
#include "valve_sdk/csgostructs.hpp"
#include "helpers/input.hpp"
#include "menu.hpp"
#include "options.hpp"
#include "helpers/math.hpp"
#include <ctime>

ImFont* g_Menufont;
ImFont* g_pDefaultFont;
ImFont* g_VeloFont;
ImFont* g_Fuck;
ImFont* g_Cum;
ImFont* g_SpectatorListFont;

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
	g_SpectatorListFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\corbelb.ttf", 12.0f, &font_config, ranges);


	// esp font
	g_pDefaultFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\corbelb.ttf", 25.f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
	

	// font for watermark; just example
	g_VeloFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\Arial.ttf", 30.f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());

	// font for watermark; just example
	g_Fuck = ImGui::GetIO().Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\Karla-Regular.ttf", 30.f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());

	g_Cum = ImGui::GetIO().Fonts->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\bahnschrift.ttf", 48.f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());

}

void Render::ClearDrawList() {
	render_mutex.lock();
	draw_list_act->Clear();
	render_mutex.unlock();
}

void Render::BeginScene() {
	int a = g_Options.aimbot.fov;
	int b = 12;
	long long c = a * b;

	draw_list->Clear();
	draw_list->PushClipRectFullScreen();

	if (g_Options.bowlsfreshcut && g_EngineClient->IsInGame() && g_LocalPlayer->IsFlashed()) {
		Render::Get().RenderText("FLASHED", 965, 545, 18.f, Color(120, 208, 255), false, true, g_VeloFont);
	}

	if (g_EngineClient->IsInGame() && g_LocalPlayer->IsAlive() && g_Options.ragebot_antiaim_desync && GetKeyState(g_Options.invertaakey)) {
		Render::Get().RenderText("LEFT", 60, 500, 30.f, g_Options.menucolor, false, true, g_Cum);
		}
	if (g_EngineClient->IsInGame() && g_LocalPlayer->IsAlive() && g_Options.ragebot_antiaim_desync && !GetKeyState(g_Options.invertaakey)) {
		Render::Get().RenderText("RIGHT", 60, 500, 30.f, g_Options.menucolor, false, true, g_Cum);
		}

	if (g_Options.misc_watermark) {
			std::string username = (" GUGAHACKER 8");
			#define VERSION ("Made By Roccoishot, Sopmk, Levito, and You're Mother. |")
			auto watermark = VERSION + username + (" | ");
				watermark = VERSION + username + (" | ");
				Render::Get().RenderText(watermark, 10, 5, 18.f, g_Options.menucolor, false, true, g_VeloFont);

				if (g_EngineClient->IsInGame()) {
					auto server = g_EngineClient->GetNetChannelInfo()->GetAddress();
					if (!strcmp(server, ("loopback")))
						server = ("Local server");
					std::string username = (" GUGAHACKER 8");
					#define VERSION ("Made By Roccoishot, Sopmk, Levito, and You're Mother. |")
					auto watermark = VERSION + username + (" | ") + server + (" | ");
					watermark = VERSION + username + (" | ") + server + (" | ");
					Render::Get().RenderText(watermark, 10, 5, 18.f, g_Options.menucolor, false, true, g_VeloFont);
				}
			}

	if (g_Options.enablebeta)
		Render::Get().RenderText("USING TEST", 10, 35, 18.f, g_Options.menucolor, false, true, g_VeloFont);

	if (g_Options.backtix > 16)
		Render::Get().RenderText("WARNING: This Backtrack Can Be Unstable!", 10, 65, 18.f, g_Options.menucolor, false, true, g_VeloFont);
		
	if (g_Options.drawfov)
		Render::Get().RenderCircle(960.f, 540.f, c, 48, g_Options.menucolor, 1.3f);

	if (g_EngineClient->IsInGame() && g_LocalPlayer)
		Visuals::Get().AddToDrawList();

	int screenWidth, screenHeight;
	g_EngineClient->GetScreenSize(screenWidth, screenHeight);

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
