#include "ColorMod.h"
#include "xor.h"
#include "hooks.hpp"
#ifdef ENABLE_XOR
#define XorStr _xor_ 
#else
#define XorStr
#endif
#pragma intrinsic(_ReturnAddress)  

IMaterial* CreateMaterial2(bool lit, const std::string& material_data)
{
	static auto created = 0;
	std::string type = lit ? XorStr("VertexLitGeneric") : XorStr("UnlitGeneric");

	auto matname = XorStr("legendware_") + std::to_string(created);
	++created;

	auto keyValues = new KeyValues(matname.c_str());
	static auto key_values_address = Utils::PatternScan2(XorStr("client.dll"), XorStr("55 8B EC 51 33 C0 C7 45"));

	using KeyValuesFn = void(__thiscall*)(void*, const char*);
	reinterpret_cast <KeyValuesFn> (key_values_address)(keyValues, type.c_str());

	static auto load_from_buffer_address = Utils::PatternScan2(XorStr("client.dll"), XorStr("55 8B EC 83 E4 F8 83 EC 34 53 8B 5D 0C 89"));
	using LoadFromBufferFn = void(__thiscall*)(void*, const char*, const char*, void*, const char*, void*, void*);

	reinterpret_cast <LoadFromBufferFn> (load_from_buffer_address)(keyValues, matname.c_str(), material_data.c_str(), nullptr, nullptr, nullptr, nullptr);

	static auto material = g_MatSystem->CreateMaterial(matname.c_str(), keyValues);
	material->IncrementReferenceCount();

	return material;
}

void modulate2(const Color color, IMaterial* material)
{
	if (!g_EngineClient->IsInGame())
		return;

	if (material)
	{
		material->ColorModulate(color[0] / 255.f, color[1] / 255.f, color[2] / 255.f);
		material->AlphaModulate(color[3] / 255.f);
		const auto tint = material->find_var(XorStr("$envmaptint"));

		if (tint)
		{
			tint->set_vector(Vector(color.r(), color.g(), color.b()));
		}
	}

	g_RenderView->SetColorModulation(color[0] / 255.f, color[1] / 255.f, color[2] / 255.f);
	g_RenderView->SetBlend(color[3] / 255.f);
}

//CHams on prop[s negro
void CNightmode::OnDME(void* pResults, DrawModelInfo_t* pInfo, matrix3x4_t* pBoneToWorld, float* flpFlexWeights, float* flpFlexDelayedWeights, Vector& vrModelOrigin, int32_t iFlags)
{
	static auto fnDME = Hooks::stdrender_hook.get_original<decltype(&Hooks::hkDrawModelExecute2)>(index::DrawModelExecute2);

	if (!pInfo->m_pClientEntity || !g_LocalPlayer)
		return;

	const auto mdl = pInfo->m_pClientEntity->GetModel();
	//run yo pockets cuh
	bool is_player = (strstr(mdl->szName, XorStr("models/player")) != nullptr && g_Options.chams_player_enabled);
	auto is_weapon = ((strstr(mdl->szName, XorStr("weapons/v_")) || strstr(mdl->szName, XorStr("weapons/w_"))) && !strstr(mdl->szName, XorStr("arms")) && g_Options.chams_strap_enabled);
	auto is_arm = (strstr(mdl->szName, XorStr("arms")) && g_Options.chams_arms_enabled);
	auto is_sleeve = (strstr(mdl->szName, XorStr("sleeve")) && g_Options.chams_arms_enabled);

	static IMaterial* shit = g_MatSystem->FindMaterial(XorStr("dev/glow_armsrace"), nullptr);

	for (MaterialHandle_t i = g_MatSystem->FirstMaterial(); i != g_MatSystem->InvalidMaterial(); i = g_MatSystem->NextMaterial(i))
	{
		IMaterial* pMaterial = g_MatSystem->GetMaterial(i);

		if (!pMaterial)
			continue;

		const char* group = pMaterial->GetTextureGroupName();
		const char* name = pMaterial->GetName();

		if (strstr(group, XorStr("StaticProp")) && !is_player && !is_weapon && !is_arm && !is_sleeve)
		{
			fnDME(g_StudioRender, 0, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);
			modulate2(Color(45, 0, 0, 255), shit);
			g_MdlRender->ForcedMaterialOverride(shit);
			fnDME(g_StudioRender, 0, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags);
		}
	}

}

void CNightmode::PerformNightmode()
{
	int cumballs = g_Options.propstoo ? 0 : -1;
	static auto r_DrawSpecificStaticProp = g_CVar->FindVar(XorStr("r_DrawSpecificStaticProp"));
	if (r_DrawSpecificStaticProp->GetInt() != cumballs)
	r_DrawSpecificStaticProp->SetValue(cumballs);

	for (MaterialHandle_t i = g_MatSystem->FirstMaterial(); i != g_MatSystem->InvalidMaterial(); i = g_MatSystem->NextMaterial(i))
	{
		IMaterial* pMaterial = g_MatSystem->GetMaterial(i);

		if (!pMaterial)
			continue;

		const char* group = pMaterial->GetTextureGroupName();
		const char* name = pMaterial->GetName();

		if (g_Options.colormodulate) {

			float worldr, worldg, worldb;
			float propr, propg, propb;
			float pillarsr, pillarsg, pillarsb;

			if (strstr(group, XorStr("World textures")))
			{
				pMaterial->GetColorModulation(&worldr, &worldg, &worldb);
			}
			if (strstr(group, XorStr("StaticProp")))
			{
				pMaterial->GetColorModulation(&propr, &propg, &propb);
			}
			if (strstr(name, XorStr("models/props/de_dust/palace_pillars")))
			{
				pMaterial->GetColorModulation(&pillarsr, &pillarsg, &pillarsb);
			}

			if (worldr != g_Options.nightmodecolor.r() * 0.00392156862 || worldr != g_Options.nightmodecolor.g() * 0.00392156862 || worldr != g_Options.nightmodecolor.b() * 0.00392156862)
			{
				if (strstr(group, XorStr("World textures")))
				{
					pMaterial->ColorModulate(g_Options.nightmodecolor.r() * 0.00392156862, g_Options.nightmodecolor.g() * 0.00392156862, g_Options.nightmodecolor.b() * 0.00392156862);
				}
			}

			if (propr != g_Options.propcolor.r() * 0.00392156862 || propg != g_Options.propcolor.g() * 0.00392156862 || propb != g_Options.propcolor.b() * 0.00392156862)
			{
				if (strstr(group, XorStr("StaticProp")))
				{
					pMaterial->ColorModulate(g_Options.propcolor.r() * 0.00392156862, g_Options.propcolor.g() * 0.00392156862, g_Options.propcolor.b() * 0.00392156862);
					pMaterial->AlphaModulate(g_Options.propcolor.a() * 0.00392156862);
				}
			}

			if (pillarsr != g_Options.nightmodecolor.r() * 0.00392156862 || pillarsg != g_Options.nightmodecolor.g() * 0.00392156862 || pillarsb != g_Options.nightmodecolor.b() * 0.00392156862)
			{
				if (strstr(name, XorStr("models/props/de_dust/palace_pillars")))
				{
					pMaterial->ColorModulate(g_Options.nightmodecolor.r() * 0.00392156862, g_Options.nightmodecolor.g() * 0.00392156862, g_Options.nightmodecolor.b() * 0.00392156862);
				}
			}

			wason = true;
		}
	}
}

void CNightmode::GtfoNight()
{
	static auto r_DrawSpecificStaticProp = g_CVar->FindVar(XorStr("r_DrawSpecificStaticProp"));
	if (r_DrawSpecificStaticProp->GetInt() != -1)
	r_DrawSpecificStaticProp->SetValue(XorStr("-1"));

	for (MaterialHandle_t i = g_MatSystem->FirstMaterial(); i != g_MatSystem->InvalidMaterial(); i = g_MatSystem->NextMaterial(i))
	{
		IMaterial* pMaterial = g_MatSystem->GetMaterial(i);

		if (!pMaterial)
			continue;

		const char* group = pMaterial->GetTextureGroupName();
		const char* name = pMaterial->GetName();

		if (g_EngineClient->IsInGame())
		{
			if (!g_Options.colormodulate && wason) {
				if (strstr(group, XorStr("World textures")))
				{
					pMaterial->ColorModulate(1.0, 1.0, 1.0);
				}
				if (strstr(name, XorStr("models/props/de_dust/palace_pillars")))
				{
					pMaterial->ColorModulate(1.0, 1.0, 1.0);
				}
			}
		}
		else
		{
			if (!g_Options.colormodulate && wason) {
				wason = false;
			}
		}
	}
}