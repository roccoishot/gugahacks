#include "FixSkyboxes.h"
#include "./valve_sdk/csgostructs.hpp"
#include "xor.h"

void Fixed::PerformNightmode()
{
	static auto r_3dsky = g_CVar->FindVar(XorStr("r_3dsky"));
	r_3dsky->SetValue(XorStr("0"));

	for (MaterialHandle_t i = g_MatSystem->FirstMaterial(); i != g_MatSystem->InvalidMaterial(); i = g_MatSystem->NextMaterial(i))
	{
		IMaterial* pMaterial = g_MatSystem->GetMaterial(i);

		if (!pMaterial)
			continue;

		const char* group = pMaterial->GetTextureGroupName();
		const char* name = pMaterial->GetName();

		if (strstr(name, ("models/props/de_nuke/hr_nuke/nuke_skydome_001")))
		{
			pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, g_Options.skychange);
		}
		if (strstr(name, ("models/props/de_inferno/hr_i/inferno_skybox")))
		{
			pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, g_Options.skychange);
		}
		if (strstr(name, ("models/props/de_dust/hr_dust/dust_skybox")))
		{
			pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, g_Options.skychange);
		}

		if (strstr(name, ("models/props/de_dust/palace_bigdome")))
		{
			pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, g_Options.skychange);
		}

		if (strstr(name, ("models/props/de_vertigo/De_Vertigo_Skybox01")))
		{
			pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, g_Options.skychange);
		}
	}
}