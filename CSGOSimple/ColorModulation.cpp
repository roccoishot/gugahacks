#include "ColorModulation.h"
#include "./valve_sdk/csgostructs.hpp"

void CNightmode::PerformNightmode()
{
	for (MaterialHandle_t i = g_MatSystem->FirstMaterial(); i != g_MatSystem->InvalidMaterial(); i = g_MatSystem->NextMaterial(i))
	{
		IMaterial* pMaterial = g_MatSystem->GetMaterial(i);

		if (!pMaterial)
			continue;

		if (!pMaterial->IsPrecached())
			continue;

		const char* group = pMaterial->GetTextureGroupName();
		const char* name = pMaterial->GetName();

		if (g_Options.colormodulate) {
			if (strstr(group, ("World textures")))
			{
				pMaterial->ColorModulate(g_Options.worldr, g_Options.worldg, g_Options.worldb);
			}
			if (strstr(group, ("Particle textures")))
			{
				pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
			}
			if (strstr(name, ("models/props/de_dust/palace_bigdome")))
			{
				pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
			}
			if (strstr(name, ("models/props/de_dust/palace_pillars")))
			{
				pMaterial->ColorModulate(g_Options.worldr, g_Options.worldg, g_Options.worldb);
			}
		}

		if (g_Options.propmodulate) {
			if (strstr(group, ("StaticProp")))
			{
				static auto r_DrawSpecificStaticProp = g_CVar->FindVar(("r_DrawSpecificStaticProp"));
				r_DrawSpecificStaticProp->SetValue(g_Options.propmodulate);
				pMaterial->ColorModulate(g_Options.propr, g_Options.propg, g_Options.propb);
				pMaterial->AlphaModulate(g_Options.propalpha);
			}
		}
	}
}