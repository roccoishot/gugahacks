#include "chams.hpp"
#include <fstream>

#include "../valve_sdk/csgostructs.hpp"
#include "../options.hpp"
#include "../hooks.hpp"
#include "../helpers/input.hpp"
#include <cstddef>
#include "../backtrack.h"
#include "../xor.h"
#include "Misc.hpp"
#include "../Globals.h"
#ifdef ENABLE_XOR
#define XorStr _xor_ 
#else
#define XorStr
#endif
#pragma intrinsic(_ReturnAddress)  

void Chams::modulate(const Color color, IMaterial* material)
{
	if (!g_EngineClient->IsInGame())
		return;

	if (!g_LocalPlayer)
		return;

	if (material)
	{
		material->ColorModulate(color[0] / 255.f, color[1] / 255.f, color[2] / 255.f);
		const auto tint = material->find_var(XorStr("$envmaptint"));

		if (tint)
		{
			tint->set_vector(Vector(color.r(), color.g(), color.b()));
		}
		material->AlphaModulate(color[3] / 255.f);
		g_RenderView->SetBlend(color[3] / 255.f);
	}
}

IMaterial* Chams::CreateMaterial(bool lit, const std::string& material_data)
{
	static auto created = 0;
	std::string type = lit ? XorStr("VertexLitGeneric") : XorStr("UnlitGeneric");

	auto matname = XorStr("gugahacks_") + std::to_string(created);
	++created;

	auto keyValues = new KeyValues(matname.c_str());
	static auto key_values_address = Utils::PatternScan2(XorStr("client.dll"), XorStr("55 8B EC 51 33 C0 C7 45"));

	using KeyValuesFn = void(__thiscall*)(void*, const char*, int, int);
	reinterpret_cast <KeyValuesFn> (key_values_address)(keyValues, type.c_str(), 0, 0);

	static auto load_from_buffer_address = Utils::PatternScan2(XorStr("client.dll"), XorStr("55 8B EC 83 E4 F8 83 EC 34 53 8B 5D 0C 89"));
	using LoadFromBufferFn = void(__thiscall*)(void*, const char*, const char*, void*, const char*, void*, void*);

	reinterpret_cast <LoadFromBufferFn> (load_from_buffer_address)(keyValues, matname.c_str(), material_data.c_str(), nullptr, nullptr, nullptr, nullptr);

	static auto material = g_MatSystem->CreateMaterial(matname.c_str(), keyValues);
	material->IncrementReferenceCount();

	return material;
}

void Chams::RemovePlayerPatches()
{
	if (!g_Options.chams_player_enabled) return;

	for (size_t i = 0; i < g_EngineClient->GetMaxClients(); i++)
	{
		C_BasePlayer* plr = (C_BasePlayer*)g_EntityList->GetClientEntity(i);

		if (!plr) continue;
		if (!plr->IsAlive()) continue;

		for (size_t patchIndex = 0; patchIndex < 5; patchIndex++)
		{
			plr->m_vecPlayerPatchEconIndices()[patchIndex] = Vector(0, 0, 0);
		}
	}
}

void Chams::OnDrawModelExecute(void* pResults, DrawModelInfo_t* pInfo, matrix3x4_t* pBoneToWorld, float* flpFlexWeights, float* flpFlexDelayedWeights, Vector& vrModelOrigin, int32_t iFlags)
{
	static auto fnDME = Hooks::stdrender_hook.get_original<decltype(&Hooks::hkDrawModelExecute2)>(index::DrawModelExecute2);

	if (!pInfo->m_pClientEntity || !g_LocalPlayer || !g_EngineClient->IsInGame())
		return;

	const auto mdl = pInfo->m_pClientEntity->GetModel();
	//run yo pockets cuh
	bool is_player = (strstr(mdl->szName, XorStr("models/player")) != nullptr && g_Options.chams_player_enabled);
	auto is_weapon = ((strstr(mdl->szName, XorStr("weapons/v_")) || strstr(mdl->szName, XorStr("weapons/w_"))) && !strstr(mdl->szName, XorStr("arms")) && g_Options.chams_strap_enabled);
	auto is_arm = (strstr(mdl->szName, XorStr("arms")) && g_Options.chams_arms_enabled);
	auto is_sleeve = (strstr(mdl->szName, XorStr("sleeve")) && g_Options.chams_arms_enabled);

	static IMaterial* Half = g_MatSystem->FindMaterial(XorStr("materials/models/inventory_items/dogtags/dogtags_lightray"), nullptr); //Like half and half but for shit
	static IMaterial* pulse = g_MatSystem->FindMaterial(XorStr("models/inventory_items/dogtags/dogtags_outline"), nullptr); //Pulse Shit
	static IMaterial* shine = g_MatSystem->FindMaterial(XorStr("models/inventory_items/cologne_prediction/cologne_prediction_glass"), nullptr); // P SHINE
	static IMaterial* othershine = g_MatSystem->FindMaterial(XorStr("models/inventory_items/trophy_majors/gloss"), nullptr); //Light Shine
	static IMaterial* velvet = g_MatSystem->FindMaterial(XorStr("models/inventory_items/trophy_majors/velvet"), nullptr); //Interium Glow
	static IMaterial* shit = (CreateMaterial(true, R"#("VertexLitGeneric"
		    {
	"$additive" "1"
	"$envmap" "models/effects/cube_white"
	"$envmaptint" "[1 1 1]"
	"$envmapfresnel" "1"
	"$envmapfresnelminmaxexp" "[0 1 2]"
	"$alpha" "1.0"
		    }
		)#")); //The actual glow shit
	static IMaterial* intellect = (CreateMaterial(true, R"#("VertexLitGeneric"
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
	static IMaterial* animated2 = (CreateMaterial(true, R"#("VertexLitGeneric"
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

	if (is_player) {
		if (g_EngineClient->IsTakingScreenshot())
			return;

		if (g_Options.chams_player_enabled == false)
			return;

		const auto ent = (C_BasePlayer*)pInfo->m_pClientEntity->GetIClientUnknown()->GetBaseEntity();
		auto class_id = ent->GetClientClass()->m_ClassID;

		if (class_id == ClassId_CCSPlayer)
		{
			if (ent && ent->IsAlive() && !ent->IsDormant()) {

				if (ent->EntIndex() != g_LocalPlayer->EntIndex() && !ent->IsEnemy()) {
					if (g_Options.teamchams) {
						if (g_Options.drawogplaya) {
							fnDME(g_StudioRender, 0, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);
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
								velvet->AlphaModulate(g_Options.color_chams_player_ally_visible[3] / 255.f);
								bool bFound = false;
								auto pVar = velvet->FindVar(XorStr("$envmaptint"), &bFound);
								if (bFound)
								{
									(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.color_chams_player_ally_visible[0] / 255.f, g_Options.color_chams_player_ally_visible[1] / 255.f, g_Options.color_chams_player_ally_visible[2] / 255.f);
								}
								g_MdlRender->ForcedMaterialOverride(velvet);
							}

							if (g_Options.player_material == 4)
							{
								shit->AlphaModulate(g_Options.glowcolor[3] / 255.f);
								bool bFound = false;
								auto pVar = shit->FindVar(XorStr("$envmaptint"), &bFound);
								if (bFound)
								{
									(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.glowcolor[0] / 255.f, g_Options.glowcolor[1] / 255.f, g_Options.glowcolor[2] / 255.f);
								}
								g_MdlRender->ForcedMaterialOverride(shit);
							}

							if (g_Options.player_material == 5)
							{
								dubble->AlphaModulate(g_Options.glowcolor[3] / 255.f);
								bool bFound = false;
								auto pVar = dubble->FindVar(XorStr("$envmaptint"), &bFound);
								if (bFound)
								{
									(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.glowcolor[0] / 255.f, g_Options.glowcolor[1] / 255.f, g_Options.glowcolor[2] / 255.f);
								}
								g_MdlRender->ForcedMaterialOverride(dubble);
							}
							fnDME(g_StudioRender, 0, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);
						}

						static IMaterial* player_enemies_type = nullptr;
						if (g_Options.player_material == 0)
							player_enemies_type = regular;
						if (g_Options.player_material == 1)
							player_enemies_type = flat;
						if (g_Options.player_material > 1)
							player_enemies_type = regular;
						if (g_Options.chams_player_ignorez && g_Options.chams_player_enabled)
						{
							player_enemies_type->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
							modulate(g_Options.color_chams_player_ally_occluded, player_enemies_type);
							g_MdlRender->ForcedMaterialOverride(player_enemies_type);
							fnDME(g_StudioRender, 0, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);
						}
						if (g_Options.chams_player_enabled)
						{
							player_enemies_type->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
							modulate(g_Options.color_chams_player_ally_visible, player_enemies_type);
							/*player_enemies_type->find_var("$phong")->set_string("1");
							player_enemies_type->find_var("$basemapalphaphongmask")->set_string("1");
							player_enemies_type->find_var("$pearlescent")->set_string("6");*/
							g_MdlRender->ForcedMaterialOverride(player_enemies_type);
							fnDME(g_StudioRender, 0, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);

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
								velvet->AlphaModulate(g_Options.color_chams_player_ally_visible[3] / 255.f);
								bool bFound = false;
								auto pVar = velvet->FindVar(XorStr("$envmaptint"), &bFound);
								if (bFound)
								{
									(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.color_chams_player_ally_visible[0] / 255.f, g_Options.color_chams_player_ally_visible[1] / 255.f, g_Options.color_chams_player_ally_visible[2] / 255.f);
								}
								g_MdlRender->ForcedMaterialOverride(velvet);
							}

							if (g_Options.player_material == 4)
							{
								shit->AlphaModulate(g_Options.glowcolor[3] / 255.f);
								bool bFound = false;
								auto pVar = shit->FindVar(XorStr("$envmaptint"), &bFound);
								if (bFound)
								{
									(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.glowcolor[0] / 255.f, g_Options.glowcolor[1] / 255.f, g_Options.glowcolor[2] / 255.f);
								}
								g_MdlRender->ForcedMaterialOverride(shit);
							}

							if (g_Options.player_material == 5)
							{
								dubble->AlphaModulate(g_Options.glowcolor[3] / 255.f);
								bool bFound = false;
								auto pVar = dubble->FindVar(XorStr("$envmaptint"), &bFound);
								if (bFound)
								{
									(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.glowcolor[0] / 255.f, g_Options.glowcolor[1] / 255.f, g_Options.glowcolor[2] / 255.f);
								}
								g_MdlRender->ForcedMaterialOverride(dubble);
							}

						}
					}
				}

				if (ent->EntIndex() == g_LocalPlayer->EntIndex() && !ent->IsEnemy()) {
					if (g_Options.localchams) {

						if (g_Options.drawogplaya) {
							fnDME(g_StudioRender, 0, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);
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
								velvet->AlphaModulate(g_Options.color_chams_player_ally_visible[3] / 255.f);
								bool bFound = false;
								auto pVar = velvet->FindVar(XorStr("$envmaptint"), &bFound);
								if (bFound)
								{
									(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.color_chams_player_local_visible[0] / 255.f, g_Options.color_chams_player_local_visible[1] / 255.f, g_Options.color_chams_player_local_visible[2] / 255.f);
								}
								g_MdlRender->ForcedMaterialOverride(velvet);
							}

							if (g_Options.player_material == 4)
							{
								shit->AlphaModulate(g_Options.glowcolorlocal[3] / 255.f);
								bool bFound = false;
								auto pVar = shit->FindVar(XorStr("$envmaptint"), &bFound);
								if (bFound)
								{
									(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.glowcolorlocal[0] / 255.f, g_Options.glowcolorlocal[1] / 255.f, g_Options.glowcolorlocal[2] / 255.f);
								}
								g_MdlRender->ForcedMaterialOverride(shit);
							}

							if (g_Options.player_material == 5)
							{
								dubble->AlphaModulate(g_Options.glowcolorlocal[3] / 255.f);
								bool bFound = false;
								auto pVar = dubble->FindVar(XorStr("$envmaptint"), &bFound);
								if (bFound)
								{
									(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.glowcolorlocal[0] / 255.f, g_Options.glowcolorlocal[1] / 255.f, g_Options.glowcolorlocal[2] / 255.f);
								}
								g_MdlRender->ForcedMaterialOverride(dubble);
							}

							fnDME(g_StudioRender, 0, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);
						}

						static IMaterial* player_enemies_type = nullptr;
						if (g_Options.player_material == 0)
							player_enemies_type = regular;
						if (g_Options.player_material == 1)
							player_enemies_type = flat;
						if (g_Options.player_material > 1)
							player_enemies_type = regular;
						if (g_Options.chams_player_ignorez && g_Options.chams_player_enabled)
						{
							player_enemies_type->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
							modulate(g_Options.color_chams_player_local_occluded, player_enemies_type);
							g_MdlRender->ForcedMaterialOverride(player_enemies_type);
							fnDME(g_StudioRender, 0, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);
						}
						if (g_Options.chams_player_enabled)
						{
							player_enemies_type->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
							modulate(g_Options.color_chams_player_local_visible, player_enemies_type);
							g_MdlRender->ForcedMaterialOverride(player_enemies_type);
							fnDME(g_StudioRender, 0, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);

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
								velvet->AlphaModulate(g_Options.color_chams_player_ally_visible[3] / 255.f);
								bool bFound = false;
								auto pVar = velvet->FindVar(XorStr("$envmaptint"), &bFound);
								if (bFound)
								{
									(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.color_chams_player_local_visible[0] / 255.f, g_Options.color_chams_player_local_visible[1] / 255.f, g_Options.color_chams_player_local_visible[2] / 255.f);
								}
								g_MdlRender->ForcedMaterialOverride(velvet);
							}

							if (g_Options.player_material == 4)
							{
								shit->AlphaModulate(g_Options.glowcolorlocal[3] / 255.f);
								bool bFound = false;
								auto pVar = shit->FindVar(XorStr("$envmaptint"), &bFound);
								if (bFound)
								{
									(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.glowcolorlocal[0] / 255.f, g_Options.glowcolorlocal[1] / 255.f, g_Options.glowcolorlocal[2] / 255.f);
								}
								g_MdlRender->ForcedMaterialOverride(shit);
							}

							if (g_Options.player_material == 5)
							{
								dubble->AlphaModulate(g_Options.glowcolorlocal[3] / 255.f);
								bool bFound = false;
								auto pVar = dubble->FindVar(XorStr("$envmaptint"), &bFound);
								if (bFound)
								{
									(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.glowcolorlocal[0] / 255.f, g_Options.glowcolorlocal[1] / 255.f, g_Options.glowcolorlocal[2] / 255.f);
								}
								g_MdlRender->ForcedMaterialOverride(dubble);
							}

							Globals::rendered = true;

						}
					}
				}

				if (ent->EntIndex() != g_LocalPlayer->EntIndex() && ent->IsEnemy() && ent->m_iTeamNum() != g_LocalPlayer->m_iTeamNum()) {
					if (g_Options.chams_player_enabled) {
						if (g_Options.drawogplaya) {
							fnDME(g_StudioRender, 0, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);
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
								velvet->AlphaModulate(g_Options.color_chams_player_enemy_visible[3] / 255.f);
								bool bFound = false;
								auto pVar = velvet->FindVar(XorStr("$envmaptint"), &bFound);
								if (bFound)
								{
									(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.color_chams_player_enemy_visible[0] / 255.f, g_Options.color_chams_player_enemy_visible[1] / 255.f, g_Options.color_chams_player_enemy_visible[2] / 255.f);
								}
								g_MdlRender->ForcedMaterialOverride(velvet);
							}

							if (g_Options.player_material == 4)
							{
								shit->AlphaModulate(g_Options.glowcolorenemy[3] / 255.f);
								bool bFound = false;
								auto pVar = shit->FindVar(XorStr("$envmaptint"), &bFound);
								if (bFound)
								{
									(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.glowcolorenemy[0] / 255.f, g_Options.glowcolorenemy[1] / 255.f, g_Options.glowcolorenemy[2] / 255.f);
								}
								g_MdlRender->ForcedMaterialOverride(shit);
							}

							if (g_Options.player_material == 5)
							{
								dubble->AlphaModulate(g_Options.glowcolorenemy[3] / 255.f);
								bool bFound = false;
								auto pVar = dubble->FindVar(XorStr("$envmaptint"), &bFound);
								if (bFound)
								{
									(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.glowcolorenemy[0] / 255.f, g_Options.glowcolorenemy[1] / 255.f, g_Options.glowcolorenemy[2] / 255.f);
								}
								g_MdlRender->ForcedMaterialOverride(dubble);
							}
							fnDME(g_StudioRender, 0, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);
						}

						static IMaterial* player_enemies_type = nullptr;
						if (g_Options.player_material == 0)
							player_enemies_type = regular;
						if (g_Options.player_material == 1)
							player_enemies_type = flat;
						if (g_Options.player_material > 1)
							player_enemies_type = regular;
						if (g_Options.chams_player_ignorez && g_Options.chams_player_enabled)
						{
							player_enemies_type->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
							modulate(g_Options.color_chams_player_enemy_occluded, player_enemies_type);
							g_MdlRender->ForcedMaterialOverride(player_enemies_type);
							fnDME(g_StudioRender, 0, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);
						}
						if (g_Options.chams_player_enabled)
						{
							player_enemies_type->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
							modulate(g_Options.color_chams_player_enemy_visible, player_enemies_type);
							g_MdlRender->ForcedMaterialOverride(player_enemies_type);
							fnDME(g_StudioRender, 0, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);

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
								velvet->AlphaModulate(g_Options.color_chams_player_enemy_visible[3] / 255.f);
								bool bFound = false;
								auto pVar = velvet->FindVar(XorStr("$envmaptint"), &bFound);
								if (bFound)
								{
									(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.color_chams_player_enemy_visible[0] / 255.f, g_Options.color_chams_player_enemy_visible[1] / 255.f, g_Options.color_chams_player_enemy_visible[2] / 255.f);
								}
								g_MdlRender->ForcedMaterialOverride(velvet);
							}

							if (g_Options.player_material == 4)
							{
								shit->AlphaModulate(g_Options.glowcolorenemy[3] / 255.f);
								bool bFound = false;
								auto pVar = shit->FindVar(XorStr("$envmaptint"), &bFound);
								if (bFound)
								{
									(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.glowcolorenemy[0] / 255.f, g_Options.glowcolorenemy[1] / 255.f, g_Options.glowcolorenemy[2] / 255.f);
								}
								g_MdlRender->ForcedMaterialOverride(shit);
							}

							if (g_Options.player_material == 5)
							{
								dubble->AlphaModulate(g_Options.glowcolorenemy[3] / 255.f);
								bool bFound = false;
								auto pVar = dubble->FindVar(XorStr("$envmaptint"), &bFound);
								if (bFound)
								{
									(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.glowcolorenemy[0] / 255.f, g_Options.glowcolorenemy[1] / 255.f, g_Options.glowcolorenemy[2] / 255.f);
								}
								g_MdlRender->ForcedMaterialOverride(dubble);
							}

						}
					}
				}
			}
		}
	}

	if (is_arm) {

		if (g_EngineClient->IsTakingScreenshot())
			return;

		if (g_Options.chams_player_enabled == false)
			return;

		if (g_Options.chams_arms_enabled == false)
			return;

		if (is_player)
			return;

		static IMaterial* player_enemies_type = nullptr;
		if (g_Options.player_material == 0)
			player_enemies_type = regular;
		if (g_Options.player_material == 1)
			player_enemies_type = flat;
		if (g_Options.player_material > 1)
			player_enemies_type = regular;
		if (g_Options.chams_arms_enabled)
		{
			if (g_Options.drawogplaya)
				fnDME(g_StudioRender, 0, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);

			player_enemies_type->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
			modulate(Color(g_Options.color_chams_arms_visible), player_enemies_type);
			g_MdlRender->ForcedMaterialOverride(player_enemies_type);
			fnDME(g_StudioRender, 0, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);

			if (g_Options.arms_material == 2)
			{
				othershine->AlphaModulate(g_Options.ohthuhshine[3] / 255.f);
				bool bFound = false;
				auto pVar = othershine->FindVar(XorStr("$envmaptint"), &bFound);
				if (bFound)
				{
					(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.ohthuhshine[0] / 255.f, g_Options.ohthuhshine[1] / 255.f, g_Options.ohthuhshine[2] / 255.f);
				}
				g_MdlRender->ForcedMaterialOverride(othershine);

			}

			if (g_Options.arms_material == 3)
			{
				shit->AlphaModulate(g_Options.glowcolorarms[3] / 255.f);
				bool bFound = false;
				auto pVar = shit->FindVar(XorStr("$envmaptint"), &bFound);
				if (bFound)
				{
					(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.glowcolorarms[0] / 255.f, g_Options.glowcolorarms[1] / 255.f, g_Options.glowcolorarms[2] / 255.f);
				}
				g_MdlRender->ForcedMaterialOverride(shit);

			}

			if (g_Options.arms_material == 4)
			{
				dubble->AlphaModulate(g_Options.glowcolorarms[3] / 255.f);
				bool bFound = false;
				auto pVar = dubble->FindVar(XorStr("$envmaptint"), &bFound);
				if (bFound)
				{
					(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.glowcolorarms[0] / 255.f, g_Options.glowcolorarms[1] / 255.f, g_Options.glowcolorarms[2] / 255.f);
				}
				g_MdlRender->ForcedMaterialOverride(dubble);

			}

			if (g_Options.arms_material == 5) {
				velvet->AlphaModulate(g_Options.color_chams_arms_visible[3] / 255.f);
				bool bFound = false;
				auto pVar = velvet->FindVar(XorStr("$envmaptint"), &bFound);
				if (bFound)
				{
					(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.color_chams_arms_visible[0] / 255.f, g_Options.color_chams_arms_visible[1] / 255.f, g_Options.color_chams_arms_visible[2] / 255.f);
				}
				g_MdlRender->ForcedMaterialOverride(velvet);

			}

			if (g_Options.arms_material == 6) {
				animated2->ColorModulate(g_Options.glowcolorstrap[0] / 255.f, g_Options.glowcolorstrap[1] / 255.f, g_Options.glowcolorstrap[2] / 255.f);
				animated2->AlphaModulate(g_Options.glowcolorstrap[3] / 255.f);
				g_MdlRender->ForcedMaterialOverride(animated2);

			}

		}
	}

	if (is_sleeve) {

		if (g_EngineClient->IsTakingScreenshot())
			return;

		if (g_Options.chams_player_enabled == false)
			return;

		if (g_Options.chams_arms_enabled == false)
			return;

		if (is_player)
			return;

		static IMaterial* player_enemies_type = nullptr;
		if (g_Options.player_material == 0)
			player_enemies_type = regular;
		if (g_Options.player_material == 1)
			player_enemies_type = flat;
		if (g_Options.player_material > 1)
			player_enemies_type = regular;

		if (g_Options.chams_arms_enabled)
		{
			if (g_Options.drawogplaya)
				fnDME(g_StudioRender, 0, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);

			player_enemies_type->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
			modulate(g_Options.color_chams_sleeve_visible, player_enemies_type);
			g_MdlRender->ForcedMaterialOverride(player_enemies_type);
			fnDME(g_StudioRender, 0, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);

			if (g_Options.arms_material == 2)
			{
				shine->AlphaModulate(g_Options.ohthuhshine[3] / 255.f);
				bool bFound = false;
				auto pVar = shine->FindVar(XorStr("$envmaptint"), &bFound);
				if (bFound)
				{
					(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.player_enemy_visible_shine[0] / 255.f, g_Options.player_enemy_visible_shine[1] / 255.f, g_Options.player_enemy_visible_shine[2] / 255.f);
				}
				g_MdlRender->ForcedMaterialOverride(shine);
			}

			if (g_Options.arms_material == 3)
			{
				shit->AlphaModulate(g_Options.glowcolorarms[3] / 255.f);
				bool bFound = false;
				auto pVar = shit->FindVar(XorStr("$envmaptint"), &bFound);
				if (bFound)
				{
					(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.glowcolorarms[0] / 255.f, g_Options.glowcolorarms[1] / 255.f, g_Options.glowcolorarms[2] / 255.f);
				}
				g_MdlRender->ForcedMaterialOverride(shit);
			}

			if (g_Options.arms_material == 4)
			{
				dubble->AlphaModulate(g_Options.glowcolorarms[3] / 255.f);
				bool bFound = false;
				auto pVar = dubble->FindVar(XorStr("$envmaptint"), &bFound);
				if (bFound)
				{
					(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.glowcolorarms[0] / 255.f, g_Options.glowcolorarms[1] / 255.f, g_Options.glowcolorarms[2] / 255.f);
				}
				g_MdlRender->ForcedMaterialOverride(dubble);
			}

			if (g_Options.arms_material == 5)
			{
				velvet->AlphaModulate(g_Options.color_chams_player_enemy_visible[3] / 255.f);
				bool bFound = false;
				auto pVar = velvet->FindVar(XorStr("$envmaptint"), &bFound);
				if (bFound)
				{
					(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.color_chams_sleeve_visible[0] / 255.f, g_Options.color_chams_sleeve_visible[1] / 255.f, g_Options.color_chams_sleeve_visible[2] / 255.f);
				}
				g_MdlRender->ForcedMaterialOverride(velvet);
			}

		}
	}

	if (is_weapon) {

		if (g_EngineClient->IsTakingScreenshot())
			return;

		if (g_Options.chams_player_enabled == false)
			return;

		if (g_Options.chams_strap_enabled == false)
			return;

		if (is_arm || is_sleeve)
			return;

		auto weapon = g_LocalPlayer->m_hActiveWeapon().Get();

		if (weapon)
		{
			if (weapon->m_iItemDefinitionIndex() == WEAPON_AUG || weapon->m_iItemDefinitionIndex() == WEAPON_SG556)
			{
				if (g_LocalPlayer->m_bIsScoped())
					return;
			}
		}

		static IMaterial* player_enemies_type = nullptr;
		if (g_Options.strap_material == 0)
			player_enemies_type = regular;
		if (g_Options.strap_material == 1)
			player_enemies_type = flat;
		if (g_Options.strap_material > 1)
			player_enemies_type = regular;

		if (g_Options.chams_strap_enabled)
		{
			if (g_Options.drawogplaya)
				fnDME(g_StudioRender, 0, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);

			player_enemies_type->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
			modulate(Color(g_Options.color_chams_strap_visible), player_enemies_type);
			g_MdlRender->ForcedMaterialOverride(player_enemies_type);
			fnDME(g_StudioRender, 0, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);

			if (g_Options.strap_material == 2)
			{
				othershine->AlphaModulate(g_Options.ohthuhshine[3] / 255.f);
				bool bFound = false;
				auto pVar = othershine->FindVar(XorStr("$envmaptint"), &bFound);
				if (bFound)
				{
					(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.ohthuhshine[0] / 255.f, g_Options.ohthuhshine[1] / 255.f, g_Options.ohthuhshine[2] / 255.f);
				}
				g_MdlRender->ForcedMaterialOverride(othershine);

			}

			if (g_Options.strap_material == 3)
			{
				shit->AlphaModulate(g_Options.glowcolorstrap[3] / 255.f);
				bool bFound = false;
				auto pVar = shit->FindVar(XorStr("$envmaptint"), &bFound);
				if (bFound)
				{
					(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.glowcolorstrap[0] / 255.f, g_Options.glowcolorstrap[1] / 255.f, g_Options.glowcolorstrap[2] / 255.f);
				}
				g_MdlRender->ForcedMaterialOverride(shit);

			}

			if (g_Options.strap_material == 4)
			{
				dubble->AlphaModulate(g_Options.glowcolorstrap[3] / 255.f);
				bool bFound = false;
				auto pVar = dubble->FindVar(XorStr("$envmaptint"), &bFound);
				if (bFound)
				{
					(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.glowcolorstrap[0] / 255.f, g_Options.glowcolorstrap[1] / 255.f, g_Options.glowcolorstrap[2] / 255.f);
				}
				g_MdlRender->ForcedMaterialOverride(dubble);

			}

			if (g_Options.strap_material == 5) {
				velvet->AlphaModulate(g_Options.color_chams_strap_visible[3] / 255.f);
				bool bFound = false;
				auto pVar = velvet->FindVar(XorStr("$envmaptint"), &bFound);
				if (bFound)
				{
					(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.color_chams_strap_visible[0] / 255.f, g_Options.color_chams_strap_visible[1] / 255.f, g_Options.color_chams_strap_visible[2] / 255.f);
				}
				g_MdlRender->ForcedMaterialOverride(velvet);

			}

			if (g_Options.strap_material == 6) {
				animated2->ColorModulate(g_Options.glowcolorstrap[0] / 255.f, g_Options.glowcolorstrap[1] / 255.f, g_Options.glowcolorstrap[2] / 255.f);
				animated2->AlphaModulate(g_Options.glowcolorstrap[3] / 255.f);
				g_MdlRender->ForcedMaterialOverride(animated2);

			}

		}
	}

}