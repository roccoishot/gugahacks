#include "FixSkyboxes.h"
#include "./valve_sdk/csgostructs.hpp"

void Fixed::PerformNightmode()
{
	for (MaterialHandle_t i = g_MatSystem->FirstMaterial(); i != g_MatSystem->InvalidMaterial(); i = g_MatSystem->NextMaterial(i))
	{
		IMaterial* pMaterial = g_MatSystem->GetMaterial(i);

		if (!pMaterial)
			continue;

		const char* group = pMaterial->GetTextureGroupName();
		const char* name = pMaterial->GetName();

		if (strstr(pMaterial->GetName(), ("models/props/de_nuke/hr_nuke/nuke_skydome_001")))
		{
			pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
		}
		if (strstr(pMaterial->GetName(), ("models/props/de_inferno/hr_i/inferno_skybox")))
		{
			pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
		}
		if (strstr(pMaterial->GetName(), ("models/props/de_dust/dust_skybox")))
		{
			pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
		}
	}
}