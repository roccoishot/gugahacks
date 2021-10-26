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
#include "features/skins.h"
#include "xor.h"
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
	pattern.append("\\*.ini");
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

void Menu::Initialize()
{
	CreateStyle();

    _visible = true;
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
bool Tab(const char* label, const ImVec2& size_arg, bool state)
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

	ImGui::RenderFrame(bb.Min, bb.Max, state ? ImColor(15, 15, 15) : ImColor(11, 11, 11), true, style.FrameRounding);
	window->DrawList->AddRect(bb.Min, bb.Max, ImColor(0, 0, 0));
	ImGui::RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);

	if (state)
	{
		window->DrawList->AddLine(bb.Min, bb.Min + ImVec2(9, 0), ImColor(255, 255, 255), 1);
		window->DrawList->AddLine(bb.Min, bb.Min + ImVec2(0, 9), ImColor(255, 255, 255), 1);

		window->DrawList->AddLine(bb.Max - ImVec2(0, 1), bb.Max - ImVec2(10, 1), ImColor(255, 255, 255), 1);
		window->DrawList->AddLine(bb.Max - ImVec2(1, 1), bb.Max - ImVec2(1, 10), ImColor(255, 255, 255), 1);
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
	Style->Colors[ImGuiCol_TitleBg] = ImColor(11, 11, 11);
	Style->Colors[ImGuiCol_Border] = ImColor(g_Options.menucolor.r(), g_Options.menucolor.g(), g_Options.menucolor.b(), 255);
	Style->Colors[ImGuiCol_Separator] = ImColor(g_Options.menucolor.r(), g_Options.menucolor.g(), g_Options.menucolor.b(), 255);
	Style->Colors[ImGuiCol_WindowBg] = ImColor(11, 11, 11);
	Style->Colors[ImGuiCol_ChildBg] = ImColor(11, 11, 11);
	Style->Colors[ImGuiCol_FrameBg] = ImColor(22, 22, 22);
	Style->Colors[ImGuiCol_Button] = ImColor(22, 22, 22);
	Style->Colors[ImGuiCol_ButtonHovered] = ImColor(0, 0, 0);
	Style->Colors[ImGuiCol_ButtonActive] = ImColor(0, 0, 0);
	Style->Colors[ImGuiCol_ScrollbarGrab] = ImColor(g_Options.menucolor.r(), g_Options.menucolor.g(), g_Options.menucolor.b(), 255);
	Style->Colors[ImGuiCol_ScrollbarBg] = ImColor(23, 23, 23);
	Style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImColor(0, 0, 0);
	Style->Colors[ImGuiCol_ScrollbarGrabActive] = ImColor(g_Options.menucolor.r(), g_Options.menucolor.g(), g_Options.menucolor.b(), 255);
	ImGui::PushFont(g_SpectatorListFont);

	int specs = 0;
	int modes = 0;
	std::string spect = u8"";
	std::string mode = u8"";
	int DrawIndex = 1;

	for (int playerId : Utils::GetObservervators(g_EngineClient->GetLocalPlayer()))
	{
		C_BasePlayer* pPlayer = (C_BasePlayer*)g_EntityList->GetClientEntity(playerId);

		if (playerId == g_EngineClient->GetLocalPlayer())
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
	bool misc_Spectators = true;

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 1.f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	if (ImGui::Begin("Spectator List", &g_Options.speclist, ImVec2(), 1.f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
	{
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();

		if (specs > 0) spect += u8"\n";     /*ЛАСТ*/
		if (modes > 0) mode += u8"\n";
		ImVec2 size = ImGui::CalcTextSize(spect.c_str());

		ImGui::SetWindowSize(ImVec2(200, 25 + size.y));
		ImGui::Separator("Spectator List");
		ImGui::Text(spect.c_str());
		DrawIndex++;
	}
	ImGui::End();
}

void Menu::Render()
{



	//RenderRadio();

	if (!_visible)
		return;

	int w, h;
	g_EngineClient->GetScreenSize(w, h);
	float x = w * 0.3f - 200.0f;
	float y = h * 0.3f - 100.0f;

	ImGui::SetNextWindowPos(ImVec2{ x, y }, ImGuiCond_Once);
	static int tab = 0;
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
	Style->Colors[ImGuiCol_TitleBg] = ImColor(11, 11, 11);
	Style->Colors[ImGuiCol_Border] = ImColor(g_Options.menucolor.r(), g_Options.menucolor.g(), g_Options.menucolor.b(), 255);
	Style->Colors[ImGuiCol_Separator] = ImColor(g_Options.menucolor.r(), g_Options.menucolor.g(), g_Options.menucolor.b(), 255);
	Style->Colors[ImGuiCol_WindowBg] = ImColor(11, 11, 11);
	Style->Colors[ImGuiCol_ChildBg] = ImColor(11, 11, 11);
	Style->Colors[ImGuiCol_FrameBg] = ImColor(22, 22, 22);
	Style->Colors[ImGuiCol_Button] = ImColor(22, 22, 22);
	Style->Colors[ImGuiCol_ButtonHovered] = ImColor(0, 0, 0);
	Style->Colors[ImGuiCol_ButtonActive] = ImColor(0, 0, 0);
	Style->Colors[ImGuiCol_ScrollbarGrab] = ImColor(g_Options.menucolor.r(), g_Options.menucolor.g(), g_Options.menucolor.b(), 255);
	Style->Colors[ImGuiCol_ScrollbarBg] = ImColor(23, 23, 23);
	Style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImColor(0, 0, 0);
	Style->Colors[ImGuiCol_ScrollbarGrabActive] = ImColor(g_Options.menucolor.r(), g_Options.menucolor.g(), g_Options.menucolor.b(), 255);
	ImGui::PushFont(g_SpectatorListFont);

	ImGui::SetNextWindowPos(ImVec2{ 0, 0 }, ImGuiSetCond_Once);
	ImGui::SetNextWindowSize(ImVec2{ 795, 695 }, ImGuiSetCond_Once);
	auto flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | NULL | NULL | ImGuiWindowFlags_NoCollapse | NULL | NULL | NULL;


	//Tabs
	ImGui::Begin("Edited", nullptr, flags);
	{
		ImVec2 p = ImGui::GetWindowPos();

		ImGui::SetCursorPos({ 5 + 5 + 0,17 });
		if (Tab("ANTIAIM", { 125,35 }, tab == 5))
			tab = 5;

		ImGui::SetCursorPos({ 15 + 115 + 10,17 });
		if (Tab("AIMBOT", { 125,35 }, tab == 0))
			tab = 0;

		ImGui::SetCursorPos({ 15 + 235 + 20,17 });
		if (Tab("VISUALS", { 125,35 }, tab == 1))
			tab = 1;

		ImGui::SetCursorPos({ 15 + 355 + 30,17 });
		if (Tab("WORLD", { 125,35 }, tab == 2))
			tab = 2;

		ImGui::SetCursorPos({ 15 + 475 + 40,17 });
		if (Tab("MISC", { 125,35 }, tab == 3))
			tab = 3;

		ImGui::SetCursorPos({ 15 + 595 + 50,17 });
		if (Tab("SKINS", { 125,35 }, tab == 4))
			tab = 4;

		if (tab == 5) {
			ImGui::SetCursorPos({ 21,65 });
			ImGui::BeginChild("##1", { 255,520 });
			{

				const char* aa_pitch_list[] = {
"None",
"Offset",
"Down",
"Up",
"Zero",
"Fake Up",
"Fake Down"

				};

				const char* aa_yaw_list[] = {
					"None",
					"Backwards",
					"Spin",
					"LBY",
					"Freestand"
				};

				ImGui::Separator("Antiaim");
				ImGui::Combo("Pitch", &g_Options.ragebot_antiaim_pitch, aa_pitch_list, IM_ARRAYSIZE(aa_pitch_list));
				ImGui::Combo("Yaw", &g_Options.ragebot_antiaim_yaw, aa_yaw_list, IM_ARRAYSIZE(aa_yaw_list));
				ImGui::Checkbox("Desync", &g_Options.ragebot_antiaim_desync);
				ImGui::Checkbox("Break LBY", &g_Options.breaklby);
				if (g_Options.ragebot_antiaim_yaw == 2) {
					ImGui::SliderInt("Spin Speed", &g_Options.spinspeed, 1.f, 10.f, "%.f");
				}
				ImGui::Text("Invert AA Key"); ImGui::SameLine(); ImGui::Hotkey("                                                                                                                                                                                 ", &g_Options.invertaa);
				ImGui::Separator("Others");
				ImGui::Text("Slowwalk Key"); ImGui::SameLine(); ImGui::Hotkey("                                                            ", &g_Options.ragebot_slowwalk_key);
				ImGui::Spacing();
				ImGui::SliderInt("Slowwalk Speed", &g_Options.ragebot_slowwalk_amt, 0, 100);
				ImGui::Spacing();
				ImGui::Checkbox("Fakeping", &g_Options.fakeping);
				if (g_Options.fakeping) {
					ImGui::SliderInt("Ping", &g_Options.fakepingzzz, 1.f, 1000.f, "%.f");
					ImGui::Text("On Key"); ImGui::SameLine(); ImGui::Hotkey("                                                                                             ", &g_Options.fakepingkey);
				}
				//ImGui::Checkbox("Resolver", &g_Options.rageresolver);
			}
			ImGui::EndChild();
		}

		if (tab == 0)
		{
			static int definition_index = WEAPON_INVALID;

			auto localPlayer = C_BasePlayer::GetPlayerByIndex(g_EngineClient->GetLocalPlayer());
			if (g_EngineClient->IsInGame() && localPlayer && localPlayer->IsAlive() && localPlayer->m_hActiveWeapon() && localPlayer->m_hActiveWeapon()->IsGun())
				definition_index = localPlayer->m_hActiveWeapon()->m_Item().m_iItemDefinitionIndex();
			else
				definition_index = WEAPON_INVALID;
			if (definition_index == WEAPON_INVALID)definition_index = WEAPON_DEAGLE;
			ImGui::SetCursorPos({ 21,65 });
			ImGui::BeginChild("##1", { 166,520 });
			{
				float group_w = ImGui::GetCurrentWindow()->Size.x - ImGui::GetStyle().FramePadding.x * 2;
				ImGui::Separator("Aimbot");
				ImGui::Checkbox("Enabled", &g_Options.aimbot.enabled);
				ImGui::Text("Fov");
				ImGui::Spacing();
				ImGui::SliderFloat("##Fov", &g_Options.aimbot.fov, 0.f, 20.f, "%.f");
				ImGui::Spacing();
				ImGui::Text("Smooth");
				ImGui::Spacing();
				ImGui::SliderFloat("##Smooth", &g_Options.aimbot.smoof, 1.f, 20.f, "%.f");
				ImGui::Spacing();
				if (ImGui::BeginCombo("##hitbox_filter", "Hitboxes"))
				{
					ImGui::Selectable("Head", &g_Options.aimbot.head, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable("Chest", &g_Options.aimbot.chest, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable("Pelvis", &g_Options.aimbot.pelvis, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable("Arms", &g_Options.aimbot.arms, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable("Hands", &g_Options.aimbot.hands, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable("Legs", &g_Options.aimbot.legs, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable("Feet", &g_Options.aimbot.feet, ImGuiSelectableFlags_DontClosePopups);

					ImGui::EndCombo();
				}

				ImGui::Separator("Others");
				ImGui::Checkbox("Hitchance", &g_Options.aimbot.hc);
				if (g_Options.aimbot.hc)
					ImGui::SliderInt("Hitchance", &g_Options.aimbot.hitchance, 1, 100, "%.f");
				ImGui::Checkbox("Backtrack", &g_Options.misc_backtrack);
				if (g_Options.misc_backtrack) {
					ImGui::SliderInt("Ticks", &g_Options.backtix, 0.f, 62.f, "%.f");
				}
				ImGui::Checkbox("Silent", &g_Options.aimbot.silent);
				if (g_Options.aimbot.silent) {
					ImGui::Text("  Silent fov");
					ImGui::Spacing();
					ImGui::SliderFloat("##Silentfov", &g_Options.aimbot.silentfov, 0.f, 360.f, "%.f");
					ImGui::Spacing();
				}
				ImGui::Checkbox("Fast Aimbot", &g_Options.aimbot.fastaimbot);
				ImGui::Checkbox("Autofire##autofire", &g_Options.aimbot.autofire);
				ImGui::Checkbox("Autorevolver", &g_Options.aimbot.autorevolver1);
				ImGui::Checkbox("Autowall##autowall", &g_Options.aimbot.autowall);
				if (g_Options.aimbot.autowall) {
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Text("Min Damage");
					ImGui::Spacing();
					ImGui::SliderInt("##minDamage", &g_Options.aimbot.autowallmin, 1, 100, "%i");
				}
					ImGui::Checkbox("RCS##rcs", &g_Options.aimbot.rcs);

					const char* rcs_types[] = {
						"Type: Legit",
						"Type: HvH"
					};

					if (ImGui::BeginCombo("##type", rcs_types[g_Options.aimbot.rcstype], ImGuiComboFlags_NoArrowButton))
					{
						for (int i = 0; i < IM_ARRAYSIZE(rcs_types); i++)
						{
							if (ImGui::Selectable(rcs_types[i], i == g_Options.aimbot.rcstype))
								g_Options.aimbot.rcstype = i;
						}

						ImGui::EndCombo();
					}
					ImGui::SliderInt("  X", &g_Options.aimbot.x, 0, 100, "%i");
					ImGui::Spacing();		ImGui::Spacing();		ImGui::Spacing();
					ImGui::SliderInt("  Y", &g_Options.aimbot.y, 0, 100, "%i");
				}
				ImGui::EndChild();
			}

		if (tab == 1)
		{
			ImGui::SetCursorPos({ 21,65 });
			ImGui::BeginChild("##1", { 166 ,580 });
			{
				ImGui::Separator("ESP");
				float group_w = ImGui::GetCurrentWindow()->Size.x - ImGui::GetStyle().FramePadding.x * 2;

					ImGui::Checkbox("Far ESP", &g_Options.faresp);
					ImGui::Checkbox("Team ESP", &g_Options.teamesp);
					ImGui::Checkbox("Boxes", &g_Options.esp_player_boxes);
					ImGui::Checkbox("Occluded ", &g_Options.esp_player_boxesOccluded);
					ImGui::Checkbox("Names", &g_Options.esp_player_names);
					ImGui::Checkbox("Health", &g_Options.esp_player_health);
					ImGui::Checkbox("Armour", &g_Options.esp_player_armour);
					ImGui::Checkbox("Weapons", &g_Options.esp_player_weapons);
					ImGui::Checkbox("Dropped Weapons", &g_Options.esp_dropped_weapons);
					ImGui::Checkbox("Flash Kill Check", &g_Options.flashkillcheck);
					ImGui::Separator("Viewmodel");
					ImGui::Spacing();
					ImGui::Checkbox("Viewmodel Changer", &g_Options.enablechanger);
					if (g_Options.enablechanger) {
						ImGui::Spacing();
						ImGui::SliderInt("FOV", &g_Options.viewmodel_fov, 0.0, 120.0, "%.f");
						ImGui::Spacing();
						ImGui::SliderInt("X", &g_Options.viewmodel_offset_x, -20.0, 20.0, "%.f");
						ImGui::Spacing();
						ImGui::SliderInt("Y", &g_Options.viewmodel_offset_y, -20.0, 20.0, "%.f");
						ImGui::Spacing();
						ImGui::SliderInt("Z", &g_Options.viewmodel_offset_z, -20.0, 20.0, "%.f");
					};

				}
				ImGui::EndChild();

				ImGui::SetCursorPos({ 417, 65 });
				ImGui::BeginChild("##2", { 166, 315 });
				{
					float group_w = ImGui::GetCurrentWindow()->Size.x - ImGui::GetStyle().FramePadding.x * 2;

					ImGui::Separator("Colors");
					if (g_Options.esp_player_boxes) {
						ImGui::Text("Boxes"); ImGui::SameLine(group_w - 20); ImGuiEx::ColorEdit4a("Enemies Visible   ", &g_Options.color_esp_enemy_visible);
					}
					if (g_Options.esp_player_boxesOccluded && g_Options.esp_player_boxes) {
						ImGui::Text("Boxes XQZ"); ImGui::SameLine(group_w - 20); ImGuiEx::ColorEdit4a("Enemies Occluded      ", &g_Options.color_esp_enemy_occluded);
					}
					if (g_Options.esp_player_names) {
						ImGui::Text("Names"); ImGui::SameLine(group_w - 20); ImGuiEx::ColorEdit4a("Names Color ", &g_Options.color_name_player);
					}
					if (g_Options.esp_player_armour) {
						ImGui::Text("Armour"); ImGui::SameLine(group_w - 20); ImGuiEx::ColorEdit4a("Armour Color ", &g_Options.color_armour_player);
					}
					if (g_Options.esp_player_weapons) {
						ImGui::Text("Weapons"); ImGui::SameLine(group_w - 20); ImGuiEx::ColorEdit4a("Weapons Color ", &g_Options.color_esp_weapons);
					}
					if (g_Options.chams_player_enabled) {
						ImGui::Text("Chams"); ImGui::SameLine(group_w - 20); ImGuiEx::ColorEdit4a("Enemy Visible ", &g_Options.color_chams_player_enemy_visible);
					}
					if (g_Options.player_material == 4 && g_Options.chams_player_enabled) {
						ImGui::Text("Double Enemy"); ImGui::SameLine(group_w - 20); ImGuiEx::ColorEdit4a("Double Color Enemies", &g_Options.glowcolorenemy);
					}
					if (g_Options.player_material == 5 && g_Options.chams_player_enabled) {
						ImGui::Text("Double Enemy"); ImGui::SameLine(group_w - 20); ImGuiEx::ColorEdit4a("Double Color Enemies", &g_Options.glowcolorenemy);
					}
					if (g_Options.player_material == 6 && g_Options.chams_player_enabled) {
						ImGui::Text("Double Enemy"); ImGui::SameLine(group_w - 20); ImGuiEx::ColorEdit4a("Double Color Enemies", &g_Options.glowcolorenemy);
					}
					if (g_Options.player_material == 4 && g_Options.teamchams) {
						ImGui::Text("Double Team"); ImGui::SameLine(group_w - 20); ImGuiEx::ColorEdit4a("Double Color Team", &g_Options.glowcolor);
					}
					if (g_Options.player_material == 5 && g_Options.teamchams) {
						ImGui::Text("Double Team"); ImGui::SameLine(group_w - 20); ImGuiEx::ColorEdit4a("Double Color Team", &g_Options.glowcolor);
					}
					if (g_Options.player_material == 6 && g_Options.teamchams) {
						ImGui::Text("Double Team"); ImGui::SameLine(group_w - 20); ImGuiEx::ColorEdit4a("Double Color Team", &g_Options.glowcolor);
					}
					if (g_Options.player_material == 4 && g_Options.localchams) {
						ImGui::Text("Double Local"); ImGui::SameLine(group_w - 20); ImGuiEx::ColorEdit4a("Double Color Local", &g_Options.glowcolorlocal);
					}
					if (g_Options.player_material == 5 && g_Options.localchams) {
						ImGui::Text("Double Local"); ImGui::SameLine(group_w - 20); ImGuiEx::ColorEdit4a("Double Color Local", &g_Options.glowcolorlocal);
					}
					if (g_Options.player_material == 6 && g_Options.localchams) {
						ImGui::Text("Double Local"); ImGui::SameLine(group_w - 20); ImGuiEx::ColorEdit4a("Double Color Local", &g_Options.glowcolorlocal);
					}
					if (g_Options.chams_player_ignorez) {
						ImGui::Text("Chams XQZ"); ImGui::SameLine(group_w - 20); ImGuiEx::ColorEdit4a("Enemy Occluded ", &g_Options.color_chams_player_enemy_occluded);
					}
					if (g_Options.chams_player_enabled && g_Options.teamchams) {
						ImGui::Text("Team Chams"); ImGui::SameLine(group_w - 20); ImGuiEx::ColorEdit4a("Team Visible ", &g_Options.color_chams_player_ally_visible);
					}
					if (g_Options.chams_player_ignorez && g_Options.teamchams) {
						ImGui::Text("Team Chams XQZ"); ImGui::SameLine(group_w - 20); ImGuiEx::ColorEdit4a("Team Occluded ", &g_Options.color_chams_player_ally_occluded);
					}
					if (g_Options.chams_player_enabled && g_Options.localchams) {
						ImGui::Text("Local Chams"); ImGui::SameLine(group_w - 20); ImGuiEx::ColorEdit4a("Kill Visible ", &g_Options.color_chams_player_local_visible);
					}
					if (g_Options.chams_player_ignorez && g_Options.localchams) {
						ImGui::Text("Local Chams XQZ"); ImGui::SameLine(group_w - 20); ImGuiEx::ColorEdit4a("Kill Occluded ", &g_Options.color_chams_player_local_occluded);
					}
					if (g_Options.chams_arms_enabled && g_Options.arms_material == 3) {
						ImGui::Text("Double Arms"); ImGui::SameLine(group_w - 20); ImGuiEx::ColorEdit4a("Double Arms", &g_Options.glowcolorarms);
					}
					if (g_Options.chams_arms_enabled && g_Options.arms_material == 4) {
						ImGui::Text("Double Arms"); ImGui::SameLine(group_w - 20); ImGuiEx::ColorEdit4a("Double Arms", &g_Options.glowcolorarms);
					}
					if (g_Options.chams_arms_enabled && g_Options.arms_material == 5) {
						ImGui::Text("Double Arms"); ImGui::SameLine(group_w - 20); ImGuiEx::ColorEdit4a("Double Arms", &g_Options.glowcolorarms);
					}
					if (g_Options.chams_arms_enabled) {
						ImGui::Text("Arm Chams"); ImGui::SameLine(group_w - 20); ImGuiEx::ColorEdit4a("Arms", &g_Options.color_chams_arms_visible);
					}
					if (g_Options.chams_arms_ignorez) {
						ImGui::Text("Arms XQZ"); ImGui::SameLine(group_w - 20); ImGuiEx::ColorEdit4a("Arms XQZ", &g_Options.color_chams_arms_occluded);
					}
					if (g_Options.chams_strap_enabled && g_Options.strap_material == 3) {
						ImGui::Text("Double Strap"); ImGui::SameLine(group_w - 20); ImGuiEx::ColorEdit4a("Double Strap", &g_Options.glowcolorstrap);
					}
					if (g_Options.chams_strap_enabled && g_Options.strap_material == 4) {
						ImGui::Text("Double Strap"); ImGui::SameLine(group_w - 20); ImGuiEx::ColorEdit4a("Double Strap", &g_Options.glowcolorstrap);
					}
					if (g_Options.chams_strap_enabled && g_Options.strap_material == 5) {
						ImGui::Text("Double Strap"); ImGui::SameLine(group_w - 20); ImGuiEx::ColorEdit4a("Double Strap", &g_Options.glowcolorstrap);
					}
					if (g_Options.chams_strap_enabled && g_Options.strap_material == 6) {
						ImGui::Text("Double Strap"); ImGui::SameLine(group_w - 20); ImGuiEx::ColorEdit4a("Double Strap", &g_Options.glowcolorstrap);
					}
					if (g_Options.chams_strap_enabled) {
						ImGui::Text("Strap Chams"); ImGui::SameLine(group_w - 20); ImGuiEx::ColorEdit4a("Strap", &g_Options.color_chams_strap_visible);
					}
					if (g_Options.chams_strap_ignorez) {
						ImGui::Text("Strap XQZ"); ImGui::SameLine(group_w - 20); ImGuiEx::ColorEdit4a("Strap XQZ", &g_Options.color_chams_strap_occluded);
					}
				}
				ImGui::EndChild();

				ImGui::SetCursorPos({ 31 + 166,65 });
				ImGui::BeginChild("##2", { 166,276 });
				{


					float group_w = ImGui::GetCurrentWindow()->Size.x - ImGui::GetStyle().FramePadding.x * 2;




				}
				ImGui::EndChild();

				ImGui::SetCursorPos({ 31 + 188,65 });
				ImGui::BeginChild("##3", { 166,620 /*nice*/ });
				{
					ImGui::Separator("Chams");

					float group_w = ImGui::GetCurrentWindow()->Size.x - ImGui::GetStyle().FramePadding.x * 2;


					ImGui::Checkbox("Enabled ", &g_Options.chams_player_enabled);
					if (g_Options.chams_player_enabled) {
						ImGui::Checkbox("Occluded", &g_Options.chams_player_ignorez);
						ImGui::Checkbox("Team Chams", &g_Options.teamchams);
						ImGui::Checkbox("Local Chams", &g_Options.localchams);
						ImGui::Checkbox("Arms Chams", &g_Options.chams_arms_enabled);
						if (g_Options.chams_arms_enabled) {
							ImGui::Checkbox("Arms XQZ", &g_Options.chams_arms_ignorez);
						}

						ImGui::Checkbox("Strap Chams", &g_Options.chams_strap_enabled);
						if (g_Options.chams_strap_enabled) {
							ImGui::Checkbox("Strap XQZ", &g_Options.chams_strap_ignorez);
						}
					}

					const char* MatList[] = {
"Textured",
"Flat",
"Shine",
"Velvet",
"Animated",
"Glow",
"Double"
					};

					const char* MatList2[] = {
"Textured",
"Flat",
"Shine",
"Animated",
"Glow",
"Double"
					};

					const char* MatList3[] = {
"Textured",
"Flat",
"Shine",
"Animated",
"Glow",
"Double"
//"AdvancedAim"
					};

					ImGui::Combo("Player Mat", &g_Options.player_material, MatList, IM_ARRAYSIZE(MatList));

					if (g_Options.chams_strap_enabled) {
						ImGui::Combo("Strap Mat", &g_Options.strap_material, MatList3, IM_ARRAYSIZE(MatList3));
					}
					if (g_Options.chams_arms_enabled) {
						ImGui::Combo("Arms Mat", &g_Options.arms_material, MatList2, IM_ARRAYSIZE(MatList2));
					}

					ImGui::Spacing();
					ImGui::Separator("Others");
					ImGui::Spacing();
					ImGui::Checkbox("EB Effects", &g_Options.ebdetection);
					ImGui::Checkbox("FOV In Scope", &g_Options.fovscope);
					ImGui::Checkbox("Draw FOV", &g_Options.drawfov);
					ImGui::Checkbox("Force Crosshair", &g_Options.sniper_xhair);
					ImGui::Checkbox("No smoke", &g_Options.no_smoke);
					ImGui::Checkbox("No flash", &g_Options.no_flash);
					ImGui::Checkbox("Flash Indicator", &g_Options.bowlsfreshcut);
					ImGui::Checkbox("Shot Info", &g_Options.shotinfo);
					ImGui::Checkbox("Floating Ragdolls", &g_Options.ragfloat);
					ImGui::Checkbox("Recoil Crosshair", &g_Options.rcross);
					ImGui::Checkbox("Grenade Prediction", &g_Options.pnade);
					ImGui::Checkbox("Thirdperson", &g_Options.misc_thirdperson); ImGui::SameLine(); ImGui::Hotkey("            ", &g_Options.misc_thirdperson_key);
					ImGui::Spacing();
					ImGui::SliderInt("Fov", &g_Options.fovchangaaa, 0, 120, "%.f");
					ImGui::Checkbox("Aspect Ratio", &g_Options.aspectchange);
					if (g_Options.aspectchange) {
						ImGui::Spacing();
						ImGui::SliderFloat("Ratio", &g_Options.aspectratio, 0.10, 3);
					};

				}
				ImGui::EndChild();
			}
			else if (tab == 2) {

				ImGui::SetCursorPos({ 21,65 });
				ImGui::BeginChild("##1", { 166 ,585 });
				{
					ImGui::Separator("World");
					if (ImGui::BeginCombo("##removals", "Removals"))
					{
						ImGui::Selectable("Recoil", &g_Options.fatassmf, ImGuiSelectableFlags_DontClosePopups);
						ImGui::Selectable("Processing", &g_Options.disablepro, ImGuiSelectableFlags_DontClosePopups);
						ImGui::Selectable("Blur", &g_Options.removeblur, ImGuiSelectableFlags_DontClosePopups);
						ImGui::Selectable("Scope", &g_Options.noscope, ImGuiSelectableFlags_DontClosePopups);

						ImGui::EndCombo();
					}
					ImGui::Checkbox("Matrix Mode", &g_Options.matrix);
					ImGui::Checkbox("Metallic Mode", &g_Options.metallica);
					ImGui::Checkbox("Fullbright", &g_Options.fullbright);
					ImGui::Checkbox("Sky Changer", &g_Options.sky_changer);
					const char* SkyList[] = {
	"Night",
	"Baggage",
	"Tibet",
	"Vietnam",
	"Lunacy",
	"Embassy",
	"Italy",
	"Jungle",
	"Office",
	"Day",
	"Cloudy",
	"Dust"
					};
					if (g_Options.sky_changer) {
						ImGui::Combo("Sky", &g_Options.skyshitsssss, SkyList, IM_ARRAYSIZE(SkyList));
					}
					ImGui::Text("Model Ambience");
					ImGui::SliderFloat("  ", &g_Options.amibence, 0, 1500);
					ImGui::Checkbox("Color Modulation", &g_Options.colormodulation);
					if (g_Options.colormodulation) {
						ImGui::Checkbox("Nightmode", &g_Options.colormodulate);
						if (g_Options.colormodulate)
							ImGui::Checkbox("Asus Props", &g_Options.asusprops);
					}
					ImGui::Separator("Glow");
					ImGui::Spacing();
					ImGui::SliderFloat("R Glow", &g_Options.worldglowr, 0, 1);
					ImGui::Spacing();
					ImGui::SliderFloat("G Glow", &g_Options.worldglowg, 0, 1);
					ImGui::Spacing();
					ImGui::SliderFloat("B Glow", &g_Options.worldglowb, 0, 1);
					ImGui::Separator("Fog");
					ImGui::Checkbox("Override Fog", &g_Options.fogchanga);
					ImGui::Spacing();
					ImGui::SliderInt("Distance", &g_Options.fogfardamn, 0, 2500);
					ImGui::Spacing();
					ImGui::SliderInt("Thickness", &g_Options.fogdens, 0, 100);
					ImGui::Text("Fog Color");
					ImGui::SameLine();
					ImGuiEx::ColorEdit3("Fog Color", &g_Options.fogcoluh);
				}
				ImGui::EndChild();
			}

			else if (tab == 3)
			{
				ImGui::SetCursorPos({ 31 + 188,65 });
				ImGui::BeginChild("##3", { 166,420});
				{
					ImGui::Separator("General");

					float group_w = ImGui::GetCurrentWindow()->Size.x - ImGui::GetStyle().FramePadding.x * 2;
					static char newname[64];
					//Ha penis
					ImGui::Checkbox("Watermark##hc", &g_Options.misc_watermark);
					//Fast Shit Doe
					ImGui::Checkbox("AntiOBS", &g_Options.antiobs);
					ImGui::Checkbox("Hitsounds", &g_Options.misc_hitmarker);
					const char* HitList[] = {
	"Metallic",
		"Agree",
		"Disagree",
		"Electric",
		"FranzJ",
		"Spark",
		"Gmod",
		"Found Game",
					};
					if (g_Options.misc_hitmarker) {
						ImGui::Combo("Sound", &g_Options.hitmarkersound, HitList, IM_ARRAYSIZE(HitList));
					}
					ImGui::Checkbox("Velocity Graph", &g_Options.Velocity);
					ImGui::SameLine(group_w - 20);
					ImGuiEx::ColorEdit4("Velocity Color", &g_Options.Velocitycol);
					if (ImGui::BeginCombo("Velocity", "Velocity"))
					{
						ImGui::Selectable("Outline", &g_Options.outline, ImGuiSelectableFlags_DontClosePopups);
						ImGui::Selectable("Last jump", &g_Options.lastjump, ImGuiSelectableFlags_DontClosePopups);
						ImGui::Selectable("Last jump outline", &g_Options.lastjumpoutline, ImGuiSelectableFlags_DontClosePopups);

						ImGui::EndCombo();
					}

					//Misc Man Shit FRFR
					const char* ClantagTypes[] = {
"Animated",
	"Discord",
	"Static",
	"Reverse"
					};
					ImGui::Checkbox("Chat Spam", &g_Options.misc_chatspam);
					ImGui::Checkbox("Clantag", &g_Options.clantag);
					if (g_Options.clantag) {
						ImGui::Combo("Type", &g_Options.clantagtype, ClantagTypes, IM_ARRAYSIZE(ClantagTypes));
					}
					ImGui::Checkbox("Auto Accept", &g_Options.autoaccept);
					if (ImGui::Button("Change Name")) {
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
					ImGui::InputText("##namec", newname, 1000);
					ImGui::Spacing();
					ImGui::Separator("Accents");
					ImGui::Spacing();
					ImGui::Text("Menu Accents"); ImGui::SameLine(); ImGuiEx::ColorEdit4("Menu Accents", &g_Options.menucolor);

				}
				ImGui::EndChild();

				ImGui::SetCursorPos({ 417, 65 });
				ImGui::BeginChild("##2", { 166, 460 });
				{
					ImGui::Separator("Movement");
					float group_w = ImGui::GetCurrentWindow()->Size.x - ImGui::GetStyle().FramePadding.x * 2;

					const char* EbModes[] = {
"OG",
"Experimental"
					};

					//NOT COOL!!!
					ImGui::Checkbox("No Duck Cooldown", &g_Options.nocool);
					ImGui::Checkbox("Bhop", &g_Options.misc_bhop);
					ImGui::Checkbox("Autostrafe", &g_Options.autostrafe); ImGui::SameLine(group_w - 50);     ImGui::Hotkey(" ", &g_Options.AutoStafe_key);
					ImGui::Checkbox("Edgebug", &g_Options.edge_bug); ImGui::SameLine(group_w - 50);          ImGui::Hotkey("  ", &g_Options.edge_bug_key);
					if (g_Options.edge_bug)
						ImGui::Combo("Eb Mode", &g_Options.ebmode, EbModes, IM_ARRAYSIZE(EbModes));
					ImGui::Checkbox("Jumpbug", &g_Options.jump_bug); ImGui::SameLine(group_w - 50);          ImGui::Hotkey("   ", &g_Options.jump_bug_key);
					ImGui::Checkbox("Edge jump", &g_Options.edgejump.enabled); ImGui::SameLine(group_w - 50); ImGui::Hotkey("    ", &g_Options.edgejump.hotkey);
					ImGui::Checkbox("Blockbot", &g_Options.blockbot); ImGui::SameLine(group_w - 50); ImGui::Hotkey("                                                                                                                                                                                                                                                                                                                                      ", &g_Options.bbkey);
					ImGui::Checkbox("Duck In Air", &g_Options.ducknair);
					ImGui::Checkbox("Spectator List", &g_Options.speclist);
					ImGui::Separator("Test");
					ImGui::Checkbox("Test Features", &g_Options.enablebeta);
					if (g_Options.enablebeta) {
						ImGui::Checkbox("Fast Shiftwalk Charge", &g_Options.slidewalk);
					}
					ImGui::EndChild();
				}

				ImGui::SetCursorPos({ 21,65 });
				ImGui::BeginChild("##1", { 166 ,580 });
				{
					ImGui::Separator("Config");

					static int selected = 0;
					static char cfgName[64];

					std::vector<std::string> cfgList;
					ReadDirectory(g_Options.folder, cfgList);
					ImGui::PushItemWidth(150.f);
					if (!cfgList.empty())
					{
						if (ImGui::BeginCombo("##SelectConfig", cfgList[selected].c_str(), ImGuiComboFlags_NoArrowButton))
						{
							for (size_t i = 0; i < cfgList.size(); i++)
							{
								if (ImGui::Selectable(cfgList[i].c_str(), i == selected))
									selected = i;
							}
							ImGui::EndCombo();

						}
						if (ImGui::Button("Save Config"))
							g_Options.SaveSettings(cfgList[selected]);
						//ImGui::SameLine();
						if (ImGui::Button("Load Config"))
							g_Options.LoadSettings(cfgList[selected]);
						//ImGui::SameLine();
						if (ImGui::Button("Delete Config"))
						{
							g_Options.DeleteSettings(cfgList[selected]);
							selected = 0;
						}
						//	ImGui::Separator();
					}
					ImGui::InputText("##configname", cfgName, 24);
					//ImGui::SameLine();
					if (ImGui::Button("Create Config"))
					{
						if (strlen(cfgName))
							g_Options.SaveSettings(cfgName + std::string(".ini"));
					}
					ImGui::PopItemWidth();
				}
				ImGui::EndChild();

			}
			else if (tab == 4)
			{
				static std::string selected_weapon_name = "";
				static std::string selected_skin_name = "";
				static auto definition_vector_index = 0;
				auto& entries = g_Options.changers.skin.m_items;
				ImGui::SetCursorPos({ 21,65 });
				ImGui::BeginChild("##1", { 166,525 });
				{
					ImGui::Spacing();
					ImGui::Spacing();

					{
						for (size_t w = 0; w < k_weapon_names.size(); w++)
						{
							switch (w)
							{
							case 0:
								ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.f), "Knife");
								break;
							case 2:
								ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.f), "Gloves");
								break;
							case 4:
								ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.f), "Pistols");
								break;
							case 14:
								ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.f), "Smgs");
								break;
							case 21:
								ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.f), "Rifles");
								break;
							case 28:
								ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.f), "Snipers");
								break;
							case 32:
								ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.f), "Heavy");
								break;
							case 34:
								ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.f), "Shotguns");
								break;
							}

							if (ImGui::Selectable(k_weapon_names[w].name, definition_vector_index == w))
							{
								definition_vector_index = w;
							}
						}
					}
					//ImGui::ListBoxFooter();

				}
				ImGui::EndChild();

				ImGui::SetCursorPos({ 31 + 380,65 });
				ImGui::BeginChild("##4", { 166,275 });
				{
					ImGui::Separator("Playermodel");
					ImGui::Text("   T Player Model");
					ImGui::PushItemWidth(160.f);
					ImGui::Combo("##TPlayerModel", &g_Options.playerModelT, "Default\0Cmdr. Goggles\0Cmdr. Wet Sox\0Lieutenant Rex Krikey\0Michael Syfers\0Operator\0Special Agent Ava\0Markus Delrow\0Sous-Lieutenant Medic\0Chem-Haz Capitaine\0Chef d'Escadron Rouchard\0Aspirant\0Officer Jacques Beltram\0D Squadron Officer\0B Squadron Officer\0Seal Team 6 Soldier\0Buckshot\0Lt. Commander Ricksaw\0Buckshot\0Commando Company\0Two Times' McCoy\0Two Times' McCoy2\0Primeiro Tenente\0Cmdr. Mae 'Dead Cold' Jamison\0Lieutenant Farlow\0John 'Van Healen' Kask\0Bio-Haz Specialist\0Sergeant Bombson\0Chem-Haz Specialist\0Getaway Sally\0Number K\0Little Kev\0Safecracker Voltzmann\0Bloody Darryl The Strapped\0Sir Bloody Loudmouth Darryl\0Sir Bloody Darryl Royale\0Sir Bloody Skullhead Darryl\0Sir Bloody Silent Darryl\0Sir Bloody Miami Darryl\0Street Soldier\0Soldier\0Slingshot\0Enforcer\0Mr. Muhlik\0Prof. Shahmat\0Osiris\0Ground Rebel\0The Elite Mr. Muhlik\0Trapper\0Trapper Aggressor\0Vypa Sista of the Revolution\0Col. Mangos Dabisi\0'Medium Rare' Crasswater\0Crasswater The Forgotten\0Elite Trapper Solman\0'The Doctor' Romanov\0Blackwolf\0Maximus\0Dragomir\0Rezan The Ready\0Rezan the Redshirt\0Dragomir\0");

					ImGui::Text("   CT Player Model");
					ImGui::PushItemWidth(160.f);
					ImGui::Combo("##CTPlayerModel", &g_Options.playerModelCT, "Default\0Cmdr. Goggles\0Cmdr. Wet Sox\0Lieutenant Rex Krikey\0Michael Syfers\0Operator\0Special Agent Ava\0Markus Delrow\0Sous-Lieutenant Medic\0Chem-Haz Capitaine\0Chef d'Escadron Rouchard\0Aspirant\0Officer Jacques Beltram\0D Squadron Officer\0B Squadron Officer\0Seal Team 6 Soldier\0Buckshot\0Lt. Commander Ricksaw\0Buckshot\0Commando Company\0Two Times' McCoy\0Two Times' McCoy2\0Primeiro Tenente\0Cmdr. Mae 'Dead Cold' Jamison\0Lieutenant Farlow\0John 'Van Healen' Kask\0Bio-Haz Specialist\0Sergeant Bombson\0Chem-Haz Specialist\0Getaway Sally\0Number K\0Little Kev\0Safecracker Voltzmann\0Bloody Darryl The Strapped\0Sir Bloody Loudmouth Darryl\0Sir Bloody Darryl Royale\0Sir Bloody Skullhead Darryl\0Sir Bloody Silent Darryl\0Sir Bloody Miami Darryl\0Street Soldier\0Soldier\0Slingshot\0Enforcer\0Mr. Muhlik\0Prof. Shahmat\0Osiris\0Ground Rebel\0The Elite Mr. Muhlik\0Trapper\0Trapper Aggressor\0Vypa Sista of the Revolution\0Col. Mangos Dabisi\0'Medium Rare' Crasswater\0Crasswater The Forgotten\0Elite Trapper Solman\0'The Doctor' Romanov\0Blackwolf\0Maximus\0Dragomir\0Rezan The Ready\0Rezan the Redshirt\0Dragomir\0");

				}
				ImGui::EndChild();

				ImGui::SetCursorPos({ 31 + 166,65 });
				ImGui::BeginChild("##2", { 210,525 });
				{
					ImGui::Spacing();
					ImGui::Spacing();
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
					//ImGui::Checkbox("skin preview", &g_Options.changers.skin.skin_preview);


				};
				ImGui::EndChild();

				ImGui::SetCursorPos({ 21,610 });
				ImGui::BeginChild("##3", { 230,62 });
				{
					auto& selected_entry = entries[k_weapon_names[definition_vector_index].definition_index];
					ImGui::InputInt("Seed", &selected_entry.seed);
					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::SliderFloat("Wear", &selected_entry.wear, 0.01, 1.00f);
					ImGui::Spacing();
					if (ImGui::Button("Force Update"))
					{
						//	if (next_enb_time <= g_GlobalVars->curtime)
						{
							static auto clear_hud_weapon_icon_fn =
								reinterpret_cast<std::int32_t(__thiscall*)(void*, std::int32_t)>(
									Utils::PatternScan2("client.dll", "55 8B EC 51 53 56 8B 75 08 8B D9 57 6B FE 2C 89 5D FC"));

							auto element = FindHudElement<std::uintptr_t*>("CCSGO_HudWeaponSelection");

							if (element)
							{
								auto hud_weapons = reinterpret_cast<hud_weapons_t*>(std::uintptr_t(element) - 0xa0);
								if (hud_weapons != nullptr)
								{

									if (*hud_weapons->get_weapon_count())
									{
										for (std::int32_t i = 0; i < *hud_weapons->get_weapon_count(); i++)
											i = clear_hud_weapon_icon_fn(hud_weapons, i);

										typedef void(*ForceUpdate) (void);
										static ForceUpdate FullUpdate = (ForceUpdate)Utils::PatternScan2("engine.dll", "A1 ? ? ? ? B9 ? ? ? ? 56 FF 50 14 8B 34 85");
										FullUpdate();

										g_ClientState->ForceFullUpdate();
									}
								}
							}

							//next_enb_time = g_GlobalVars->curtime + 1.f;
						}
					}

				}
				ImGui::EndChild();
			}
			ImGui::PopFont();
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
