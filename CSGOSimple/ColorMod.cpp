#include "ColorMod.h"
#include "xor.h"
#include "hooks.hpp"
#ifdef ENABLE_XOR
#define XorStr _xor_ 
#else
#define XorStr
#endif
#pragma intrinsic(_ReturnAddress)  

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