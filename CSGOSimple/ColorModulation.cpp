#include "ColorModulation.h"

std::vector <MaterialBackup> materials;

void nightmode::clear_stored_materials()
{
	materials.clear();
}

void nightmode::modulate(MaterialHandle_t i, IMaterial* material, bool backup = false)
{
	auto name = material->GetTextureGroupName();
	if (g_EngineClient->IsInGame() && g_LocalPlayer) {
		if (g_Options.misc_nightmode) {
			if (strstr(name, crypt_str("World")))
			{
				if (backup)
					materials.emplace_back(MaterialBackup(i, material));

				Color CuhLuh(50 / 255.0f, 50 / 255.0f, 50 / 255.0f);

				float colorr;
				float colorg;
				float colorb;

				material->GetColorModulation(&colorr, &colorg, &colorb);
				Color CUHLOOR(colorr, colorg, colorb);

				if (CuhLuh != CUHLOOR && g_Options.changemats)
					material->ColorModulate((float)50 / 255.0f, (float)50 / 255.0f, (float)50 / 255.0f);

			}
			else if (strstr(name, crypt_str("StaticProp")))
			{
				if (backup)
					materials.emplace_back(MaterialBackup(i, material));

				Color CuhLuh(120 / 255.0f, 120 / 255.0f, 120 / 255.0f);

				float colorr;
				float colorg;
				float colorb;
				float alpha;
				float cum = 200 / 255.f;
				alpha = material->GetAlphaModulation();
				material->GetColorModulation(&colorr, &colorg, &colorb);
				Color CUHLOOR(colorr, colorg, colorb);

				if (CuhLuh != CUHLOOR && g_Options.changemats)
					material->ColorModulate((float)120 / 255.0f, (float)120 / 255.0f, (float)120 / 255.0f);

				if (g_Options.asusprops) {
					material->AlphaModulate((float)200 / 255.0f);
				}
				else {
					material->AlphaModulate((float)255 / 255.0f);
				}

			}
		}
	}
}

void nightmode::apply()
{
	if (!materials.empty())
	{
		for (auto i = 0; i < (int)materials.size(); i++) //-V202
			modulate(materials[i].handle, materials[i].material);

		return;
	}

	materials.clear();
	auto materialsystem = g_MatSystem;

	for (auto i = materialsystem->FirstMaterial(); i != materialsystem->InvalidMaterial(); i = materialsystem->NextMaterial(i))
	{
		auto material = materialsystem->GetMaterial(i);

		if (!material)
			continue;

		if (!material->IsPrecached())
			continue;

		if (material->IsErrorMaterial())
			continue;

		modulate(i, material, true);
	}
}

void nightmode::remove()
{
	for (auto i = 0; i < materials.size(); i++)
	{
		if (!materials[i].material)
			continue;

		if (materials[i].material->IsErrorMaterial())
			continue;

		materials[i].restore();
		materials[i].material->Refresh();
	}

	materials.clear();
}