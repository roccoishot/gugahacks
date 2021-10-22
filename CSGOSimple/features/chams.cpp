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

Chams::Chams()
{
	std::ofstream("csgo\\materials\\glowOverlay.vmt") << R"#("VertexLitGeneric"
{
	"$additive" "1"
	"$envmap" "models/effects/cube_white"
	"$envmaptint" "[1 1 1]"
	"$envmapfresnel" "1"
	"$envmapfresnelminmaxexp" "[0 1 2]"
	"$alpha" "1.0"
}
)#";

	std::ofstream("csgo\\materials\\animatedfuck.vmt") << R"#("VertexLitGeneric"
		    {
		        "$basetexture"				"dev/zone_warning"
		        "$additive"					"1"
		        "$envmap"					"editor/cube_vertigo"
		        "$envmaptint"				"[0 0.5 0.55]"
		        "$envmapfresnel"			"1"
		        "$envmapfresnelminmaxexp"   "[0.00005 0.6 6]"
		        "$alpha"					"1"
   
		        Proxies
		        {
		            TextureScroll
		            {
		                "texturescrollvar"			"$baseTextureTransform"
		                "texturescrollrate"			"0.25"
		                "texturescrollangle"		"270"
		            }
		            Sine
		            {
		                "sineperiod"				"2"
		                "sinemin"					"0.1"
		                "resultVar"					"$envmapfresnelminmaxexp[1]"
		            }
		        }
		    }
		)#";

	std::ofstream("csgo\\materials\\animatedshit.vmt") << R"#("VertexLitGeneric"
{
	"$basetexture" "models/weapons/energy_sword/blade_mideffect_mask"
	"$texture2" "models/weapons/energy_sword/blade_mideffect"
 	"$model" 1
	"$nocull" "1"
	"$additive" "1"
	"$color2" "[0.8 1 4]"
	
		
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
		)#";

}


Chams::~Chams()
{
	std::remove("csgo\\materials\\glowOverlay.vmt");
	std::remove("csgo\\materials\\animatedfuck.vmt");
	std::remove("csgo\\materials\\animatedshit.vmt");
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

	class KeyValues {
	public:
		static KeyValues* fromString(const char* name, const char* value) noexcept;
		KeyValues* findKey(const char* keyName, bool create) noexcept;
		void setString(const char* keyName, const char* value) noexcept;
	};

	//const auto kv = KeyValues::fromString("VertexLitGeneric", "$envmap models/effects/cube_white $envmapfresnel 1 $additive 1 $envmapfresnelminmaxexp [0 1 2] $alpha 1.0");
	//kv->setString("$envmaptint", "[1 1 1]");

	//materials\models\inventory_items\dogtags\dogtags_lightray.vmt
	// save for l8r //static IMaterial* pulse = g_MatSystem->FindMaterial("models/inventory_items/dogtags/dogtags_outline", TEXTURE_GROUP_OTHER);
	static IMaterial* shine = g_MatSystem->FindMaterial("models/inventory_items/cologne_prediction/cologne_prediction_glass", nullptr);
	static IMaterial* othershine = g_MatSystem->FindMaterial("models/inventory_items/trophy_majors/gloss", nullptr);
	static IMaterial* velvet = g_MatSystem->FindMaterial("models/inventory_items/trophy_majors/velvet", TEXTURE_GROUP_OTHER);
	static IMaterial* shit = g_MatSystem->FindMaterial("glowOverlay", nullptr);
	static IMaterial* dubble = g_MatSystem->FindMaterial("dev/glow_armsrace", TEXTURE_GROUP_MODEL);
	static IMaterial* animated = g_MatSystem->FindMaterial("animatedfuck", nullptr);
	static IMaterial* animated2 = g_MatSystem->FindMaterial("animatedshit", nullptr); //this is the advancedaim lightning shit
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
					fnDME(g_StudioRender, 0, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);
					if (g_Options.player_material == 2) {
						shine->ColorModulate(g_Options.player_enemy_visible_shine[0] / 255.f, g_Options.player_enemy_visible_shine[1] / 255.f, g_Options.player_enemy_visible_shine[2] / 255.f);
						shine->AlphaModulate(g_Options.player_enemy_visible_shine[3] / 255.f);
						g_StudioRender->ForcedMaterialOverride(shine);
					}
					if (g_Options.player_material == 3) {
						velvet->ColorModulate(g_Options.color_chams_player_ally_visible[0] / 255.f, g_Options.color_chams_player_ally_visible[1] / 255.f, g_Options.color_chams_player_ally_visible[2] / 255.f);
						velvet->AlphaModulate(g_Options.color_chams_player_ally_visible[3] / 255.f);
						g_StudioRender->ForcedMaterialOverride(velvet);
					}
					if (g_Options.player_material == 4) {
						animated->ColorModulate(g_Options.glowcolor[0] / 255.f, g_Options.glowcolor[1] / 255.f, g_Options.glowcolor[2] / 255.f);
						animated->AlphaModulate(g_Options.glowcolor[3] / 255.f);
						g_StudioRender->ForcedMaterialOverride(animated);
					}
					if (g_Options.player_material == 5) {
						shit->ColorModulate(g_Options.glowcolor[0] / 255.f, g_Options.glowcolor[1] / 255.f, g_Options.glowcolor[2] / 255.f);
						shit->AlphaModulate(g_Options.glowcolor[3] / 255.f);
						g_StudioRender->ForcedMaterialOverride(shit);
					}
					fnDME(g_StudioRender, 0, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);

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
								animated->AlphaModulate(g_Options.glowcolor[3] / 255.f);
								animated->ColorModulate(g_Options.glowcolor[0] / 255.f, g_Options.glowcolor[1] / 255.f, g_Options.glowcolor[2] / 255.f);
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
								dubble->AlphaModulate(g_Options.glowcolor[3] / 255.f);
								bool bFound = false;
								auto pVar = dubble->FindVar("$envmaptint", &bFound);
								if (bFound)
								{
									(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.glowcolor[0] / 255.f, g_Options.glowcolor[1] / 255.f, g_Options.glowcolor[2] / 255.f);
								}
								g_MdlRender->ForcedMaterialOverride(dubble);
							}

						}
					}
				}

				const auto enemy = ent->IsEnemy();
				if (enemy)
				{

					fnDME(g_StudioRender, 0, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);
					if (g_Options.player_material == 2) {
						shine->ColorModulate(g_Options.player_enemy_visible_shine[0] / 255.f, g_Options.player_enemy_visible_shine[1] / 255.f, g_Options.player_enemy_visible_shine[2] / 255.f);
						shine->AlphaModulate(g_Options.player_enemy_visible_shine[3] / 255.f);
						g_StudioRender->ForcedMaterialOverride(shine);
					}
					if (g_Options.player_material == 3) {
						velvet->ColorModulate(g_Options.color_chams_player_enemy_visible[0] / 255.f, g_Options.color_chams_player_enemy_visible[1] / 255.f, g_Options.color_chams_player_enemy_visible[2] / 255.f);
						velvet->AlphaModulate(g_Options.color_chams_player_enemy_visible[3] / 255.f);
						g_StudioRender->ForcedMaterialOverride(velvet);
					}
					if (g_Options.player_material == 4) {
						animated->ColorModulate(g_Options.glowcolorenemy[0] / 255.f, g_Options.glowcolorenemy[1] / 255.f, g_Options.glowcolorenemy[2] / 255.f);
						animated->AlphaModulate(g_Options.glowcolorenemy[3] / 255.f);
						g_StudioRender->ForcedMaterialOverride(animated);
					}
					if (g_Options.player_material == 5) {
						shit->ColorModulate(g_Options.glowcolorenemy[0] / 255.f, g_Options.glowcolorenemy[1] / 255.f, g_Options.glowcolorenemy[2] / 255.f);
						shit->AlphaModulate(g_Options.glowcolorenemy[3] / 255.f);
						g_StudioRender->ForcedMaterialOverride(shit);
					}
					fnDME(g_StudioRender, 0, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);

					if (g_Options.chams_player_enabled && g_Options.chams_player_ignorez)
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
									animated->AlphaModulate(g_Options.glowcolorenemy[3] / 255.f);
									animated->ColorModulate(g_Options.glowcolorenemy[0] / 255.f, g_Options.glowcolorenemy[1] / 255.f, g_Options.glowcolorenemy[2] / 255.f);
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
									dubble->AlphaModulate(g_Options.glowcolorenemy[3] / 255.f);
									bool bFound = false;
									auto pVar = dubble->FindVar("$envmaptint", &bFound);
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
	}

	if (is_arm || is_sleeve) {

		if (is_player)
			return;

		fnDME(g_StudioRender, 0, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);
		if (g_Options.arms_material == 2) {
			othershine->ColorModulate(g_Options.ohthuhshine[0] / 255.f, g_Options.ohthuhshine[1] / 255.f, g_Options.ohthuhshine[2] / 255.f);
			othershine->AlphaModulate(g_Options.ohthuhshine[3] / 255.f);
			g_StudioRender->ForcedMaterialOverride(othershine);
		}
		if (g_Options.arms_material == 3) {
			animated->ColorModulate(g_Options.glowcolorarms[0] / 255.f, g_Options.glowcolorarms[1] / 255.f, g_Options.glowcolorarms[2] / 255.f);
			animated->AlphaModulate(g_Options.glowcolorarms[3] / 255.f);
			g_StudioRender->ForcedMaterialOverride(animated);
		}
		if (g_Options.arms_material == 4) {
			shit->ColorModulate(g_Options.glowcolorarms[0] / 255.f, g_Options.glowcolorarms[1] / 255.f, g_Options.glowcolorarms[2] / 255.f);
			shit->AlphaModulate(g_Options.glowcolorarms[3] / 255.f);
			g_StudioRender->ForcedMaterialOverride(shit);
		}
		fnDME(g_StudioRender, 0, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);

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
					othershine->AlphaModulate(g_Options.ohthuhshine[3] / 255.f);
					bool bFound = false;
					auto pVar = shine->FindVar("$envmaptint", &bFound);
					if (bFound)
					{
						(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.ohthuhshine[0] / 255.f, g_Options.ohthuhshine[1] / 255.f, g_Options.ohthuhshine[2] / 255.f);
					}
					g_MdlRender->ForcedMaterialOverride(othershine);
				}

				if (g_Options.arms_material == 3)
				{
					animated->AlphaModulate(g_Options.glowcolorarms[3] / 255.f);
					animated->ColorModulate(g_Options.glowcolorarms[0] / 255.f, g_Options.glowcolorarms[1] / 255.f, g_Options.glowcolorarms[2] / 255.f);
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
					dubble->AlphaModulate(g_Options.glowcolorarms[3] / 255.f);
					bool bFound = false;
					auto pVar = dubble->FindVar("$envmaptint", &bFound);
					if (bFound)
					{
						(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.glowcolorarms[0] / 255.f, g_Options.glowcolorarms[1] / 255.f, g_Options.glowcolorarms[2] / 255.f);
					}
					g_MdlRender->ForcedMaterialOverride(dubble);
				}

			}
		}
	}

	if (is_weapon) {

		if (is_arm || is_sleeve)
			return;

		fnDME(g_StudioRender, 0, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);
		if (g_Options.strap_material == 2) {
			othershine->ColorModulate(g_Options.player_enemy_visible_shine[0] / 255.f, g_Options.player_enemy_visible_shine[1] / 255.f, g_Options.player_enemy_visible_shine[2] / 255.f);
			othershine->AlphaModulate(g_Options.player_enemy_visible_shine[3] / 255.f);
			g_StudioRender->ForcedMaterialOverride(othershine);
		}
		if (g_Options.strap_material == 3) {
			animated->ColorModulate(g_Options.glowcolorstrap[0] / 255.f, g_Options.glowcolorstrap[1] / 255.f, g_Options.glowcolorstrap[2] / 255.f);
			animated->AlphaModulate(g_Options.glowcolorstrap[3] / 255.f);
			g_StudioRender->ForcedMaterialOverride(animated);
		}
		if (g_Options.strap_material == 4) {
			shit->ColorModulate(g_Options.glowcolorstrap[0] / 255.f, g_Options.glowcolorstrap[1] / 255.f, g_Options.glowcolorstrap[2] / 255.f);
			shit->AlphaModulate(g_Options.glowcolorstrap[3] / 255.f);
			g_StudioRender->ForcedMaterialOverride(shit);
		}
		if (g_Options.strap_material == 6) {
			animated2->ColorModulate(g_Options.glowcolorstrap[0] / 255.f, g_Options.glowcolorstrap[1] / 255.f, g_Options.glowcolorstrap[2] / 255.f);
			animated2->AlphaModulate(g_Options.glowcolorstrap[3] / 255.f);
			g_StudioRender->ForcedMaterialOverride(animated2);
		}
		fnDME(g_StudioRender, 0, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);

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
					othershine->AlphaModulate(g_Options.player_enemy_visible_shine[3] / 255.f);
					bool bFound = false;
					auto pVar = shine->FindVar("$envmaptint", &bFound);
					if (bFound)
					{
						(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.player_enemy_visible_shine[0] / 255.f, g_Options.player_enemy_visible_shine[1] / 255.f, g_Options.player_enemy_visible_shine[2] / 255.f);
					}
					g_MdlRender->ForcedMaterialOverride(othershine);
				}

				if (g_Options.strap_material == 3)
				{
					animated->AlphaModulate(g_Options.glowcolorstrap[3] / 255.f);
					animated->ColorModulate(g_Options.glowcolorstrap[0] / 255.f, g_Options.glowcolorstrap[1] / 255.f, g_Options.glowcolorstrap[2] / 255.f);
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
					dubble->AlphaModulate(g_Options.glowcolorstrap[3] / 255.f);
					bool bFound = false;
					auto pVar = dubble->FindVar("$envmaptint", &bFound);
					if (bFound)
					{
						(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.glowcolorstrap[0] / 255.f, g_Options.glowcolorstrap[1] / 255.f, g_Options.glowcolorstrap[2] / 255.f);
					}
					g_MdlRender->ForcedMaterialOverride(dubble);
				}

				if (g_Options.strap_material == 6) {
					animated2->ColorModulate(g_Options.glowcolorstrap[0] / 255.f, g_Options.glowcolorstrap[1] / 255.f, g_Options.glowcolorstrap[2] / 255.f);
					animated2->AlphaModulate(g_Options.glowcolorstrap[3] / 255.f);
					g_StudioRender->ForcedMaterialOverride(animated2);
				}

			}
		}
	}

}