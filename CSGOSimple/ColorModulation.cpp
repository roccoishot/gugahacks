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
		if (strstr(name, crypt_str("World")))
		{
			if (backup)
				materials.emplace_back(MaterialBackup(i, material));

			Color CuhLuh(30, 30, 30);

			float colorr;
			float colorg;
			float colorb;

			material->GetColorModulation(&colorr, &colorg, &colorb);
			Color CUHLOOR(colorr, colorg, colorb);

			if (CUHLOOR == CuhLuh && !g_Options.colormodulation) {
				return;
			}
			else if (CUHLOOR != CuhLuh && g_Options.colormodulation) {
				material->ColorModulate((float)30 / 255.0f, (float)30 / 255.0f, (float)30 / 255.0f);
			}

		}
		else if (strstr(name, crypt_str("StaticProp")))
		{
			if (backup)
				materials.emplace_back(MaterialBackup(i, material));

			Color CuhLuh(100, 100, 100);

			float colorr;
			float colorg;
			float colorb;

			material->GetColorModulation(&colorr, &colorg, &colorb);
			Color CUHLOOR(colorr, colorg, colorb);

			if (CUHLOOR == CuhLuh) {
				return;
			}
			else {
				material->ColorModulate((float)100 / 255.0f, (float)100 / 255.0f, (float)100 / 255.0f);
			}

			if (g_Options.asusprops) {
				material->AlphaModulate((float)200 / 255.0f);
			}
			else if (!g_Options.asusprops) {
				material->AlphaModulate((float)255 / 255.0f);
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