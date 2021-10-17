#include "ColorModulation.h"
#include "./valve_sdk/csgostructs.hpp"

void CNightmode::UpdateWorldTextures()
{
	if (g_Options.colormodulate) {
		if (!NightmodeDone) {
			PerformNightmode();
			NightmodeDone = true;
		}
		if (NightmodeDone)
			return;
	}
}


void CNightmode::PerformNightmode()
{
	for (MaterialHandle_t i = g_MatSystem->FirstMaterial(); i != g_MatSystem->InvalidMaterial(); i = g_MatSystem->NextMaterial(i))
	{
		IMaterial* pMaterial = g_MatSystem->GetMaterial(i);

		if (!pMaterial)
			continue;

		const char* group = pMaterial->GetTextureGroupName();
		const char* name = pMaterial->GetName();

			if (strstr(name, ("models/props/de_nuke/hr_nuke/nuke_skydome_001")))
			{
				pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
			}

			if (strstr(name, ("models/props/de_inferno/hr_i/inferno_skybox")))
			{
				pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
			}
			if (strstr(name, ("models/props/de_dust/dust_skybox")))
			{
				pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
			}

		if (g_Options.colormodulate) {
			if (strstr(group, ("World")))
			{
				pMaterial->ColorModulate(g_Options.colormodulation.r() * 0.00255, g_Options.colormodulation.g() * 0.00255, g_Options.colormodulation.b() * 0.00255);
			}
			if (strstr(group, ("Particle textures")))
			{
				pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
			}
		}

		if (g_Options.propmodulate && g_Options.colormodulate) {
			if (strstr(group, ("StaticProp")))
			{
				pMaterial->ColorModulate(g_Options.proprmodulation.r() * 0.00255 * 2, g_Options.proprmodulation.g() * 0.00255 * 2, g_Options.proprmodulation.b() * 0.00255 * 2);
				pMaterial->AlphaModulate(g_Options.propalpha);
			}
		}
	}
}