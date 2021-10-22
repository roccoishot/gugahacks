#include "ColorModulation.h"
#include "./valve_sdk/csgostructs.hpp"

void CNightmode::PerformNightmode()
{
	static auto r_DrawSpecificStaticProp = g_CVar->FindVar(("r_DrawSpecificStaticProp"));
	r_DrawSpecificStaticProp->SetValue(g_Options.colormodulate);

	for (MaterialHandle_t i = g_MatSystem->FirstMaterial(); i != g_MatSystem->InvalidMaterial(); i = g_MatSystem->NextMaterial(i))
	{
		IMaterial* pMaterial = g_MatSystem->GetMaterial(i);

		if (!pMaterial)
			continue;

		if (!pMaterial->IsPrecached())
			continue;

		const char* group = pMaterial->GetTextureGroupName();
		const char* name = pMaterial->GetName();

		float world_textures = g_Options.colormodulate ? 0.30f : 1.f;
		float staticprop = g_Options.colormodulate ? 0.30f : 1.f;
		float palace_pillars = g_Options.colormodulate ? 0.30f : 1.f;
		float propalefuh = g_Options.asusprops ? 0.60f : 1.f;

		if (g_Options.colormodulate) {
			if (strstr(group, ("World")))
			{
				pMaterial->ColorModulate(world_textures, world_textures, world_textures);
			}
			if (strstr(group, ("StaticProp")))
			{
				pMaterial->ColorModulate(staticprop, staticprop, staticprop);
				pMaterial->AlphaModulate(propalefuh);
			}

			if (strstr(name, ("models/props/de_dust/palace_bigdome")))
			{
				pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, g_Options.colormodulate);
			}
			if (strstr(name, ("models/props/de_dust/palace_pillars")))
			{
				pMaterial->ColorModulate(palace_pillars, palace_pillars, palace_pillars);
			}

			if (strstr(group, ("Particle textures")))
			{
				pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, g_Options.colormodulate);
			}

		}
		}
	}