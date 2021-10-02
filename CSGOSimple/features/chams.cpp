#include "chams.hpp"
#include <fstream>

#include "../valve_sdk/csgostructs.hpp"
#include "../options.hpp"
#include "../hooks.hpp"
#include "../helpers/input.hpp"

void modulate(const Color color, IMaterial* material)
{
	if (!g_EngineClient->IsInGame())
		return;

	if (material)
	{
		material->ColorModulate(color[0] / 255.f, color[1] / 255.f, color[2] / 255.f);
		material->AlphaModulate(color[3] / 255.f);
		const auto tint = material->find_var("$envmaptint");

		if (tint)
		{
			tint->set_vector(Vector(color.r(), color.g(), color.b()));
		}
	}

	g_RenderView->SetColorModulation(color[0] / 255.f, color[1] / 255.f, color[2] / 255.f);
}

void Chams::OnDrawModelExecute(void* pResults, DrawModelInfo_t* pInfo, matrix3x4_t* pBoneToWorld, float* flpFlexWeights, float* flpFlexDelayedWeights, Vector& vrModelOrigin, int32_t iFlags)
{

	static auto fnDME = Hooks::stdrender_hook.get_original<decltype(&Hooks::hkDrawModelExecute2)>(index::DrawModelExecute2);

	if (!pInfo->m_pClientEntity || !g_LocalPlayer)
		return;

	const auto mdl = pInfo->m_pClientEntity->GetModel();
	//run yo pockets cuh
	bool is_weapon = strstr(mdl->szName, "weapon") != nullptr;
	bool is_arm = strstr(mdl->szName, "arms") != nullptr;
	bool is_player = strstr(mdl->szName, "models/player") != nullptr;
	bool is_sleeve = strstr(mdl->szName, "sleeve") != nullptr;
	std::ofstream("csgo\\materials\\glowOverlay.vmt") << R"#("VertexLitGeneric"
{
    "$additive" "1"
    "$envmap" "models/effects/cube_white"
    "$envmaptint" "[0 0.5 1]"
    "$envmapfresnel" "1"
    "$envmapfresnelminmaxexp" "[0 1 2]"
    "$alpha" "1.0"
}
)#";
	//materials\models\inventory_items\dogtags\dogtags_lightray.vmt
	// save for l8r //static IMaterial* pulse = g_MatSystem->FindMaterial("models/inventory_items/dogtags/dogtags_outline", TEXTURE_GROUP_OTHER);
	static IMaterial* shine = g_MatSystem->FindMaterial("models/inventory_items/trophy_majors/gloss", TEXTURE_GROUP_OTHER);
	static IMaterial* velvet = g_MatSystem->FindMaterial("models/inventory_items/trophy_majors/velvet", TEXTURE_GROUP_OTHER);
	static IMaterial* animated = g_MatSystem->FindMaterial("models/inventory_items/music_kit/darude_01/mp3_detail", TEXTURE_GROUP_OTHER);
	static IMaterial* shit = g_MatSystem->FindMaterial("glowOverlay", TEXTURE_GROUP_MODEL);
	static IMaterial* dubble = g_MatSystem->FindMaterial("dev/glow_armsrace", TEXTURE_GROUP_MODEL);
	if (is_player) {
		// 
		// Draw player Chams.
		// 
		const auto ent = (C_BasePlayer*)pInfo->m_pClientEntity->GetIClientUnknown()->GetBaseEntity();
		auto class_id = ent->GetClientClass()->m_ClassID;
		if (class_id == ClassId_CCSPlayer)
		{
			if (ent && ent->IsAlive()) {
				if (g_Options.teamchams) {
					static IMaterial* player_enemies_type = nullptr;
					if (g_Options.player_material == 0)
						player_enemies_type = g_MatSystem->FindMaterial("debug/debugambientcube", TEXTURE_GROUP_MODEL);
					if (g_Options.player_material == 1)
						player_enemies_type = g_MatSystem->FindMaterial("debug/debugdrawflat", TEXTURE_GROUP_MODEL);
					if (g_Options.player_material > 1)
						player_enemies_type = g_MatSystem->FindMaterial("debug/debugambientcube", TEXTURE_GROUP_MODEL);
					static IMaterial* player_enemies_occluded_type = nullptr;
					if (g_Options.player_material == 0)
						player_enemies_occluded_type = g_MatSystem->FindMaterial("debug/debugambientcube", TEXTURE_GROUP_MODEL);
					if (g_Options.player_material == 1)
						player_enemies_occluded_type = g_MatSystem->FindMaterial("debug/debugdrawflat", TEXTURE_GROUP_MODEL);
					if (g_Options.player_material > 1)
						player_enemies_occluded_type = g_MatSystem->FindMaterial("debug/debugambientcube", TEXTURE_GROUP_MODEL);

					if (!g_Options.player_material == 1) {
						if (!player_enemies_type->IsPrecached())
							return;

						if (!player_enemies_occluded_type->IsPrecached())
							return;
					}

					if (player_enemies_type != nullptr && player_enemies_occluded_type != nullptr && shine != nullptr && velvet != nullptr)
					{
						if (g_Options.chams_player_ignorez && g_Options.chams_player_enabled)
						{
							player_enemies_occluded_type->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
							modulate(g_Options.color_chams_player_ally_occluded, player_enemies_occluded_type);
							g_StudioRender->ForcedMaterialOverride(player_enemies_occluded_type);
							fnDME(g_StudioRender, 0, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);
						}
						if (g_Options.chams_player_enabled)
						{
							player_enemies_type->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
							modulate(g_Options.color_chams_player_ally_visible, player_enemies_type);
							g_StudioRender->ForcedMaterialOverride(player_enemies_type);
							fnDME(g_StudioRender, 0, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);

							if (g_Options.player_material == 2)
							{
								shine->AlphaModulate(g_Options.player_enemy_visible_shine[3] / 255.f);
								bool bFound = false;
								auto pVar = shine->FindVar("$envmaptint", &bFound);
								if (bFound)
								{
									(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.player_enemy_visible_shine[0] / 255.f, g_Options.player_enemy_visible_shine[1] / 255.f, g_Options.player_enemy_visible_shine[2] / 255.f);
								}
								g_MdlRender->ForcedMaterialOverride(shine);
							}

							if (g_Options.player_material == 3)
							{
								velvet->AlphaModulate(g_Options.player_enemy_visible_shine[3] / 255.f);
								bool bFound = false;
								auto pVar = velvet->FindVar("$envmaptint", &bFound);
								if (bFound)
								{
									(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.color_chams_player_ally_visible[0] / 255.f, g_Options.color_chams_player_ally_visible[1] / 255.f, g_Options.color_chams_player_ally_visible[2] / 255.f);
								}
								g_MdlRender->ForcedMaterialOverride(velvet);
							}

							if (g_Options.player_material == 4)
							{
								animated->AlphaModulate(g_Options.player_enemy_visible_shine[3] / 255.f);
								bool bFound = false;
								auto pVar = animated->FindVar("$envmaptint", &bFound);
								if (bFound)
								{
									(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.player_enemy_visible_shine[0] / 255.f, g_Options.player_enemy_visible_shine[1] / 255.f, g_Options.player_enemy_visible_shine[2] / 255.f);
								}
								g_MdlRender->ForcedMaterialOverride(animated);
							}

							if (g_Options.player_material == 5)
							{
								shit->AlphaModulate(g_Options.glowcolor[3] / 255.f);
								bool bFound = false;
								auto pVar = shit->FindVar("$envmaptint", &bFound);
								if (bFound)
								{
									(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.glowcolor[0] / 255.f, g_Options.glowcolor[1] / 255.f, g_Options.glowcolor[2] / 255.f);
								}
								g_MdlRender->ForcedMaterialOverride(shit);
							}

							if (g_Options.player_material == 6)
							{
								dubble->AlphaModulate(g_Options.doublecolor[3] / 255.f);
								bool bFound = false;
								auto pVar = dubble->FindVar("$envmaptint", &bFound);
								if (bFound)
								{
									(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.doublecolor[0] / 255.f, g_Options.doublecolor[1] / 255.f, g_Options.doublecolor[2] / 255.f);
								}
								g_MdlRender->ForcedMaterialOverride(dubble);
							}

						}
					}
				}

				const auto enemy = ent->IsEnemy();
				if (enemy)
				{
					constexpr int MAX_PATCHES = 5;

					if (!g_Options.chams_player_enabled && !g_Options.chams_player_ignorez)
					{
						{
							for (size_t k = 0; k < MAX_PATCHES; k++)
							{
								NULL;
							}
						}
						return;
					}
					else
					{
						static IMaterial* player_enemies_type = nullptr;
						if (g_Options.player_material == 0)
							player_enemies_type = g_MatSystem->FindMaterial("debug/debugambientcube", TEXTURE_GROUP_MODEL);
						if (g_Options.player_material == 1)
							player_enemies_type = g_MatSystem->FindMaterial("debug/debugdrawflat", TEXTURE_GROUP_MODEL);
						if (g_Options.player_material > 1)
							player_enemies_type = g_MatSystem->FindMaterial("debug/debugambientcube", TEXTURE_GROUP_MODEL);
						static IMaterial* player_enemies_occluded_type = nullptr;
						if (g_Options.player_material == 0)
							player_enemies_occluded_type = g_MatSystem->FindMaterial("debug/debugambientcube", TEXTURE_GROUP_MODEL);
						if (g_Options.player_material == 1)
							player_enemies_occluded_type = g_MatSystem->FindMaterial("debug/debugdrawflat", TEXTURE_GROUP_MODEL);
						if (g_Options.player_material > 1)
							player_enemies_occluded_type = g_MatSystem->FindMaterial("debug/debugambientcube", TEXTURE_GROUP_MODEL);

						if (!g_Options.player_material == 1) {
							if (!player_enemies_type->IsPrecached())
								return;

							if (!player_enemies_occluded_type->IsPrecached())
								return;
						}

						if (player_enemies_type != nullptr && player_enemies_occluded_type != nullptr && shine != nullptr && velvet != nullptr)
						{
							if (g_Options.chams_player_ignorez && g_Options.chams_player_enabled)
							{
								player_enemies_occluded_type->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
								modulate(g_Options.color_chams_player_enemy_occluded, player_enemies_occluded_type);
								g_StudioRender->ForcedMaterialOverride(player_enemies_occluded_type);
								fnDME(g_StudioRender, 0, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);
							}
							if (g_Options.chams_player_enabled)
							{
								player_enemies_type->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
								modulate(g_Options.color_chams_player_enemy_visible, player_enemies_type);
								g_StudioRender->ForcedMaterialOverride(player_enemies_type);
								fnDME(g_StudioRender, 0, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);

								if (g_Options.player_material == 2)
								{
									shine->AlphaModulate(g_Options.player_enemy_visible_shine[3] / 255.f);
									bool bFound = false;
									auto pVar = shine->FindVar("$envmaptint", &bFound);
									if (bFound)
									{
										(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.player_enemy_visible_shine[0] / 255.f, g_Options.player_enemy_visible_shine[1] / 255.f, g_Options.player_enemy_visible_shine[2] / 255.f);
									}
									g_MdlRender->ForcedMaterialOverride(shine);
								}

								if (g_Options.player_material == 3)
								{
									velvet->AlphaModulate(g_Options.player_enemy_visible_shine[3] / 255.f);
									bool bFound = false;
									auto pVar = velvet->FindVar("$envmaptint", &bFound);
									if (bFound)
									{
										(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.color_chams_player_enemy_visible[0] / 255.f, g_Options.color_chams_player_enemy_visible[1] / 255.f, g_Options.color_chams_player_enemy_visible[2] / 255.f);
									}
									g_MdlRender->ForcedMaterialOverride(velvet);
								}

								if (g_Options.player_material == 4)
								{
									animated->AlphaModulate(g_Options.player_enemy_visible_shine[3] / 255.f);
									bool bFound = false;
									auto pVar = animated->FindVar("$envmaptint", &bFound);
									if (bFound)
									{
										(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.player_enemy_visible_shine[0] / 255.f, g_Options.player_enemy_visible_shine[1] / 255.f, g_Options.player_enemy_visible_shine[2] / 255.f);
									}
									g_MdlRender->ForcedMaterialOverride(animated);
								}

								if (g_Options.player_material == 5)
								{
									shit->AlphaModulate(g_Options.glowcolorenemy[3] / 255.f);
									bool bFound = false;
									auto pVar = shit->FindVar("$envmaptint", &bFound);
									if (bFound)
									{
										(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.glowcolorenemy[0] / 255.f, g_Options.glowcolorenemy[1] / 255.f, g_Options.glowcolorenemy[2] / 255.f);
									}
									g_MdlRender->ForcedMaterialOverride(shit);
								}

								if (g_Options.player_material == 6)
								{
									dubble->AlphaModulate(g_Options.doublecolorenemy[3] / 255.f);
									bool bFound = false;
									auto pVar = dubble->FindVar("$envmaptint", &bFound);
									if (bFound)
									{
										(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.doublecolorenemy[0] / 255.f, g_Options.doublecolorenemy[1] / 255.f, g_Options.doublecolorenemy[2] / 255.f);
									}
									g_MdlRender->ForcedMaterialOverride(dubble);
								}

							}
						}
						else
						{
							return;
						}
					}
				}
			}
		}
	}

	if (is_arm) {
		static IMaterial* player_enemies_type = nullptr;
		if (g_Options.arms_material == 0)
			player_enemies_type = g_MatSystem->FindMaterial("debug/debugambientcube", TEXTURE_GROUP_MODEL);
		if (g_Options.arms_material == 1)
			player_enemies_type = g_MatSystem->FindMaterial("debug/debugdrawflat", TEXTURE_GROUP_MODEL);
		if (g_Options.arms_material > 1)
			player_enemies_type = g_MatSystem->FindMaterial("debug/debugambientcube", TEXTURE_GROUP_MODEL);
		static IMaterial* player_enemies_occluded_type = nullptr;
		if (g_Options.arms_material == 0)
			player_enemies_occluded_type = g_MatSystem->FindMaterial("debug/debugambientcube", TEXTURE_GROUP_MODEL);
		if (g_Options.arms_material == 1)
			player_enemies_occluded_type = g_MatSystem->FindMaterial("debug/debugdrawflat", TEXTURE_GROUP_MODEL);
		if (g_Options.arms_material > 1)
			player_enemies_occluded_type = g_MatSystem->FindMaterial("debug/debugambientcube", TEXTURE_GROUP_MODEL);

		if (!g_Options.arms_material == 1) {
			if (!player_enemies_type->IsPrecached())
				return;

			if (!player_enemies_occluded_type->IsPrecached())
				return;
		}

		if (player_enemies_type != nullptr && player_enemies_occluded_type != nullptr && shine != nullptr && velvet != nullptr)
		{
			if (g_Options.chams_arms_ignorez && g_Options.chams_arms_enabled)
			{
				player_enemies_occluded_type->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, (g_Options.chams_arms_ignorez));
				modulate(g_Options.color_chams_arms_occluded, player_enemies_occluded_type);
				g_StudioRender->ForcedMaterialOverride(player_enemies_occluded_type);
				fnDME(g_StudioRender, 0, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);
			}
			if (g_Options.chams_arms_enabled)
			{
				player_enemies_type->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
				modulate(g_Options.color_chams_arms_visible, player_enemies_type);
				g_StudioRender->ForcedMaterialOverride(player_enemies_type);
				fnDME(g_StudioRender, 0, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);

				if (g_Options.arms_material == 2)
				{
					shine->AlphaModulate(g_Options.player_enemy_visible_shine[3] / 255.f);
					bool bFound = false;
					auto pVar = shine->FindVar("$envmaptint", &bFound);
					if (bFound)
					{
						(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.player_enemy_visible_shine[0] / 255.f, g_Options.player_enemy_visible_shine[1] / 255.f, g_Options.player_enemy_visible_shine[2] / 255.f);
					}
					g_MdlRender->ForcedMaterialOverride(shine);
				}

				if (g_Options.arms_material == 3)
				{
					animated->AlphaModulate(g_Options.player_enemy_visible_shine[3] / 255.f);
					bool bFound = false;
					auto pVar = animated->FindVar("$envmaptint", &bFound);
					if (bFound)
					{
						(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.player_enemy_visible_shine[0] / 255.f, g_Options.player_enemy_visible_shine[1] / 255.f, g_Options.player_enemy_visible_shine[2] / 255.f);
					}
					g_MdlRender->ForcedMaterialOverride(animated);
				}

				if (g_Options.arms_material == 4)
				{
					shit->AlphaModulate(g_Options.glowcolorarms[3] / 255.f);
					bool bFound = false;
					auto pVar = shit->FindVar("$envmaptint", &bFound);
					if (bFound)
					{
						(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.glowcolorarms[0] / 255.f, g_Options.glowcolorarms[1] / 255.f, g_Options.glowcolorarms[2] / 255.f);
					}
					g_MdlRender->ForcedMaterialOverride(shit);
				}

				if (g_Options.arms_material == 5)
				{
					dubble->AlphaModulate(g_Options.doublecolorarms[3] / 255.f);
					bool bFound = false;
					auto pVar = dubble->FindVar("$envmaptint", &bFound);
					if (bFound)
					{
						(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.doublecolorarms[0] / 255.f, g_Options.doublecolorarms[1] / 255.f, g_Options.doublecolorarms[2] / 255.f);
					}
					g_MdlRender->ForcedMaterialOverride(dubble);
				}

			}
		}
	}
	if (is_sleeve) {
		static IMaterial* player_enemies_type = nullptr;
		if (g_Options.arms_material == 0)
			player_enemies_type = g_MatSystem->FindMaterial("debug/debugambientcube", TEXTURE_GROUP_MODEL);
		if (g_Options.arms_material == 1)
			player_enemies_type = g_MatSystem->FindMaterial("debug/debugdrawflat", TEXTURE_GROUP_MODEL);
		if (g_Options.arms_material > 1)
			player_enemies_type = g_MatSystem->FindMaterial("debug/debugambientcube", TEXTURE_GROUP_MODEL);
		static IMaterial* player_enemies_occluded_type = nullptr;
		if (g_Options.arms_material == 0)
			player_enemies_occluded_type = g_MatSystem->FindMaterial("debug/debugambientcube", TEXTURE_GROUP_MODEL);
		if (g_Options.arms_material == 1)
			player_enemies_occluded_type = g_MatSystem->FindMaterial("debug/debugdrawflat", TEXTURE_GROUP_MODEL);
		if (g_Options.arms_material > 1)
			player_enemies_occluded_type = g_MatSystem->FindMaterial("debug/debugambientcube", TEXTURE_GROUP_MODEL);

		if (!g_Options.arms_material == 1) {
			if (!player_enemies_type->IsPrecached())
				return;

			if (!player_enemies_occluded_type->IsPrecached())
				return;
		}

		if (player_enemies_type != nullptr && player_enemies_occluded_type != nullptr && shine != nullptr && velvet != nullptr)
		{
			if (g_Options.chams_sleeve_ignorez && g_Options.chams_sleeve_enabled)
			{
				player_enemies_occluded_type->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, (g_Options.chams_arms_ignorez));
				modulate(g_Options.color_chams_sleeve_occluded, player_enemies_occluded_type);
				g_StudioRender->ForcedMaterialOverride(player_enemies_occluded_type);
				fnDME(g_StudioRender, 0, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);
			}
			if (g_Options.chams_sleeve_enabled)
			{
				player_enemies_type->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
				modulate(g_Options.color_chams_sleeve_visible, player_enemies_type);
				g_StudioRender->ForcedMaterialOverride(player_enemies_type);
				fnDME(g_StudioRender, 0, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);

				if (g_Options.arms_material == 2)
				{
					shine->AlphaModulate(g_Options.player_enemy_visible_shine[3] / 255.f);
					bool bFound = false;
					auto pVar = shine->FindVar("$envmaptint", &bFound);
					if (bFound)
					{
						(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.player_enemy_visible_shine[0] / 255.f, g_Options.player_enemy_visible_shine[1] / 255.f, g_Options.player_enemy_visible_shine[2] / 255.f);
					}
					g_MdlRender->ForcedMaterialOverride(shine);
				}

				if (g_Options.arms_material == 3)
				{
					animated->AlphaModulate(g_Options.player_enemy_visible_shine[3] / 255.f);
					bool bFound = false;
					auto pVar = animated->FindVar("$envmaptint", &bFound);
					if (bFound)
					{
						(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.player_enemy_visible_shine[0] / 255.f, g_Options.player_enemy_visible_shine[1] / 255.f, g_Options.player_enemy_visible_shine[2] / 255.f);
					}
					g_MdlRender->ForcedMaterialOverride(animated);
				}

				if (g_Options.arms_material == 4)
				{
					shit->AlphaModulate(g_Options.glowcolorarms[3] / 255.f);
					bool bFound = false;
					auto pVar = shit->FindVar("$envmaptint", &bFound);
					if (bFound)
					{
						(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.glowcolorarms[0] / 255.f, g_Options.glowcolorarms[1] / 255.f, g_Options.glowcolorarms[2] / 255.f);
					}
					g_MdlRender->ForcedMaterialOverride(shit);
				}

				if (g_Options.arms_material == 5)
				{
					dubble->AlphaModulate(g_Options.doublecolorarms[3] / 255.f);
					bool bFound = false;
					auto pVar = dubble->FindVar("$envmaptint", &bFound);
					if (bFound)
					{
						(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.doublecolorarms[0] / 255.f, g_Options.doublecolorarms[1] / 255.f, g_Options.doublecolorarms[2] / 255.f);
					}
					g_MdlRender->ForcedMaterialOverride(dubble);
				}

			}
		}
	}

	if (is_weapon) {

		if (is_arm) {
			return;
		}
		if (is_sleeve) {
			return;
		}

		static IMaterial* player_enemies_type = nullptr;
		if (g_Options.strap_material == 0)
			player_enemies_type = g_MatSystem->FindMaterial("debug/debugambientcube", TEXTURE_GROUP_MODEL);
		if (g_Options.strap_material == 1)
			player_enemies_type = g_MatSystem->FindMaterial("debug/debugdrawflat", TEXTURE_GROUP_MODEL);
		if (g_Options.strap_material > 1)
			player_enemies_type = g_MatSystem->FindMaterial("debug/debugambientcube", TEXTURE_GROUP_MODEL);
		static IMaterial* player_enemies_occluded_type = nullptr;
		if (g_Options.strap_material == 0)
			player_enemies_occluded_type = g_MatSystem->FindMaterial("debug/debugambientcube", TEXTURE_GROUP_MODEL);
		if (g_Options.strap_material == 1)
			player_enemies_occluded_type = g_MatSystem->FindMaterial("debug/debugdrawflat", TEXTURE_GROUP_MODEL);
		if (g_Options.strap_material > 1)
			player_enemies_occluded_type = g_MatSystem->FindMaterial("debug/debugambientcube", TEXTURE_GROUP_MODEL);

		if (!g_Options.strap_material == 1) {
			if (!player_enemies_type->IsPrecached())
				return;

			if (!player_enemies_occluded_type->IsPrecached())
				return;
		}

		if (player_enemies_type != nullptr && player_enemies_occluded_type != nullptr && shine != nullptr && velvet != nullptr)
		{
			if (g_Options.chams_strap_ignorez && g_Options.chams_strap_enabled)
			{
				player_enemies_occluded_type->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, (g_Options.chams_strap_ignorez));
				modulate(g_Options.color_chams_strap_occluded, player_enemies_occluded_type);
				g_StudioRender->ForcedMaterialOverride(player_enemies_occluded_type);
				fnDME(g_StudioRender, 0, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);
			}
			if (g_Options.chams_strap_enabled)
			{
				player_enemies_type->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
				modulate(g_Options.color_chams_strap_visible, player_enemies_type);
				g_StudioRender->ForcedMaterialOverride(player_enemies_type);
				fnDME(g_StudioRender, 0, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);

				if (g_Options.strap_material == 2)
				{
					shine->AlphaModulate(g_Options.player_enemy_visible_shine[3] / 255.f);
					bool bFound = false;
					auto pVar = shine->FindVar("$envmaptint", &bFound);
					if (bFound)
					{
						(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.player_enemy_visible_shine[0] / 255.f, g_Options.player_enemy_visible_shine[1] / 255.f, g_Options.player_enemy_visible_shine[2] / 255.f);
					}
					g_MdlRender->ForcedMaterialOverride(shine);
				}

				if (g_Options.strap_material == 3)
				{
					animated->AlphaModulate(g_Options.player_enemy_visible_shine[3] / 255.f);
					bool bFound = false;
					auto pVar = animated->FindVar("$envmaptint", &bFound);
					if (bFound)
					{
						(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.player_enemy_visible_shine[0] / 255.f, g_Options.player_enemy_visible_shine[1] / 255.f, g_Options.player_enemy_visible_shine[2] / 255.f);
					}
					g_MdlRender->ForcedMaterialOverride(animated);
				}

				if (g_Options.strap_material == 4)
				{
					shit->AlphaModulate(g_Options.glowcolorstrap[3] / 255.f);
					bool bFound = false;
					auto pVar = shit->FindVar("$envmaptint", &bFound);
					if (bFound)
					{
						(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.glowcolorstrap[0] / 255.f, g_Options.glowcolorstrap[1] / 255.f, g_Options.glowcolorstrap[2] / 255.f);
					}
					g_MdlRender->ForcedMaterialOverride(shit);
				}

				if (g_Options.strap_material == 5)
				{
					dubble->AlphaModulate(g_Options.doublecolorstrap[3] / 255.f);
					bool bFound = false;
					auto pVar = dubble->FindVar("$envmaptint", &bFound);
					if (bFound)
					{
						(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.doublecolorstrap[0] / 255.f, g_Options.doublecolorstrap[1] / 255.f, g_Options.doublecolorstrap[2] / 255.f);
					}
					g_MdlRender->ForcedMaterialOverride(dubble);
				}
			}
		}
	}

}
