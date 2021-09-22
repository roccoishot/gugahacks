#include "ColorModulation.h"
#include "./valve_sdk/csgostructs.hpp"

void CNightmode::PerformNightmode()
{

	if (!g_Options.colormodulate)
		return;

	for (MaterialHandle_t i = g_MatSystem->FirstMaterial(); i != g_MatSystem->InvalidMaterial(); i = g_MatSystem->NextMaterial(i))
	{
		IMaterial* pMaterial = g_MatSystem->GetMaterial(i);

		if (!pMaterial)
			continue;

		if (!pMaterial->IsPrecached())
			continue;

		const char* group = pMaterial->GetTextureGroupName();
		const char* name = pMaterial->GetName();

		if (g_Options.sky_changer) {
			if (strstr(name, ("models/props/de_nuke/hr_nuke/nuke_skydome_001")))
			{
				pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
			}

			if (strstr(name, ("models/props/de_inferno/hr_i/inferno_skybox")))
			{
				pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
			}
		}

		if (g_Options.colormodulate) {
			if (strstr(group, ("World textures")))
			{
				pMaterial->ColorModulate(g_Options.colormodulation.r() * 0.00255, g_Options.colormodulation.g() * 0.00255, g_Options.colormodulation.b() * 0.00255);
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
				pMaterial->ColorModulate(g_Options.colormodulation.r() * 0.00255, g_Options.colormodulation.g() * 0.00255, g_Options.colormodulation.b() * 0.00255);
			}
		}

		if (g_Options.propmodulate) {
			if (strstr(group, ("StaticProp")))
			{
				static auto r_DrawSpecificStaticProp = g_CVar->FindVar(("r_DrawSpecificStaticProp"));
				r_DrawSpecificStaticProp->SetValue(g_Options.propmodulate);
				pMaterial->ColorModulate(g_Options.proprmodulation.r() * 0.00255 * 2, g_Options.proprmodulation.g() * 0.00255 * 2, g_Options.proprmodulation.b() * 0.00255 * 2);
				pMaterial->AlphaModulate(g_Options.propalpha);
			}
		}
	}
}