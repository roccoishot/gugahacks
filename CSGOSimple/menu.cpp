#include "Menu.hpp"
#define NOMINMAX
#include <Windows.h>
#include <chrono>

#include "valve_sdk/csgostructs.hpp"
#include "helpers/input.hpp"
#include "options.hpp"
#include "features/Misc.hpp"
#include "ui.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui_internal.h"
#include "imgui/impl/imgui_impl_dx9.h"
#include "imgui/impl/imgui_impl_win32.h"
#include "features/kit_parser.h"
#include "render.hpp"
#include "./features/item_definitions.h"
#include "Globals.h"
#include "xor.h"
#include "./features/skins.h"
#include "sexynutssexyfuckdickshitnigga.h"
#include "image.hpp"

#ifdef ENABLE_XOR
#define XorStr _xor_ 
#else
#define XorStr
#endif
#pragma intrinsic(_ReturnAddress)  

using namespace std;

void ReadDirectory(const std::string& name, std::vector<std::string>& v)
{
	auto pattern(name);
	pattern.append("\\*.cfg");
	WIN32_FIND_DATAA data;
	HANDLE hFind;
	if ((hFind = FindFirstFileA(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE)
	{
		do
		{
			v.emplace_back(data.cFileName);
		} while (FindNextFileA(hFind, &data) != 0);
		FindClose(hFind);
	}
}
struct hud_weapons_t {
	std::int32_t* get_weapon_count() {
		return reinterpret_cast<std::int32_t*>(std::uintptr_t(this) + 0x80);
	}
};

namespace ImGuiEx
{
	inline bool ColorEdit4(const char* label, Color* v, bool show_alpha = true)
	{
		float clr[4] = {
			v->r() / 255.0f,
			v->g() / 255.0f,
			v->b() / 255.0f,
			v->a() / 255.0f
		};
		//clr[3]=255;
		if (ImGui::ColorEdit4(label, clr, ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_AlphaBar)) {
			v->SetColor(clr[0], clr[1], clr[2], clr[3]);
			return true;
		}
		return false;
	}
	inline bool ColorEdit4a(const char* label, Color* v, bool show_alpha = true)
	{
		float clr[4] = {
			v->r() / 255.0f,
			v->g() / 255.0f,
			v->b() / 255.0f,
			v->a() / 255.0f
		};
		//clr[3]=255;
		if (ImGui::ColorEdit4(label, clr, show_alpha | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_AlphaBar)) {
			v->SetColor(clr[0], clr[1], clr[2], clr[3]);
			return true;
		}
		return false;
	}

	inline bool ColorEdit3(const char* label, Color* v)
	{
		return ColorEdit4(label, v, false);
	}
}


IDirect3DTexture9* rageiconD = nullptr;
IDirect3DTexture9* legiticonD = nullptr;
IDirect3DTexture9* visualsiconD = nullptr;
IDirect3DTexture9* misciconD = nullptr;
IDirect3DTexture9* skinsiconD = nullptr;
IDirect3DTexture9* sexdickD = nullptr;
IDirect3DTexture9* playericonD = nullptr;
IDirect3DTexture9* cfgiconD = nullptr;
void Menu::Initialize()
{
	CreateStyle();

	_visible = true;

	//cry
	if (guger == nullptr) {
		D3DXCreateTextureFromFileInMemory(g_D3DDevice9, &text, sizeof(text), &guger);
	}
	if (bg == nullptr) {
		D3DXCreateTextureFromFileInMemory(g_D3DDevice9, &background, sizeof(background), &bg);
	}

	if (rageiconD == nullptr) {
		D3DXCreateTextureFromFileInMemoryEx(g_D3DDevice9, &rageicon, sizeof(rageicon), 42, 42, D3DX_DEFAULT, D3DUSAGE_DYNAMIC, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &rageiconD);
	}
	if (legiticonD == nullptr) {
		D3DXCreateTextureFromFileInMemoryEx(g_D3DDevice9, &legiticon, sizeof(legiticon), 42, 42, D3DX_DEFAULT, D3DUSAGE_DYNAMIC, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &legiticonD);
	}
	if (visualsiconD == nullptr) {
		D3DXCreateTextureFromFileInMemoryEx(g_D3DDevice9, &visualsicon, sizeof(visualsicon), 42, 42, D3DX_DEFAULT, D3DUSAGE_DYNAMIC, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &visualsiconD);
	}
	if (misciconD == nullptr) {
		D3DXCreateTextureFromFileInMemoryEx(g_D3DDevice9, &miscicon, sizeof(miscicon), 42, 42, D3DX_DEFAULT, D3DUSAGE_DYNAMIC, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &misciconD);
	}
	if (skinsiconD == nullptr) {
		D3DXCreateTextureFromFileInMemoryEx(g_D3DDevice9, &skinsicon, sizeof(skinsicon), 42, 42, D3DX_DEFAULT, D3DUSAGE_DYNAMIC, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &skinsiconD);
	}
	if (cfgiconD == nullptr) {
		D3DXCreateTextureFromFileInMemoryEx(g_D3DDevice9, &cfgicon, sizeof(cfgicon), 42, 42, D3DX_DEFAULT, D3DUSAGE_DYNAMIC, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &cfgiconD);
	}
	if (sexdickD == nullptr) {
		D3DXCreateTextureFromFileInMemoryEx(g_D3DDevice9, &sexdick, sizeof(sexdick), 42, 42, D3DX_DEFAULT, D3DUSAGE_DYNAMIC, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &sexdickD);
	}
}

void Menu::Shutdown()
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Menu::OnDeviceLost()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
}

void Menu::OnDeviceReset()
{
	ImGui_ImplDX9_CreateDeviceObjects();
}
bool Tab(const char* label, const ImVec2& size_arg, bool state, ImTextureID user_texture_id)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos;
	ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, pos + size);
	ImGui::ItemSize(size, style.FramePadding.y);
	if (!ImGui::ItemAdd(bb, id))
		return false;
	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, NULL);
	if (pressed)
		ImGui::MarkItemEdited(id);

	//ImGui::RenderFrame(bb.Min, bb.Max, state ? ImColor(15, 15, 15) : ImColor(11, 11, 11), true, style.FrameRounding);
	//window->DrawList->AddRect(bb.Min, bb.Max, ImColor(0, 0, 0));


	const ImVec4 active_dis = ImVec4(255 / 255.f, 255 / 255.f, 255 / 255.f, 1.f);
	const ImVec4 hover_dis = ImVec4(255 / 255.f, 255 / 255.f, 255 / 255.f, 0.7f);
	const ImVec4 else_dis = ImVec4(255 / 255.f, 255 / 255.f, 255 / 255.f, 0.4f);

	float deltatime = 2.5f * ImGui::GetIO().DeltaTime;

	static std::map<ImGuiID, ImVec4> hover_animation;
	auto it_hover = hover_animation.find(id);
	if (it_hover == hover_animation.end()) {
		hover_animation.insert({ id, hover_dis });
		it_hover = hover_animation.find(id);
	}
	if (state) {
		ImVec4 to = active_dis;
		if (it_hover->second.x != to.x) {
			if (it_hover->second.x < to.x)
				it_hover->second.x = ImMin(it_hover->second.x + deltatime, to.x);
			else if (it_hover->second.x > to.x)
				it_hover->second.x = ImMax(to.x, it_hover->second.x - deltatime);
		}

		if (it_hover->second.y != to.y) {
			if (it_hover->second.y < to.y)
				it_hover->second.y = ImMin(it_hover->second.y + deltatime, to.y);
			else if (it_hover->second.y > to.y)
				it_hover->second.y = ImMax(to.y, it_hover->second.y - deltatime);
		}

		if (it_hover->second.z != to.z) {
			if (it_hover->second.z < to.z)
				it_hover->second.z = ImMin(it_hover->second.z + deltatime, to.z);
			else if (it_hover->second.z > to.z)
				it_hover->second.z = ImMax(to.z, it_hover->second.z - deltatime);
		}

		if (it_hover->second.w != to.w) {
			if (it_hover->second.w < to.w)
				it_hover->second.w = ImMin(it_hover->second.w + deltatime, to.w);
			else if (it_hover->second.w > to.w)
				it_hover->second.w = ImMax(to.w, it_hover->second.w - deltatime);
		}
	}
	else if (hovered || pressed) {
		ImVec4 to = hover_dis;
		if (it_hover->second.x != to.x) {
			if (it_hover->second.x < to.x)
				it_hover->second.x = ImMin(it_hover->second.x + deltatime, to.x);
			else if (it_hover->second.x > to.x)
				it_hover->second.x = ImMax(to.x, it_hover->second.x - deltatime);
		}

		if (it_hover->second.y != to.y) {
			if (it_hover->second.y < to.y)
				it_hover->second.y = ImMin(it_hover->second.y + deltatime, to.y);
			else if (it_hover->second.y > to.y)
				it_hover->second.y = ImMax(to.y, it_hover->second.y - deltatime);
		}

		if (it_hover->second.z != to.z) {
			if (it_hover->second.z < to.z)
				it_hover->second.z = ImMin(it_hover->second.z + deltatime, to.z);
			else if (it_hover->second.z > to.z)
				it_hover->second.z = ImMax(to.z, it_hover->second.z - deltatime);
		}

		if (it_hover->second.w != to.w) {
			if (it_hover->second.w < to.w)
				it_hover->second.w = ImMin(it_hover->second.w + deltatime, to.w);
			else if (it_hover->second.w > to.w)
				it_hover->second.w = ImMax(to.w, it_hover->second.w - deltatime);
		}
	}
	else {
		ImVec4 to = else_dis;
		if (it_hover->second.x != to.x) {
			if (it_hover->second.x < to.x)
				it_hover->second.x = ImMin(it_hover->second.x + deltatime, to.x);
			else if (it_hover->second.x > to.x)
				it_hover->second.x = ImMax(to.x, it_hover->second.x - deltatime);
		}

		if (it_hover->second.y != to.y) {
			if (it_hover->second.y < to.y)
				it_hover->second.y = ImMin(it_hover->second.y + deltatime, to.y);
			else if (it_hover->second.y > to.y)
				it_hover->second.y = ImMax(to.y, it_hover->second.y - deltatime);
		}

		if (it_hover->second.z != to.z) {
			if (it_hover->second.z < to.z)
				it_hover->second.z = ImMin(it_hover->second.z + deltatime, to.z);
			else if (it_hover->second.z > to.z)
				it_hover->second.z = ImMax(to.z, it_hover->second.z - deltatime);
		}

		if (it_hover->second.w != to.w) {
			if (it_hover->second.w < to.w)
				it_hover->second.w = ImMin(it_hover->second.w + deltatime, to.w);
			else if (it_hover->second.w > to.w)
				it_hover->second.w = ImMax(to.w, it_hover->second.w - deltatime);
		}
	}
	static std::map<ImGuiID, float> filled_animation;
	auto it_filled = filled_animation.find(id);
	if (it_filled == filled_animation.end()) {
		filled_animation.insert({ id, 0.f });
		it_filled = filled_animation.find(id);
	}


	ImColor white(255.f, 255.f, 255.f, Menu::Get().alpha);
	ImColor grey(122.f / 255.f, 122.f / 255.f, 122.f / 255.f, Menu::Get().alpha);

	ImGui::GetWindowDrawList()->AddImage(user_texture_id, bb.Min + ImVec2(5 + 8, 0 - 3), bb.Min + ImVec2(30 + 8 + 5 + 8, 30 + 8 - 3), ImVec2(0, 0), ImVec2(1, 1), ImGui::GetColorU32(it_hover->second));


	window->DrawList->AddText(uhFont, 18.f, bb.Min + ImVec2(58 - 3 + 8, 8), ImGui::GetColorU32(it_hover->second), label);

	if (state)
	{
		/*
		window->DrawList->AddLine(bb.Min, bb.Min + ImVec2(9, 0), ImColor(255, 255, 255), 1);
		window->DrawList->AddLine(bb.Min, bb.Min + ImVec2(0, 9), ImColor(255, 255, 255), 1);

		window->DrawList->AddLine(bb.Max - ImVec2(0, 1), bb.Max - ImVec2(10, 1), ImColor(255, 255, 255), 1);
		window->DrawList->AddLine(bb.Max - ImVec2(1, 1), bb.Max - ImVec2(1, 10), ImColor(255, 255, 255), 1);
	*/
	}

	return pressed;
}

void Menu::SpecList()
{
	if (!g_Options.speclist)
		return;

	ImGuiStyle* Style = &ImGui::GetStyle();
	Style->WindowBorderSize = 0.5;
	Style->FrameBorderSize = 0.5;
	Style->ChildBorderSize = 0.5;
	Style->WindowRounding = 0;
	Style->ChildRounding = 0;
	Style->FrameRounding = 0;
	Style->ScrollbarSize = 6;
	Style->ScrollbarRounding = 0;
	Style->PopupRounding = 0;
	Style->GrabRounding = 0;
	Style->Colors[ImGuiCol_Text] = ImColor(255, 255, 255, 255);
	Style->Colors[ImGuiCol_TitleBg] = ImColor(9, 9, 9);
	Style->Colors[ImGuiCol_Border] = ImColor(g_Options.menucolor.r(), g_Options.menucolor.g(), g_Options.menucolor.b(), 255);
	Style->Colors[ImGuiCol_Separator] = ImColor(g_Options.menucolor.r(), g_Options.menucolor.g(), g_Options.menucolor.b(), 255);
	Style->Colors[ImGuiCol_WindowBg] = ImColor(9, 9, 9);
	Style->Colors[ImGuiCol_ChildBg] = ImColor(9, 9, 9);
	Style->Colors[ImGuiCol_FrameBg] = ImColor(18, 18, 18);
	Style->Colors[ImGuiCol_Button] = ImColor(18, 18, 18);
	Style->Colors[ImGuiCol_ButtonHovered] = ImColor(0, 0, 0);
	Style->Colors[ImGuiCol_ButtonActive] = ImColor(0, 0, 0);
	Style->Colors[ImGuiCol_ScrollbarGrab] = ImColor(g_Options.menucolor.r(), g_Options.menucolor.g(), g_Options.menucolor.b(), 255);
	Style->Colors[ImGuiCol_ScrollbarBg] = ImColor(19, 19, 19);
	Style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImColor(0, 0, 0);
	Style->Colors[ImGuiCol_ScrollbarGrabActive] = ImColor(g_Options.menucolor.r(), g_Options.menucolor.g(), g_Options.menucolor.b(), 255);
	ImGui::PushFont(g_MenuFont);

	int modes = 0;
	std::string spect = u8"";
	std::string mode = u8"";
	int DrawIndex = 1;

	int specs = 0;
	for (int playerId : Utils::GetObservervators(g_EngineClient->GetLocalPlayer()))
	{
		C_BasePlayer* pPlayer = (C_BasePlayer*)g_EntityList->GetClientEntity(playerId);

		if (playerId == g_LocalPlayer->EntIndex())
			continue;

		if (!pPlayer)
			continue;

		player_info_t Pinfo;

		g_EngineClient->GetPlayerInfo(playerId, &Pinfo);

		if (Pinfo.fakeplayer)
			continue;

		spect += Pinfo.szName;
		spect += u8"\n";
		specs++;

	}

	g_Options.spectators = specs;

	bool misc_Spectators = true;

	float sexdickyaw = 1.f;

	if (g_EngineClient->IsInGame() && specs)
		sexdickyaw = 1.f;
	else
		sexdickyaw = alpha;

	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, sexdickyaw);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, 1.f);
	if (ImGui::Begin(XorStr("Spectator List"), &g_Options.speclist, ImVec2(), 1.f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
	{
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
		if (specs > 0) spect += u8"\n";     /*ЛАСТ*/
		if (modes > 0) mode += u8"\n";
		ImVec2 size = ImGui::CalcTextSize(spect.c_str());

		ImGui::SetWindowSize(ImVec2(200, 25 + size.y));
		ImGui::SetWindowPos(ImVec2(g_Options.specx, g_Options.specy));
		ImGui::Separator(XorStr("Spectator List"));
		ImGui::Text(spect.c_str());
		DrawIndex++;
		ImGui::PopFont();
	}
	ImGui::End();
}

// kys
void spacing() {
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5);
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 8);
}

void sliderspacing() {
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 28);
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 8);
}
void colspacing() {
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	ImGui::SameLine(window->Size.x - 18 - 8);
}
void colspacing2() {
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	ImGui::SameLine(window->Size.x - 45);
}
void colspacing3() {
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	ImGui::SameLine(window->Size.x - 45 - 19);
}
void keybindspacing() {
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	ImGui::SameLine(window->Size.x - 62);
}
void Menu::Render()
{
	//RenderRadio();

#define alphaflags uiColorEditFlags_NoInputs | uiColorEditFlags_AlphaPreview | uiColorEditFlags_AlphaBar | uiColorEditFlags_NoOptions
#define noalphaflags uiColorEditFlags_NoInputs | uiColorEditFlags_NoAlpha

	static int tab = 0;
	ImGuiStyle* Style = &ImGui::GetStyle();
	ImColor bg(9.f / 255.f, 9.f / 255.f, 9.f / 255.f, alpha);
	ImColor grey(122, 122, 122, (int)(alpha * 255));
	ImColor accent(g_Options.menucolor.r(), g_Options.menucolor.g(), g_Options.menucolor.b(), (int)(alpha * 255));

	Style->WindowBorderSize = 0.5;
	Style->FrameBorderSize = 0.5;
	Style->ChildBorderSize = 0.5;
	Style->WindowRounding = 0;
	Style->ChildRounding = 0;
	Style->FrameRounding = 0;
	Style->ScrollbarSize = 6;
	Style->ScrollbarRounding = 0;
	Style->PopupRounding = 0;
	Style->GrabRounding = 0;
	Style->Colors[ImGuiCol_Text] = ImColor(255, 255, 255, 255);
	Style->Colors[ImGuiCol_TitleBg] = ImColor(8, 8, 8);
	Style->Colors[ImGuiCol_Border] = ImColor(g_Options.menucolor.r(), g_Options.menucolor.g(), g_Options.menucolor.b(), 255);
	Style->Colors[ImGuiCol_Separator] = ImColor(g_Options.menucolor.r(), g_Options.menucolor.g(), g_Options.menucolor.b(), 255);
	Style->Colors[ImGuiCol_WindowBg] = ImColor(8, 8, 8);
	Style->Colors[ImGuiCol_ChildBg] = ImColor(8, 8, 8, 0);
	Style->Colors[ImGuiCol_FrameBg] = ImColor(17, 17, 17);
	Style->Colors[ImGuiCol_Button] = ImColor(17, 17, 17);
	Style->Colors[ImGuiCol_ButtonHovered] = ImColor(0, 0, 0);
	Style->Colors[ImGuiCol_ButtonActive] = ImColor(0, 0, 0);
	Style->Colors[ImGuiCol_ScrollbarGrab] = ImColor(g_Options.menucolor.r(), g_Options.menucolor.g(), g_Options.menucolor.b(), 255);
	Style->Colors[ImGuiCol_ScrollbarBg] = ImColor(18, 18, 18);
	Style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImColor(0, 0, 0);
	Style->Colors[ImGuiCol_ScrollbarGrabActive] = ImColor(g_Options.menucolor.r(), g_Options.menucolor.g(), g_Options.menucolor.b(), 255);

	//lol
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.f, 0.f, 0.f, 0.f));
	ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, ImVec4(0.f, 0.f, 0.f, 0.f));
	ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(bg));

	ImGui::PushFont(g_MenuFont);

	static int testint = 100;
	static int testint2 = 0;
	static int testint3 = 0;
	static int testint4 = 0;

	auto flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | NULL | ImGuiWindowFlags_NoCollapse | NULL | NULL | NULL;

	int w, h;
	g_EngineClient->GetScreenSize(w, h);
	int x = w / 2;
	int y = h / 2;

	float end_width = 727;

	static float m_alpha = 1.f;
	static float m_width = 0.f;

	if (_visible)
		m_width = std::clamp(m_width + (3.f * end_width * ImGui::GetIO().DeltaTime * (_visible ? 1.f : -1.f)), 0.0001f, end_width);
	else
		m_width = std::clamp(m_width - (3.f * end_width * ImGui::GetIO().DeltaTime * (_visible ? -1.f : 1.f)), 0.0001f, end_width);

	if (m_width == end_width)
		m_alpha = std::clamp(m_alpha + (3.f * ImGui::GetIO().DeltaTime * (_visible ? 1.f : -1.f)), 0.0001f, 1.f);
	else
		m_alpha = std::clamp(m_alpha - (3.f * ImGui::GetIO().DeltaTime * (_visible ? -1.f : 1.f)), 0.0001f, 1.f);


	// set alpha in class to use later in widgets
	alpha = m_alpha;

	if (m_width <= 0.0001f && !_visible)
		return;


	int rightsidesize = end_width;
	int verticalsize = 560;

	ImVec2 startsize = ImVec2(500, verticalsize); //setup sum vars


	rightsidesize = m_width;
	verticalsize = startsize.y;


	ImVec2 pos = ImVec2(x + (startsize.x / 2 ), y + (startsize.y / 2));

	ImGui::SetNextWindowPos(ImVec2{ x + (startsize.x / 2), y + (startsize.y / 2) }, ImGuiSetCond_Once);
	ImGui::SetNextWindowSize(ImVec2(m_width, startsize.y), ImGuiSetCond_Always);


	//"PLEASE NO SCAMMER BIG MON 🥵🍆💦"
	if (ImGui::Begin(("Gugerhax"), &_visible, ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground | ImGuiColorEditFlags_NoInputs | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoResize)) {


		verticalsize = ImGui::GetWindowHeight();
		pos = ImGui::GetWindowPos();



		//bg
		ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y), ImVec2(ImGui::GetWindowPos().x + rightsidesize, ImGui::GetWindowPos().y + verticalsize), bg, bg, bg, bg);

		//surrounding lines fuck u rocco
		ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(ImGui::GetWindowPos().x + 5, ImGui::GetWindowPos().y + verticalsize), ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y), accent, accent, accent, accent);
		ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + 7), ImVec2(ImGui::GetWindowPos().x + rightsidesize, ImGui::GetWindowPos().y), accent, accent, accent, accent);
		ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(ImGui::GetWindowPos().x + rightsidesize - 4, ImGui::GetWindowPos().y + verticalsize), ImVec2(ImGui::GetWindowPos().x + rightsidesize, ImGui::GetWindowPos().y), accent, accent, accent, accent);

		ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(ImGui::GetWindowPos().x + 116 + 37, ImGui::GetWindowPos().y), ImVec2(ImGui::GetWindowPos().x + 115 + 37, ImGui::GetWindowPos().y + verticalsize), accent, accent, accent, accent);
		ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + verticalsize), ImVec2(ImGui::GetWindowPos().x + rightsidesize, ImGui::GetWindowPos().y + verticalsize - 1), accent, accent, accent, accent);


		ImGui::SetCursorPos(ImVec2(3, -32));
		ImGui::Image(Menu::Get().bg, ImVec2(150, 150), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1.f, 1.f, 1.f, alpha));
		ImGui::SetCursorPos(ImVec2(3, -32));
		ImGui::Image(Menu::Get().guger, ImVec2(150, 150), ImVec2(0, 0), ImVec2(1, 1), ImColor(g_Options.menucolor.r(), g_Options.menucolor.g(), g_Options.menucolor.b(), (int)(alpha * 255)));

		static int tab = 0;


		if (m_alpha <= 1.f)
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, m_alpha);

		widget_alpha = alpha;

		ImGui::SetCursorPos(ImVec2(0, 130));

		if (Tab(XorStr("Aimbot"), ImVec2(163, 36), tab == 0, legiticonD))
			tab = 0;

		int tabspacing = 23;

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + tabspacing);
		ImGui::SetCursorPosX(0);
		if (Tab(XorStr("Antiaim"), ImVec2(163, 36), tab == 1, rageiconD))
			tab = 1;

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + tabspacing);
		ImGui::SetCursorPosX(0);
		if (Tab(XorStr("Visuals"), ImVec2(163, 36), tab == 2, visualsiconD))
			tab = 2;

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + tabspacing);
		ImGui::SetCursorPosX(0);
		if (Tab(XorStr("Chams"), ImVec2(163, 36), tab == 4, skinsiconD))
			tab = 4;


		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + tabspacing);
		ImGui::SetCursorPosX(0);
		if (Tab(XorStr("World"), ImVec2(163, 36), tab == 5, cfgiconD))
			tab = 5;

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + tabspacing);
		ImGui::SetCursorPosX(0);
		if (Tab(XorStr("Misc"), ImVec2(163, 36), tab == 3, misciconD))
			tab = 3;

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + tabspacing);
		ImGui::SetCursorPosX(0);
		if (Tab(XorStr("Skins"), ImVec2(163, 36), tab == 6, sexdickD))
			tab = 6;


		//aimbot
		if (tab == 0) {
			ImGui::SetCursorPos(ImVec2(173, 50));
			ImGui::BeginChild("Aimbot", ImVec2(254, 498)); {

				ImGui::Spacing();
				ImGui::Spacing();
				spacing();
				ImGui::Checkbox(XorStr("Enabled"), &g_Options.aimbot.enabled); keybindspacing(); ImGui::Hotkey(XorStr("##aimkey"), &g_Options.aimbot.aimkey);
				sliderspacing();
				ImGui::SliderInt(XorStr("Fov"), &g_Options.aimbot.fov, 0, 20, XorStr("%.f"));
				sliderspacing();
				ImGui::SliderInt(XorStr("Smooth"), &g_Options.aimbot.smoof, 1, 20, XorStr("%.f"));
				spacing();
				if (ImGui::BeginCombo(XorStr("##hitbox_filter"), XorStr("Hitboxes")))
				{
					ImGui::Selectable(XorStr("Head"), &g_Options.aimbot.head, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XorStr("Neck"), &g_Options.aimbot.neck, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XorStr("Chest"), &g_Options.aimbot.chest, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XorStr("Stomach"), &g_Options.aimbot.stomach, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XorStr("Pelvis"), &g_Options.aimbot.pelvis, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XorStr("Arms"), &g_Options.aimbot.arms, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XorStr("Hands"), &g_Options.aimbot.hands, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XorStr("Legs"), &g_Options.aimbot.legs, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XorStr("Feet"), &g_Options.aimbot.feet, ImGuiSelectableFlags_DontClosePopups);

					ImGui::EndCombo();
				}

				spacing();
				ImGui::Checkbox(XorStr("No Aimbot If Spec"), &g_Options.aimbot.noaonspec);
				spacing();
				ImGui::Checkbox(XorStr("Backtrack"), &g_Options.aimbot.backtrack);
				spacing();
				ImGui::Checkbox(XorStr("Aim at Backtrack (Beta)"), &g_Options.aimbot.aimatbt);
				spacing();
				ImGui::Checkbox(XorStr("Silent"), &g_Options.aimbot.silent);
				if (g_Options.aimbot.silent) {
					sliderspacing();
					ImGui::SliderFloat(XorStr("Silent fov"), &g_Options.aimbot.silentfov, 0.f, 180.f, XorStr("%.f"));
				}
				spacing();
				ImGui::Checkbox(XorStr("Autofire"), &g_Options.aimbot.autofire); keybindspacing(); ImGui::Hotkey(XorStr("##afkey"), &g_Options.aimbot.afkey);
				if (g_Options.aimbot.enabled)
				{
					sliderspacing();
					ImGui::SliderInt(XorStr("Min Dmg"), &g_Options.aimbot.mindmg, 1, 100, XorStr("%i"));
				}
				spacing();
				ImGui::Checkbox(XorStr("Autowall"), &g_Options.aimbot.autowall);
				if (g_Options.aimbot.autowall) {
					sliderspacing();
					ImGui::SliderInt(XorStr("AWall Min Dmg"), &g_Options.aimbot.autowallmin, 1, 100, XorStr("%i"));
				}
				if (g_Options.aimbot.enabled || g_Options.aimbot.autowall)
				{
					sliderspacing();
					ImGui::SliderInt(XorStr("Dmg Override"), &g_Options.aimbot.dmgovrd, 1, 100, XorStr("%i"));
					spacing();
					ImGui::Text(XorStr("Override Key"));
					keybindspacing();
					ImGui::Hotkey(XorStr("                                                                                                                                                "), &g_Options.aimbot.dmgovrdk);
				}

				spacing();
				ImGui::Checkbox(XorStr("RCS"), &g_Options.aimbot.rcs);

				if (g_Options.aimbot.rcs) {
					sliderspacing();
					ImGui::SliderInt(XorStr("  X"), &g_Options.aimbot.x, 0, 100, XorStr("%i"));
					sliderspacing();
					ImGui::SliderInt(XorStr("  Y"), &g_Options.aimbot.y, 0, 100, XorStr("%i"));
				}
			}
			spacing();
			ImGui::EndChild();

			ImGui::SameLine(0, 21);

			ImGui::BeginChild("Rage / Other", ImVec2(254, 498)); {

				ImGui::Spacing();
				ImGui::Spacing();
				spacing();
				ImGui::Checkbox(XorStr("Hitchance"), &g_Options.aimbot.hc);
				if (g_Options.aimbot.hc)
				{
					sliderspacing();
					ImGui::SliderInt(XorStr("Hitchance %"), &g_Options.aimbot.hitchance, 1, 100, XorStr("%.f"));
					if (g_Options.aimbot.dt)
					{
						sliderspacing();
						ImGui::SliderInt(XorStr("DT Hitchance"), &g_Options.aimbot.dthc, 1, 100, XorStr("%.f"));
					}
				}
				spacing();
				ImGui::Checkbox(XorStr("Zeusbot"), &g_Options.aimbot.zeusbot);
				spacing();
				ImGui::Checkbox(XorStr("Safe Head"), &g_Options.aimbot.safehead);
				spacing();
				ImGui::Checkbox(XorStr("Hide Shots"), &g_Options.aimbot.hs); keybindspacing(); ImGui::Hotkey(XorStr("##hskey"), &g_Options.aimbot.hshotkey);
				spacing();
				ImGui::Checkbox(XorStr("Doubletap"), &g_Options.aimbot.dt); keybindspacing(); ImGui::Hotkey(XorStr("##dtkey"), &g_Options.aimbot.dthotkey);
				if (g_Options.aimbot.dt)
				{
					sliderspacing();
					ImGui::SliderInt(XorStr("DT Ticks"), &g_Options.aimbot.dtticks, 1, 14, XorStr("%i"));
				}
				spacing();
				ImGui::Checkbox(XorStr("Fast Aimbot"), &g_Options.aimbot.fastaimbot);
				spacing();
				ImGui::Checkbox(XorStr("Autorevolver"), &g_Options.aimbot.autorevolver1);
				spacing();
				ImGui::Checkbox(XorStr("Autostop"), &g_Options.aimbot.autostop);
				spacing();
				ImGui::Checkbox(XorStr("Resolver (Finally)"), &g_Options.aimbot.rageresolver);
				if (g_Options.aimbot.rageresolver)
				{
					spacing();
					ImGui::Checkbox(XorStr("Print Resolver Logs"), &g_Options.aimbot.printresolver);
				}
				spacing();
				ImGui::Checkbox(XorStr("Rage Aim"), &g_Options.aimbot.rageaim);
				spacing();
			}
			ImGui::EndChild();


		}

		//aa
		if (tab == 1) {
			ImGui::SetCursorPos(ImVec2(173, 50));
			ImGui::BeginChild("Antiaim", ImVec2(529, 498)); {

				ImGui::Spacing();
				ImGui::Spacing();
				spacing();

				const char* aa_pitch_list[] = {
XorStr("None"),
XorStr("Offset"),
XorStr("Down"),
XorStr("Up"),
XorStr("Zero"),
XorStr("Fake Up"),
XorStr("Fake Down"),
XorStr("Fake Zero")


				};

				const char* aa_yaw_list[] = {
XorStr("None"),
XorStr("Backwards"),
XorStr("Spin"),
XorStr("LBY"),
XorStr("Freestand"),
XorStr("Custom")
				};

				const char* desyncpresets[] = {
XorStr("None"),
XorStr("Low Delta"),
XorStr("Normal"),
XorStr("Custom"),
XorStr("Spin")
				};

				const char* indcpos[] = {
XorStr("None"),
XorStr("Origin"),
XorStr("Right Arm"),
XorStr("Left Arm"),
XorStr("Head")
				};

				const char* breakertype[] = {
XorStr("LBY"),
XorStr("LBY Reverse"),
XorStr("Auto LBY"),
XorStr("Balance"),
				};

				ImGui::Combo(XorStr("Pitch"), &g_Options.ragebot_antiaim_pitch, aa_pitch_list, IM_ARRAYSIZE(aa_pitch_list));
				//ImGui::Text(Misc::Get().readwebdata().c_str());
				spacing();
				ImGui::Combo(XorStr("Yaw"), &g_Options.ragebot_antiaim_yaw, aa_yaw_list, IM_ARRAYSIZE(aa_yaw_list));
				spacing();
				ImGui::Checkbox(XorStr("Desync"), &g_Options.ragebot_antiaim_desync);
				spacing();
				ImGui::Checkbox(XorStr("Body Breaker"), &g_Options.breaklby);
				if (g_Options.breaklby)
				{
					spacing();
					ImGui::Combo(XorStr("Breaker Type"), &g_Options.breakertp, breakertype, IM_ARRAYSIZE(breakertype));
				}
				if (g_Options.ragebot_antiaim_yaw == 2)
				{
					sliderspacing();
					ImGui::SliderInt(XorStr("Spin Speed"), &g_Options.spinspeed, 1.f, 10.f, XorStr("%.f"));
				}
				if (g_Options.ragebot_antiaim_yaw == 5)
				{
					sliderspacing();
					ImGui::SliderInt(XorStr("Real Degree"), &g_Options.customreal, -180.f, 180.f, XorStr("%.f"));
				}
				if (g_Options.ragebot_antiaim_yaw == 6)
				{
					sliderspacing();
					ImGui::SliderInt(XorStr("Jitter Degree"), &g_Options.jitterdegree, 0, 180.f, XorStr("%.f"));
				}

				if (g_Options.spinroll == false)
				{
					//sliderspacing();
					//ImGui::SliderFloat(XorStr("Real Roll"), &g_Options.realroll, -60.f, 60.f, XorStr("%.f"));
				}
				else
				{
					//sliderspacing();
					//ImGui::SliderInt(XorStr("Roll Spin Speed"), &g_Options.spinrollspeed, 1.f, 10.f, XorStr("%.f"));
				}
				//spacing();

				//ImGui::Checkbox(XorStr("Spin Roll"), &g_Options.spinroll);

				if (g_Options.ragebot_antiaim_desync)
				{
					if (g_Options.spinroll2 == false)
					{
						//sliderspacing();
						//ImGui::SliderFloat(XorStr("Fake Roll"), &g_Options.roll, -60.f, 60.f, XorStr("%.f"));
					}
					else
					{
						//sliderspacing();
						//ImGui::SliderInt(XorStr("Fake Roll Spin Speed"), &g_Options.spinroll2speed, 1.f, 10.f, XorStr("%.f"));
					}
					//spacing();
					//ImGui::Checkbox(XorStr("Spin Fake Roll"), &g_Options.spinroll2);
					spacing();
					ImGui::Combo(XorStr("Fake Preset"), &g_Options.fakepreset, desyncpresets, IM_ARRAYSIZE(desyncpresets));
					if (g_Options.fakepreset == 3)
					{
						sliderspacing();
						ImGui::SliderInt(XorStr("Fake Degree"), &g_Options.fakedegree, -180.f, 180.f, XorStr("%.f"));
					}
					if (g_Options.fakepreset == 4)
					{
						sliderspacing();
						ImGui::SliderInt(XorStr("Fake Spin Speed"), &g_Options.fakespinsp, 1.f, 10.f, XorStr("%.f"));
					}
					spacing();
					ImGui::Combo(XorStr("Indicator Pos"), &g_Options.indicatorhb, indcpos, IM_ARRAYSIZE(indcpos));
				}
				spacing();
				ImGui::Text(XorStr("Invert AA Key")); keybindspacing(); ImGui::Hotkey(XorStr("##invertkey"), &g_Options.invertaa);
				ImGui::Separator(XorStr("Others"));
				spacing();
				ImGui::Checkbox(XorStr("No Pitch on Land"), &g_Options.headthing);
				spacing();
				ImGui::Checkbox(XorStr("Randomize Fake"), &g_Options.randomizefake);
				spacing();
				ImGui::Checkbox(XorStr("No Packets on Shot"), &g_Options.nopacketonshot);
				spacing();
				ImGui::Checkbox(XorStr("Air Stuck"), &g_Options.urrstuck); keybindspacing(); ImGui::Hotkey(XorStr("##airstuckkey"), &g_Options.urrstuckkey);
				spacing();
				ImGui::Checkbox(XorStr("Fakeping"), &g_Options.fakeping);
				if (g_Options.fakeping) {
					sliderspacing();
					ImGui::SliderInt(XorStr("Ping"), &g_Options.fakepingzzz, 1.f, 1000.f, XorStr("%.f"));
					spacing();
					ImGui::Text(XorStr("On Key")); keybindspacing(); ImGui::Hotkey(XorStr("##fakepingkey"), &g_Options.fakepingkey);
				}
				spacing();
				ImGui::Checkbox(XorStr("Fakelag"), &g_Options.fakelag);

				if (g_Options.fakelag)
				{
					sliderspacing();
					ImGui::SliderInt(XorStr("Ticks"), &g_Options.faketicks, 1, 16, XorStr("%.f"));
					spacing();
					ImGui::Checkbox(XorStr("On Peek"), &g_Options.flonpeek);
					if (g_Options.flonpeek)
					{
						sliderspacing();
						ImGui::SliderInt(XorStr("OP Ticks"), &g_Options.flop, 1, 16, XorStr("%.f"));
					}
				}
				spacing();
				ImGui::Checkbox(XorStr("Slowwalk"), &g_Options.slowwalk); keybindspacing(); ImGui::Hotkey(XorStr("##slowwalkkey"), &g_Options.ragebot_slowwalk_key);
				if (g_Options.slowwalk) {
					sliderspacing();
					ImGui::SliderInt(XorStr("Speed"), &g_Options.ragebot_slowwalk_amt, 0, 100);
				}
				spacing();
				ImGui::Checkbox(XorStr("Fakeduck"), &g_Options.fakeduck); keybindspacing(); ImGui::Hotkey(XorStr("                                                                                                            "), &g_Options.fdkey);
				spacing();
				ImGui::Checkbox(XorStr("Slidewalk"), &g_Options.slidebruh);
				spacing();
			}
			ImGui::EndChild();


		}

		//esp
		if (tab == 2) {
			ImGui::SetCursorPos(ImVec2(173, 50));
			ImGui::BeginChild("ESP", ImVec2(254, 498)); {

				ImGui::Spacing();
				ImGui::Spacing();
				spacing();
				const char* boxtype[] = {
				XorStr("Normal"),
				XorStr("Corners")
				};

				ImGui::Checkbox(XorStr("Dormant ESP"), &g_Options.dormantesp);
				spacing();
				ImGui::Checkbox(XorStr("Far ESP"), &g_Options.faresp);
				spacing();
				ImGui::Checkbox(XorStr("Boxes"), &g_Options.esp_player_boxes); colspacing(); ImGuiEx::ColorEdit4a(XorStr("Enemies Visible   "), &g_Options.color_esp_enemy_visible);
				if (g_Options.esp_player_boxesOccluded) {
					colspacing2(); ImGuiEx::ColorEdit4a(XorStr("Enemies Occluded      "), &g_Options.color_esp_enemy_occluded);
				}
				spacing();
				if (g_Options.esp_player_boxes)
				{
					spacing();
					ImGui::Combo(XorStr("Style"), &g_Options.boxtype, boxtype, IM_ARRAYSIZE(boxtype));
					spacing();
				}
				ImGui::Checkbox(XorStr("Skeletons"), &g_Options.skeletonesp); colspacing(); ImGuiEx::ColorEdit4a(XorStr("Skeleton Color "), &g_Options.color_skemletom);
				spacing();
				if (g_Options.skeletonesp)
				{
					sliderspacing();
					ImGui::SliderFloat(XorStr("Thickness##skelthicc"), &g_Options.skelethicc, 0.1, 4);
					spacing();
				}
				ImGui::Checkbox(XorStr("Snaplines"), &g_Options.snaplines); colspacing(); ImGuiEx::ColorEdit4a(XorStr("Snapline Color "), &g_Options.color_cumming);
				if (g_Options.snaplines)
				{
					sliderspacing();
					ImGui::SliderFloat(XorStr("Thickness##snapthicc"), &g_Options.snapthicc, 0.1, 4);
				}
				spacing();
				ImGui::Checkbox(XorStr("Player Indicator "), &g_Options.playerind);	colspacing(); ImGuiEx::ColorEdit4(XorStr("Indicator Color "), &g_Options.color_indicator);
				spacing();
				ImGui::Checkbox(XorStr("Occluded "), &g_Options.esp_player_boxesOccluded);
				spacing();
				ImGui::Checkbox(XorStr("Names"), &g_Options.esp_player_names); colspacing(); ImGuiEx::ColorEdit4a(XorStr("Names Color "), &g_Options.color_name_player);
				spacing();
				if (g_Options.aimbot.backtrack)
				{
					ImGui::Checkbox(XorStr("Backtrack"), &g_Options.showbt); colspacing(); ImGuiEx::ColorEdit4a(XorStr("Backtrack Color "), &g_Options.color_backtrackind);
					spacing();
				}
				ImGui::Checkbox(XorStr("Health"), &g_Options.esp_player_health);
				spacing();
				ImGui::Checkbox(XorStr("Armor"), &g_Options.esp_player_armour); colspacing(); ImGuiEx::ColorEdit4a(XorStr("Armor Color "), &g_Options.color_armour_player);
				spacing();
				ImGui::Checkbox(XorStr("Weapons"), &g_Options.esp_player_weapons); colspacing(); ImGuiEx::ColorEdit4a(XorStr("Weapons Color "), &g_Options.color_esp_weapons);
				spacing();
				ImGui::Checkbox(XorStr("Dropped Weapons"), &g_Options.esp_dropped_weapons);
				spacing();
				ImGui::Checkbox(XorStr("Flash Kill Check"), &g_Options.flashkillcheck);
				spacing();
			}
			ImGui::EndChild();

			ImGui::SameLine(0, 21);

			ImGui::SetCursorPos(ImVec2(448, 50));
			ImGui::BeginChild("Others", ImVec2(254, 498)); {

				ImGui::Spacing();
				ImGui::Spacing();
				spacing();
				ImGui::Checkbox(XorStr("FOV In Scope"), &g_Options.fovscope);
				spacing();
				if (g_Options.fovscope)
				{
					ImGui::Checkbox(XorStr("Viewmodel In Scope"), &g_Options.viewmodelinscope);
					spacing();
				}
				ImGui::Checkbox(XorStr("Draw FOV"), &g_Options.drawfov);
				spacing();
				ImGui::Checkbox(XorStr("Force Crosshair"), &g_Options.sniper_xhair);
				spacing();
				ImGui::Checkbox(XorStr("No smoke"), &g_Options.no_smoke);
				spacing();
				ImGui::Checkbox(XorStr("No flash"), &g_Options.no_flash);
				spacing();
				ImGui::Checkbox(XorStr("Floating Ragdolls"), &g_Options.ragfloat);
				spacing();
				ImGui::Checkbox(XorStr("Recoil Crosshair"), &g_Options.rcross);
				spacing();
				ImGui::Checkbox(XorStr("Grenade Prediction"), &g_Options.pnade);
				spacing();
				ImGui::Checkbox(XorStr("Thirdperson"), &g_Options.misc_thirdperson); keybindspacing(); ImGui::Hotkey(XorStr("##thirdpersonkey"), &g_Options.misc_thirdperson_key);
				if (g_Options.misc_thirdperson)
				{
					spacing();
					ImGui::Checkbox(XorStr("Thirdperson Spectate"), &g_Options.thirdinspec);
				}
				sliderspacing();
				ImGui::SliderInt(XorStr("Fov"), &g_Options.fovchangaaa, 0, 30, XorStr("%.f"));
				spacing();
				ImGui::Checkbox(XorStr("Aspect Ratio"), &g_Options.aspectchange);
				if (g_Options.aspectchange) {
					sliderspacing();
					ImGui::SliderFloat(XorStr("Ratio"), &g_Options.aspectratio, 0.10, 3);
				};
				sliderspacing();
				ImGui::SliderFloat(XorStr("Viewmodel Fov"), &g_Options.viewmodel_fov, 0, 89);
				sliderspacing();
				ImGui::SliderFloat(XorStr("X"), &g_Options.viewmodel_offset_x, -50.0, 50.0);
				sliderspacing();
				ImGui::SliderFloat(XorStr("Y"), &g_Options.viewmodel_offset_y, -50.0, 50.0);
				sliderspacing();
				ImGui::SliderFloat(XorStr("Z"), &g_Options.viewmodel_offset_z, -50.0, 50.0);
				sliderspacing();
				ImGui::SliderFloat(XorStr("Roll"), &g_Options.viewmodel_offset_roll, -180, 180);
				spacing();

			}
			ImGui::EndChild();

		}

		//chams
		if (tab == 4) {
			ImGui::SetCursorPos(ImVec2(173, 50));
			ImGui::BeginChild("Chams", ImVec2(254, 498)); {

				ImGui::Spacing();
				ImGui::Spacing();
				spacing();
				ImGui::Checkbox(XorStr("Enabled "), &g_Options.chams_player_enabled); if (g_Options.chams_player_enabled) {
					colspacing(); ImGuiEx::ColorEdit4a(XorStr("Enemy Visible "), &g_Options.color_chams_player_enemy_visible);
					if (g_Options.chams_player_ignorez)
					{
						colspacing2(); ImGuiEx::ColorEdit4a(XorStr("Enemy Occluded "), &g_Options.color_chams_player_enemy_occluded);
					}
					if (g_Options.player_material >= 4) {
						colspacing3(); ImGuiEx::ColorEdit4a(XorStr("Double Color Enemies"), &g_Options.glowcolorenemy);
					}
					spacing();
					ImGui::Checkbox(XorStr("Occluded"), &g_Options.chams_player_ignorez);
					spacing();
					ImGui::Checkbox(XorStr("Team Chams"), &g_Options.teamchams); colspacing(); ImGuiEx::ColorEdit4a(XorStr("Team Visible "), &g_Options.color_chams_player_ally_visible);
					if (g_Options.chams_player_ignorez)
					{
						colspacing2(); ImGuiEx::ColorEdit4a(XorStr("Team Occluded "), &g_Options.color_chams_player_ally_occluded);
					}
					if (g_Options.player_material >= 4) {
						colspacing3(); ImGuiEx::ColorEdit4a(XorStr("Double Color Team"), &g_Options.glowcolor);
					}
					spacing();
					ImGui::Checkbox(XorStr("Local Chams"), &g_Options.localchams); colspacing(); ImGuiEx::ColorEdit4a(XorStr("Local Visible "), &g_Options.color_chams_player_local_visible);
					if (g_Options.chams_player_ignorez)
					{
						colspacing2(); ImGuiEx::ColorEdit4a(XorStr("Local Occluded "), &g_Options.color_chams_player_local_occluded);
					}
					if (g_Options.player_material >= 4) {
						colspacing3(); ImGuiEx::ColorEdit4a(XorStr("Double Color Local"), &g_Options.glowcolorlocal);
					}
					spacing();
					if (g_Options.ragebot_antiaim_desync) {
						ImGui::Checkbox(XorStr("Desync Chams [BETA] "), &g_Options.chams_fake_enabled); colspacing(); ImGuiEx::ColorEdit4a(XorStr("Fake Chams "), &g_Options.color_chams_fake);
						spacing();
					}
					ImGui::Checkbox(XorStr("Arm Chams"), &g_Options.chams_arms_enabled); colspacing(); ImGuiEx::ColorEdit4a(XorStr("Arms "), &g_Options.color_chams_arms_visible);
					if (g_Options.arms_material >= 3 && g_Options.arms_material != 5) {
						colspacing2(); ImGuiEx::ColorEdit4a(XorStr("Double Arms"), &g_Options.glowcolorarms);
					}
					spacing();
					ImGui::Checkbox(XorStr("Strap Chams"), &g_Options.chams_strap_enabled); colspacing(); ImGuiEx::ColorEdit4a(XorStr("Strap "), &g_Options.color_chams_strap_visible);
					if (g_Options.strap_material >= 3 && g_Options.strap_material != 5) {
						colspacing2(); ImGuiEx::ColorEdit4a(XorStr("Double Strap"), &g_Options.glowcolorstrap);
					}
					spacing();
					//ImGui::Checkbox(XorStr("Pearlecent"), &g_Options.pearlescent);
					//spacing();
					ImGui::Checkbox(XorStr("Draw OG Mdl"), &g_Options.drawogplaya);

				}
				spacing();

				const char* MatList[] = {
XorStr("Textured"),
XorStr("Flat"),
XorStr("Shine"),
XorStr("Velvet"),
XorStr("Glow"),
XorStr("Double")
				};

				const char* MatList3[] = {
XorStr("Textured"),
XorStr("Flat"),
XorStr("Shine"),
XorStr("Glow"),
XorStr("Double"),
XorStr("Velvet")
				};

				const char* MatList2[] = {
XorStr("Textured"),
XorStr("Flat"),
XorStr("Shine"),
XorStr("Glow"),
XorStr("Double"),
XorStr("Velvet"),
XorStr("AdvancedAim [MIGI]")
				};

				if (g_Options.chams_player_enabled)
				{
					ImGui::Combo(XorStr("Player Mat"), &g_Options.player_material, MatList, IM_ARRAYSIZE(MatList));
					spacing();
				}

				if (g_Options.chams_strap_enabled)
				{
					ImGui::Combo(XorStr("Strap Mat"), &g_Options.strap_material, MatList2, IM_ARRAYSIZE(MatList2));
					spacing();
				}

				if (g_Options.chams_arms_enabled)
				{
					ImGui::Combo(XorStr("Arm Mat"), &g_Options.arms_material, MatList3, IM_ARRAYSIZE(MatList3));
					spacing();
				}

				spacing();
			}
			ImGui::EndChild();

			ImGui::SameLine(0, 21);
		}

		//world
		if (tab == 5) {
			ImGui::SetCursorPos(ImVec2(173, 50));
			ImGui::BeginChild("World", ImVec2(254, 498)); {

				ImGui::Spacing();
				ImGui::Spacing();
				spacing();
				if (ImGui::BeginCombo(XorStr("##removals"), XorStr("Removals")))
				{
					ImGui::Selectable(XorStr("Recoil"), &g_Options.fatassmf, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XorStr("Processing"), &g_Options.disablepro, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XorStr("Blur"), &g_Options.removeblur, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XorStr("Scope"), &g_Options.noscope, ImGuiSelectableFlags_DontClosePopups);

					ImGui::EndCombo();
				}
				spacing();
				ImGui::Checkbox(XorStr("Matrix Mode"), &g_Options.matrix);
				spacing();
				ImGui::Checkbox(XorStr("Metallic Mode"), &g_Options.metallica);
				spacing();
				ImGui::Checkbox(XorStr("Fullbright"), &g_Options.fullbright);
				spacing();
				ImGui::Checkbox(XorStr("Sky Changer"), &g_Options.sky_changer);
				if (!g_Options.sky_changer) {
					sliderspacing();
				}
				const char* SkyList[] = {
XorStr("Night"),
XorStr("Baggage"),
XorStr("Tibet"),
XorStr("Vietnam"),
XorStr("Lunacy"),
XorStr("Embassy"),
XorStr("Italy"),
XorStr("Jungle"),
XorStr("Office"),
XorStr("Day"),
XorStr("Cloudy"),
XorStr("Dust")
				};
				if (g_Options.sky_changer) {
					spacing();
					ImGui::Combo(XorStr("Sky"), &g_Options.skyshitsssss, SkyList, IM_ARRAYSIZE(SkyList));
					sliderspacing();
				}

				ImGui::SliderFloat(XorStr("Model Ambience"), &g_Options.amibence, 0, 1500);
				spacing();
				ImGui::Checkbox(XorStr("Color Modulation"), &g_Options.colormodulate);
				if (g_Options.colormodulate) {
					spacing();
					ImGui::Text(XorStr("World Color"));
					ImGui::SameLine();
					ImGuiEx::ColorEdit3(XorStr("World Color"), &g_Options.nightmodecolor);
					spacing();
					ImGui::Checkbox(XorStr("Prop Modulation"), &g_Options.propstoo);
					if (g_Options.propstoo) {
						spacing();
						ImGui::Text(XorStr("Prop Color"));
						ImGui::SameLine();
						ImGuiEx::ColorEdit4a(XorStr("Prop Color"), &g_Options.propcolor);
					}
				}
				spacing();
				ImGui::Text(XorStr("World Glow"));
				ImGui::SameLine();
				ImGuiEx::ColorEdit3(XorStr("World Glow"), &g_Options.color_worldglow);
				spacing();
				ImGui::Separator(XorStr("Fog"));
				spacing();
				ImGui::Checkbox(XorStr("Override Fog"), &g_Options.fogoverride);
				sliderspacing();
				ImGui::SliderInt(XorStr("Distance"), &g_Options.fogfardamn, 0, 2500);
				sliderspacing();
				ImGui::SliderInt(XorStr("Thickness"), &g_Options.fogdens, 0, 100);
				spacing();
				ImGui::Text(XorStr("Fog Color"));
				ImGui::SameLine();
				ImGuiEx::ColorEdit3(XorStr("Fog Color"), &g_Options.fogcoluh);
				spacing();
			}
			ImGui::EndChild();
		}

	if (tab == 6) {
		static std::string selected_weapon_name = "";
		static std::string selected_skin_name = "";
		static auto definition_vector_index = 0;
		auto& entries = g_Options.changers.skin.m_items;
			auto& selected_entry = entries[k_weapon_names[definition_vector_index].definition_index];
			auto& satatt = g_Options.changers.skin.statrack_items[k_weapon_names[definition_vector_index].definition_index];
			selected_entry.definition_index = k_weapon_names[definition_vector_index].definition_index;
			selected_entry.definition_vector_index = definition_vector_index;



			ImGui::SetCursorPos(ImVec2(173, 50));
			ImGui::BeginChild("Weapon", ImVec2(254, 498)); {

				ImGui::Spacing();

				for (size_t w = 0; w < k_weapon_names.size(); w++) {
					if (ImGui::Selectable(k_weapon_names[w].name, definition_vector_index == w)) {
						definition_vector_index = w;
					}
				}

			}
			ImGui::EndChild();

			ImGui::SameLine(0, 21);

			ImGui::BeginChild("Colors", ImVec2(254, 498)); {
				ImGui::Spacing();
				ImGui::Spacing();
				spacing();
				ImGui::Checkbox(("Enabled"), &selected_entry.enabled);
				spacing();
				ImGui::InputInt(("Seed"), &selected_entry.seed);
				sliderspacing();
				ImGui::SliderFloat(("Wear:"), &selected_entry.wear, FLT_MIN, 100.f, "%.10f", 5);
				spacing();


				ImGui::Spacing();
				spacing();

				auto& selected_entry = entries[k_weapon_names[definition_vector_index].definition_index];
				auto& satatt = g_Options.changers.skin.statrack_items[k_weapon_names[definition_vector_index].definition_index];
				selected_entry.definition_index = k_weapon_names[definition_vector_index].definition_index;
				selected_entry.definition_vector_index = definition_vector_index;
				if (selected_entry.definition_index == WEAPON_KNIFE || selected_entry.definition_index == WEAPON_KNIFE_T)
				{
					ImGui::PushItemWidth(160.f);

					ImGui::Combo("", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text)
						{
							*out_text = k_knife_names.at(idx).name;
							return true;
						}, nullptr, k_knife_names.size(), 10);
					selected_entry.definition_override_index = k_knife_names.at(selected_entry.definition_override_vector_index).definition_index;

				}
				else if (selected_entry.definition_index == GLOVE_T_SIDE || selected_entry.definition_index == GLOVE_CT_SIDE)
				{
					ImGui::PushItemWidth(160.f);

					ImGui::Combo("", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text)
						{
							*out_text = k_glove_names.at(idx).name;
							return true;
						}, nullptr, k_glove_names.size(), 10);
					selected_entry.definition_override_index = k_glove_names.at(selected_entry.definition_override_vector_index).definition_index;
				}
				else {
					static auto unused_value = 0;
					selected_entry.definition_override_vector_index = 0;
				}

				if (selected_entry.definition_index != GLOVE_T_SIDE &&
					selected_entry.definition_index != GLOVE_CT_SIDE &&
					selected_entry.definition_index != WEAPON_KNIFE &&
					selected_entry.definition_index != WEAPON_KNIFE_T)
				{
					selected_weapon_name = k_weapon_names_preview[definition_vector_index].name;
				}
				else
				{
					if (selected_entry.definition_index == GLOVE_T_SIDE ||
						selected_entry.definition_index == GLOVE_CT_SIDE)
					{
						selected_weapon_name = k_glove_names_preview.at(selected_entry.definition_override_vector_index).name;
					}
					if (selected_entry.definition_index == WEAPON_KNIFE ||
						selected_entry.definition_index == WEAPON_KNIFE_T)
					{
						selected_weapon_name = k_knife_names_preview.at(selected_entry.definition_override_vector_index).name;
					}
				}
				if (skins_parsed)
				{
					static char filter_name[32];
					std::string filter = filter_name;

					bool is_glove = selected_entry.definition_index == GLOVE_T_SIDE ||
						selected_entry.definition_index == GLOVE_CT_SIDE;

					bool is_knife = selected_entry.definition_index == WEAPON_KNIFE ||
						selected_entry.definition_index == WEAPON_KNIFE_T;

					int cur_weapidx = 0;
					if (!is_glove && !is_knife)
					{
						cur_weapidx = k_weapon_names[definition_vector_index].definition_index;
						//selected_weapon_name = k_weapon_names_preview[definition_vector_index].name;
					}
					else
					{
						if (selected_entry.definition_index == GLOVE_T_SIDE ||
							selected_entry.definition_index == GLOVE_CT_SIDE)
						{
							cur_weapidx = k_glove_names.at(selected_entry.definition_override_vector_index).definition_index;
						}
						if (selected_entry.definition_index == WEAPON_KNIFE ||
							selected_entry.definition_index == WEAPON_KNIFE_T)
						{
							cur_weapidx = k_knife_names.at(selected_entry.definition_override_vector_index).definition_index;

						}
					}



					auto weaponName = weaponnames(cur_weapidx);

					{
						if (selected_entry.definition_index != GLOVE_T_SIDE && selected_entry.definition_index != GLOVE_CT_SIDE)
						{
							if (ImGui::Selectable(" - ", selected_entry.paint_kit_index == -1))
							{
								selected_entry.paint_kit_vector_index = -1;
								selected_entry.paint_kit_index = -1;
								selected_skin_name = "";
							}

							int lastID = ImGui::GetItemID();
							for (size_t w = 0; w < k_skins.size(); w++)
							{
								for (auto names : k_skins[w].weaponName)
								{
									std::string name = k_skins[w].name;

									if (g_Options.changers.skin.show_cur)
									{
										if (names != weaponName)
											continue;
									}

									if (name.find(filter) != name.npos)
									{
										ImGui::PushID(lastID++);

										ImGui::PushStyleColor(ImGuiCol_Text, skins::get_color_ratiry(is_knife && g_Options.changers.skin.show_cur ? 6 : k_skins[w].rarity));
										{
											if (ImGui::Selectable(name.c_str(), selected_entry.paint_kit_vector_index == w))
											{
												selected_entry.paint_kit_vector_index = w;
												selected_entry.paint_kit_index = k_skins[selected_entry.paint_kit_vector_index].id;
												selected_skin_name = k_skins[w].name_short;
											}
										}
										ImGui::PopStyleColor();

										ImGui::PopID();
									}
								}
							}
						}
						else
						{
							int lastID = ImGui::GetItemID();

							if (ImGui::Selectable(" - ", selected_entry.paint_kit_index == -1))
							{
								selected_entry.paint_kit_vector_index = -1;
								selected_entry.paint_kit_index = -1;
								selected_skin_name = "";
							}

							for (size_t w = 0; w < k_gloves.size(); w++)
							{
								for (auto names : k_gloves[w].weaponName)
								{
									std::string name = k_gloves[w].name;


									if (g_Options.changers.skin.show_cur)
									{
										if (names != weaponName)
											continue;
									}

									if (name.find(filter) != name.npos)
									{
										ImGui::PushID(lastID++);

										ImGui::PushStyleColor(ImGuiCol_Text, skins::get_color_ratiry(6));
										{
											if (ImGui::Selectable(name.c_str(), selected_entry.paint_kit_vector_index == w))
											{
												selected_entry.paint_kit_vector_index = w;
												selected_entry.paint_kit_index = k_gloves[selected_entry.paint_kit_vector_index].id;
												selected_skin_name = k_gloves[selected_entry.paint_kit_vector_index].name_short;
											}
										}
										ImGui::PopStyleColor();

										ImGui::PopID();
									}
								}
							}
						}
					}
					//	ImGui::ListBoxFooter();
				}
				else
				{
					ImGui::Text("skins parsing, wait...");
				};
			}
				ImGui::EndChild();
		}

		//misc
		if (tab == 3) {
			ImGui::SetCursorPos(ImVec2(173, 50));
			ImGui::BeginChild("Configs", ImVec2(254, 193)); {

				ImGui::Spacing();
				ImGui::Spacing();
				spacing();
				static int selected = 0;
				static char cfgName[64];

				std::vector<std::string> cfgList;
				ReadDirectory(g_Options.folder, cfgList);
				ImGui::PushItemWidth(150.f);
				if (!cfgList.empty())
				{
					if (ImGui::BeginCombo(XorStr("##SelectConfig"), cfgList[selected].c_str(), ImGuiComboFlags_NoArrowButton))
					{
						for (size_t i = 0; i < cfgList.size(); i++)
						{
							if (ImGui::Selectable(cfgList[i].c_str(), i == selected))
								selected = i;
						}
						ImGui::EndCombo();

					}
					spacing();
					if (ImGui::Button(XorStr("Save Config")))
					{
						g_Options.SaveSettings(cfgList[selected]);
					}
					spacing();
					if (ImGui::Button(XorStr("Load Config")))
					{
						g_Options.LoadSettings(cfgList[selected]);
					}
					spacing();
					if (ImGui::Button(XorStr("Delete Config")))
					{
						g_Options.DeleteSettings(cfgList[selected]);
						selected = 0;
					}
					spacing();
				}
				ImGui::InputText(XorStr("##configname"), cfgName, 24);
				spacing();
				if (ImGui::Button(XorStr("Create Config")))
				{
					if (strlen(cfgName))
						g_Options.SaveSettings(cfgName + std::string(XorStr(".cfg")));
				}
				spacing();
				ImGui::PopItemWidth();
			}
			ImGui::EndChild();


			ImGui::SetCursorPos(ImVec2(448, 50));
			ImGui::BeginChild("Misc", ImVec2(254, 498)); {
				spacing();
				int screenx;
				int screeny;
				g_EngineClient->GetScreenSize(screenx, screeny);

				//NOT COOL!!!
				ImGui::Checkbox(XorStr("No Duck Cooldown"), &g_Options.nocool);
				spacing();
				ImGui::Checkbox(XorStr("Bhop"), &g_Options.misc_bhop);
				spacing();
				const char* Autostrafe[] = {
XorStr("Normal"),
XorStr("Directional")
				};
				ImGui::Checkbox(XorStr("Autostrafe"), &g_Options.autostrafe); keybindspacing(); ImGui::Hotkey(XorStr("##strafekey"), &g_Options.AutoStafe_key);
				spacing();
				if (g_Options.autostrafe) {
					ImGui::Combo(XorStr("Type"), &g_Options.autostrafemode, Autostrafe, IM_ARRAYSIZE(Autostrafe));
					spacing();
				}
				ImGui::Checkbox(XorStr("Edgebug"), &g_Options.edge_bug); keybindspacing();          ImGui::Hotkey(XorStr("##ebkey"), &g_Options.edge_bug_key);
				spacing();
				ImGui::Checkbox(XorStr("Jumpbug"), &g_Options.jump_bug); keybindspacing();          ImGui::Hotkey(XorStr("##jbkey"), &g_Options.jump_bug_key);
				spacing();
				ImGui::Checkbox(XorStr("Edge jump"), &g_Options.edgejump.enabled); keybindspacing(); ImGui::Hotkey(XorStr("##ejkey"), &g_Options.edgejump.hotkey);
				spacing();
				ImGui::Checkbox(XorStr("Long jump"), &g_Options.longjump); keybindspacing(); ImGui::Hotkey(XorStr("##ljkey"), &g_Options.ljkey);
				spacing();
				ImGui::Checkbox(XorStr("Blockbot"), &g_Options.blockbot); keybindspacing(); ImGui::Hotkey(XorStr("##blockbotkey"), &g_Options.bbkey);
				spacing();
				ImGui::Checkbox(XorStr("Duck In Air"), &g_Options.ducknair); keybindspacing(); ImGui::Hotkey(XorStr("##dairkey"), &g_Options.diarkey);
				spacing();
				ImGui::Checkbox(XorStr("Spectator List"), &g_Options.speclist);
				if (g_Options.speclist)
				{
					sliderspacing();
					ImGui::SliderInt(XorStr("X"), &g_Options.specx, 1, screenx);
					sliderspacing();
					ImGui::SliderInt(XorStr("Y"), &g_Options.specy, 1, screeny);
				}
				spacing();
				ImGui::Checkbox(XorStr("Quick Stop"), &g_Options.quickstop);
				spacing();
				ImGui::Checkbox(XorStr("Preserve Killfeed"), &g_Options.drugs);
				spacing();
				ImGui::Separator(XorStr("Movement Recorder"));
				spacing();
				ImGui::Text(XorStr("Record Movement")); keybindspacing(); ImGui::Hotkey(XorStr("##recordahh"), &g_Options.movementrecord);
				spacing();
				ImGui::Text(XorStr("Play Movement")); keybindspacing(); ImGui::Hotkey(XorStr("##playaahh"), &g_Options.movementplay);
				spacing();
				ImGui::Checkbox(XorStr("Force VAngles"), &g_Options.forceangles);
				spacing();
				if (ImGui::Button(XorStr("Refresh")))
					movement_recorder::Get().load_records();
				spacing();

			}
			ImGui::EndChild();

			ImGui::SetCursorPos(ImVec2(173, 278));
			ImGui::BeginChild("Others", ImVec2(254, 270)); {
				spacing();
				static char newname[64];
				//Ha penis
				ImGui::Checkbox(XorStr("Watermark"), &g_Options.misc_watermark);
				spacing();
				//Fast Shit Doe
				ImGui::Checkbox(XorStr("AntiOBS"), &g_Options.antiobs);
				spacing();
				ImGui::Checkbox(XorStr("Hitsounds"), &g_Options.misc_hitmarker);
				spacing();
				const char* HitList[] = {
XorStr("Metallic"),
	XorStr("Agree"),
	XorStr("Disagree"),
	XorStr("Electric"),
	XorStr("FranzJ"),
	XorStr("Spark"),
	XorStr("Gmod"),
	XorStr("Found Game")
				};
				if (g_Options.misc_hitmarker) {
					ImGui::Combo(XorStr("Sound"), &g_Options.hitmarkersound, HitList, IM_ARRAYSIZE(HitList));
					spacing();
				}
				ImGui::Checkbox(XorStr("Velocity"), &g_Options.Velocity);
				ImGui::SameLine();
				ImGuiEx::ColorEdit4(XorStr("Velocity Color"), &g_Options.Velocitycol);
				spacing();
				if (ImGui::BeginCombo(XorStr("##velocity_filter"), XorStr("Velocity")))
				{
					ImGui::Selectable(XorStr("Outline"), &g_Options.outline, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XorStr("Last jump"), &g_Options.lastjump, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XorStr("Last jump outline"), &g_Options.lastjumpoutline, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XorStr("Graph"), &g_Options.velgraphline, ImGuiSelectableFlags_DontClosePopups);

					ImGui::EndCombo();
				}
				spacing();

				//Misc Man Shit FRFR
				const char* ClantagTypes[] = {
XorStr("Animated"),
	XorStr("Discord"),
	XorStr("Static"),
	XorStr("Wrong"),
	XorStr("Fuck"),
	XorStr("Big Fuck"),
	XorStr("Email Me")
				};
				const char* ChatSpamType[] = {
XorStr("Guga Rep"),
	XorStr("BLM")
				};
				ImGui::Checkbox(XorStr("Chat Spam"), &g_Options.misc_chatspam);
				spacing();
				if (g_Options.misc_chatspam)
				{
					ImGui::Combo(XorStr("Type##chatspam"), &g_Options.chatspamtype, ChatSpamType, IM_ARRAYSIZE(ChatSpamType));
					spacing();
				}
				ImGui::Checkbox(XorStr("Clantag"), &g_Options.clantag);
				spacing();
				if (g_Options.clantag) {
					ImGui::Combo(XorStr("Type##clantag"), &g_Options.clantagtype, ClantagTypes, IM_ARRAYSIZE(ClantagTypes));
					spacing();
				}
				ImGui::Checkbox(XorStr("Rank Reveal"), &g_Options.rankreveal);
				spacing();
				ImGui::Checkbox(XorStr("Auto Accept"), &g_Options.autoaccept);
				spacing();
				if (ImGui::Button(XorStr("Change Name"))) {
					//NameChange
					void NameChange(); {
						static bool changed_name = false;
						static float name = 0;
						if (g_Options.namec != name) {
							changed_name = false;
						}
						if (!changed_name) {
							switch (g_Options.namec) {
							case 0:
								Utils::SetName(newname);
								break;
							}
							changed_name = false;
							name = g_Options.namec;
						}
					};
				}
				spacing();
				ImGui::InputText(XorStr("##namec"), newname, 1000);
				spacing();
				ImGui::Separator(XorStr("Accents"));
				spacing();
				ImGui::Text(XorStr("Menu Accents")); ImGui::SameLine(); ImGuiEx::ColorEdit4(XorStr("Menu Accents"), &g_Options.menucolor);
				spacing();
				ImGui::Separator(XorStr("Playermodel"));
				ImGui::Text(XorStr("   T Player Model"));
				spacing();
				ImGui::Combo(XorStr("##TPlayerModel"), &g_Options.playerModelT, XorStr("Default\0Cmdr. Goggles\0Cmdr. Wet Sox\0Lieutenant Rex Krikey\0Michael Syfers\0Operator\0Special Agent Ava\0Markus Delrow\0Sous-Lieutenant Medic\0Chem-Haz Capitaine\0Chef d'Escadron Rouchard\0Aspirant\0Officer Jacques Beltram\0D Squadron Officer\0B Squadron Officer\0Seal Team 6 Soldier\0Buckshot\0Lt. Commander Ricksaw\0Buckshot\0Commando Company\0Two Times' McCoy\0Two Times' McCoy2\0Primeiro Tenente\0Cmdr. Mae 'Dead Cold' Jamison\0Lieutenant Farlow\0John 'Van Healen' Kask\0Bio-Haz Specialist\0Sergeant Bombson\0Chem-Haz Specialist\0Getaway Sally\0Number K\0Little Kev\0Safecracker Voltzmann\0Bloody Darryl The Strapped\0Sir Bloody Loudmouth Darryl\0Sir Bloody Darryl Royale\0Sir Bloody Skullhead Darryl\0Sir Bloody Silent Darryl\0Sir Bloody Miami Darryl\0Street Soldier\0Soldier\0Slingshot\0Enforcer\0Mr. Muhlik\0Prof. Shahmat\0Osiris\0Ground Rebel\0The Elite Mr. Muhlik\0Trapper\0Trapper Aggressor\0Vypa Sista of the Revolution\0Col. Mangos Dabisi\0'Medium Rare' Crasswater\0Crasswater The Forgotten\0Elite Trapper Solman\0'The Doctor' Romanov\0Blackwolf\0Maximus\0Dragomir\0Rezan The Ready\0Rezan the Redshirt\0Dragomir\0"));
				spacing();
				ImGui::Text(XorStr("   CT Player Model"));
				spacing();
				ImGui::Combo(XorStr("##CTPlayerModel"), &g_Options.playerModelCT, XorStr("Default\0Cmdr. Goggles\0Cmdr. Wet Sox\0Lieutenant Rex Krikey\0Michael Syfers\0Operator\0Special Agent Ava\0Markus Delrow\0Sous-Lieutenant Medic\0Chem-Haz Capitaine\0Chef d'Escadron Rouchard\0Aspirant\0Officer Jacques Beltram\0D Squadron Officer\0B Squadron Officer\0Seal Team 6 Soldier\0Buckshot\0Lt. Commander Ricksaw\0Buckshot\0Commando Company\0Two Times' McCoy\0Two Times' McCoy2\0Primeiro Tenente\0Cmdr. Mae 'Dead Cold' Jamison\0Lieutenant Farlow\0John 'Van Healen' Kask\0Bio-Haz Specialist\0Sergeant Bombson\0Chem-Haz Specialist\0Getaway Sally\0Number K\0Little Kev\0Safecracker Voltzmann\0Bloody Darryl The Strapped\0Sir Bloody Loudmouth Darryl\0Sir Bloody Darryl Royale\0Sir Bloody Skullhead Darryl\0Sir Bloody Silent Darryl\0Sir Bloody Miami Darryl\0Street Soldier\0Soldier\0Slingshot\0Enforcer\0Mr. Muhlik\0Prof. Shahmat\0Osiris\0Ground Rebel\0The Elite Mr. Muhlik\0Trapper\0Trapper Aggressor\0Vypa Sista of the Revolution\0Col. Mangos Dabisi\0'Medium Rare' Crasswater\0Crasswater The Forgotten\0Elite Trapper Solman\0'The Doctor' Romanov\0Blackwolf\0Maximus\0Dragomir\0Rezan The Ready\0Rezan the Redshirt\0Dragomir\0"));
				spacing();
				//ImGui::Separator(XorStr("Test"));
				//spacing();
				//ImGui::Checkbox(XorStr("Test Features"), &g_Options.enabletest);
				//spacing();
				//if (g_Options.enabletest)
				//{
					
					//spacing();
				//}

			}
			ImGui::EndChild();
		}
		ImGui::PopStyleVar(ImGuiStyleVar_Alpha);
	}
	ImGui::End();
}


void Menu::Toggle()
{
	_visible = !_visible;
}

void Menu::CreateStyle()
{

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.900000f, 0.900000f, 0.900000f, 1.000000f));
	ImGui::PushStyleColor(ImGuiCol_TextDisabled, ImVec4(0.600000f, 0.600000f, 0.600000f, 1.000000f));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.000000f, 0.000000f, 0.000000f, 0.4f));
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.000000f, 0.000000f, 0.000000f, 0.000000f));
	ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.110000f, 0.110000f, 0.110000f, 0.920000f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.500000f, 0.500000f, 0.500000f, 0.500000f));
	ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0.000000f, 0.000000f, 0.000000f, 0.000000f));
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.430000f, 0.430000f, 0.430000f, 0.390000f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.470000f, 0.470000f, 0.470000f, 0.400000f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.40000f, 0.40000f, 0.40000f, 0.690000f));
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.270000f, 0.270000f, 0.540000f, 0.830000f));
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.320000f, 0.320000f, 0.630000f, 0.870000f));
	ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, ImVec4(0.400000f, 0.400000f, 0.800000f, 0.200000f));
	ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.400000f, 0.400000f, 0.550000f, 0.800000f));
	ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, ImVec4(0.200000f, 0.250000f, 0.300000f, 0.00000f));
	ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, ImVec4(0.400000f, 0.400000f, 0.800000f, 0.00000f));
	ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, ImVec4(0.400000f, 0.400000f, 0.800000f, 0.00000f));
	ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, ImVec4(0.410000f, 0.390000f, 0.800000f, 0.00000f));
	ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.900000f, 0.900000f, 0.900000f, 0.500000f));
	ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(1.000000f, 1.000000f, 1.000000f, 0.300000f));
	ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(0.410000f, 0.390000f, 0.800000f, 0.600000f));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.350000f, 0.400000f, 0.610000f, 0.0000f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.400000f, 0.480000f, 0.710000f, 0.0000f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.460000f, 0.540000f, 0.800000f, 0.000000f));
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.400000f, 0.400000f, 0.400000f, 0.450000f));
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.400000f, 0.400000f, 0.400000f, 0.800000f));
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.400000f, 0.400000f, 0.400000f, 0.800000f));
	ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(g_Options.menucolor.r(), g_Options.menucolor.g(), g_Options.menucolor.b(), 255));
	ImGui::PushStyleColor(ImGuiCol_SeparatorHovered, ImVec4(g_Options.menucolor.r(), g_Options.menucolor.g(), g_Options.menucolor.b(), 255));
	ImGui::PushStyleColor(ImGuiCol_SeparatorActive, ImVec4(g_Options.menucolor.r(), g_Options.menucolor.g(), g_Options.menucolor.b(), 255));
	ImGui::PushStyleColor(ImGuiCol_ResizeGrip, ImVec4(1.000000f, 1.000000f, 1.000000f, 0.160000f));
	ImGui::PushStyleColor(ImGuiCol_ResizeGripHovered, ImVec4(0.780000f, 0.820000f, 1.000000f, 0.600000f));
	ImGui::PushStyleColor(ImGuiCol_ResizeGripActive, ImVec4(0.780000f, 0.820000f, 1.000000f, 0.900000f));
	ImGui::PushStyleColor(ImGuiCol_TextSelectedBg, ImVec4(0.000000f, 0.000000f, 1.000000f, 0.350000f));
	ImGui::PushStyleColor(ImGuiCol_DragDropTarget, ImVec4(1.000000f, 1.000000f, 0.000000f, 0.900000f));
}
