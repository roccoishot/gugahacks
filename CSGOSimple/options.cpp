#include <ShlObj.h>
#include <filesystem>
#include "options.hpp"
#include "valve_sdk/misc/Enums.hpp"
#include "valve_sdk/sdk.hpp"

//weapon configs
const std::map<int, const char*> config_names = {
	//Pistols
	{WEAPON_CZ75A, "CZ75 Auto"},
	{WEAPON_DEAGLE, "Desert Eagle"},
	{WEAPON_ELITE, "Dual Berettas"},
	{WEAPON_FIVESEVEN, "Five-SeveN"},
	{WEAPON_HKP2000, "P2000"},
	{WEAPON_P250, "P250"},
	{WEAPON_USP_SILENCER, "USP-S"},
	{WEAPON_TEC9, "Tec-9"},
	{WEAPON_REVOLVER, "R8 Revolver"},
	{WEAPON_GLOCK, "Glock-18"},

	//Heavy SH
	{WEAPON_MAG7, "MAG-7"},
	{WEAPON_NOVA, "Nova"},
	{WEAPON_SAWEDOFF, "Sawed-Off"},
	{WEAPON_XM1014, "XM1014"},

	//Smg
	{WEAPON_UMP45, "UMP-45"},
	{WEAPON_P90, "P90"},
	{WEAPON_BIZON, "PP-Bizon"},
	{WEAPON_MAC10, "MAC-10"},
	{WEAPON_MP7, "MP7"},
	{WEAPON_MP9, "MP9"},
	{WEAPON_MP5, "MP5-SD"},
	
	//Heavy RF
	{WEAPON_M249, "M249"},
	{WEAPON_NEGEV, "Negev"},

	//Rifles
	{WEAPON_AK47, "AK-47"},
	{WEAPON_AUG, "AUG"},
	{WEAPON_GALILAR, "Galil AR"},
	{WEAPON_M4A1_SILENCER, "M4A1-S"},
	{WEAPON_M4A1, "M4A4"},
	{WEAPON_SG556, "SG 553"},
	{WEAPON_FAMAS, "FAMAS"},

	//Snipers
	{WEAPON_AWP, "AWP"},
	{WEAPON_G3SG1, "G3SG1"},
	{WEAPON_SCAR20, "SCAR-20"},
	{WEAPON_SSG08, "SSG 08"},

	//Others
	{WEAPON_KNIFE, "Knife"},
	{WEAPON_KNIFE_T, "TKnife"},

	{GLOVE_T_SIDE, "Glove"},
};

const std::map<int, const char*> k_weapon_names = {
	{WEAPON_KNIFE, "knife ct"},
	{WEAPON_KNIFE_T, "knife t"},
	{GLOVE_CT_SIDE, "glove ct"},
	{GLOVE_T_SIDE, "glove t"},
	{61, "USP"},
	{32, "P100"},
	{4, "GLOCK"},
	{2, "DUALIES"},
	{36, "P250"},
	{3, "FIVE SEVEN"},
	{30, "TEC 9"},
	{63, "CZ75"},
	{64, "REVOLVER"},
	{1, "DEAGLE"},

	{34, "MP9"},
	{17, "MAC-10"},
	{23, "MP5"},
	{33, "MP7"},
	{24, "UMP-45"},
	{19, "P90"},
	{26, "PP BIZON"},

	{7, "AK-47"},
	{60, "M4A1-S"},
	{16, "M4A4"},
	{8, "AUG"},
	{39, "SG553"},
	{10, "FAMAS"},
	{13, "GALIL"},

	{40, "SCOUT"},
	{38, "SCAR-20"},
	{9, "AWP"},
	{11, "G3SG1"},

	{14, "M249"},
	{28, "NEGEV"},

	{27, "MAG-7"},
	{35, "NOVA"},
	{29, "SAWED OFF"},
	{25, "XM1014"},
};

void Options::SetupValue(int& value, std::string category, std::string name)
{
	ints.push_back(new ConfigValue<int>(std::move(category), std::move(name), &value));
}

void Options::SetupValue(bool& value, std::string category, std::string name)
{
	bools.push_back(new ConfigValue<bool>(std::move(category), std::move(name), &value));
}

void Options::SetupValue(float& value, std::string category, std::string name)
{
	floats.push_back(new ConfigValue<float>(std::move(category), std::move(name), &value));
}

void Options::SetupColor(Color& value, const std::string& name)
{
	SetupValue(value._CColor[0], "Colors", name + "_r");
	SetupValue(value._CColor[1], "Colors", name + "_g");
	SetupValue(value._CColor[2], "Colors", name + "_b");
	SetupValue(value._CColor[3], "Colors", name + "_a");
}



void Options::SetupWeapons()
{
	for (auto& [val, key] : config_names)
	{
		auto aimbot = &g_Options.weapons[val].legit;

		if (val != WEAPON_KNIFE && val != WEAPON_KNIFE_T && val != GLOVE_T_SIDE)
		{
			SetupValue(aimbot->enabled, key, "Aimbot");
			//SetupValue(aimbot->deathmatch, key, "Deathmatch");
			//SetupValue(aimbot->silent, key, "pSilent");
			SetupValue(aimbot->silent2, key, "Silent");

			SetupValue(aimbot->fov, key, "Fov");
			SetupValue(aimbot->silent_fov, key, "Silent Fov");
			SetupValue(aimbot->smooth, key, "Smooth");

			SetupValue(aimbot->hitboxes.head, key, "Head Hitbox");
			SetupValue(aimbot->hitboxes.chest, key, "Chest Hitbox");
			SetupValue(aimbot->hitboxes.pelvis, key, "Pelvis Hitbox");
			SetupValue(aimbot->hitboxes.arms, key, "Arms Hitbox");
			SetupValue(aimbot->hitboxes.hands, key, "Hands Hitbox");
			SetupValue(aimbot->hitboxes.legs, key, "Legs Hitbox");
			SetupValue(aimbot->hitboxes.feet, key, "Feet Hitbox");

			SetupValue(aimbot->rcs.enabled, key, "RCS Enabled");
			SetupValue(aimbot->rcs.type, key, "RCS Type");
			SetupValue(aimbot->rcs.x, key, "RCS X");
			SetupValue(aimbot->rcs.y, key, "RCS Y");

			SetupValue(aimbot->autowall.enabled, key, "AutoWall Enabled");
			SetupValue(aimbot->autowall.min_damage, key, "AutoWall MinDamage");

			SetupValue(aimbot->autofire.enabled, key, "AutoFire Enabled");
			SetupValue(aimbot->autofire.hotkey, key, "AutoFire Hotkey");
			SetupValue(aimbot->enablehc, key, "Hitchance Enable");
			SetupValue(aimbot->hitchance, key, "Hitchance Value");

		}
	}
	for (auto& [key, val] : k_weapon_names) {
		auto& option = g_Options.changers.skin.m_items[key];
		SetupValue(option.definition_vector_index, val, "d_vec_index");
		SetupValue(option.definition_index, val, "d_index");
		SetupValue(option.paint_kit_vector_index, val, "pk_vec_index");
		SetupValue(option.paint_kit_index, val, "pk_index");
		SetupValue(option.definition_override_index, val, "do_index");
		SetupValue(option.definition_override_vector_index, val, "do_vec_index");
		SetupValue(option.seed, val, "Seed");
		//SetupValue(option.enable_stat_track, val, "stattrack");
		//SetupValue(option.stat_trak, val, "stat_trak_val");
		SetupValue(option.wear, val, "Wear");
	}

}

void Options::SetupVisuals()
{
	SetupValue(flashkillcheck, "Visuals", "Flash Kill Check");
	SetupValue(teamesp, "Visuals", "Team ESP");
	SetupValue(teamchams, "Visuals", "Team Chams");
	SetupValue(esp_player_boxes, "Visuals", "Boxes");
	SetupValue(esp_player_boxesOccluded, "ESP", "Occluded");
	SetupValue(esp_player_names, "Visuals", "Names");
	SetupValue(esp_player_health, "Visuals", "Health");
	SetupValue(esp_player_armour, "Visuals", "Armour");
	SetupValue(esp_player_weapons, "Visuals", "Weapon");
	SetupValue(esp_dropped_weapons, "Visuals", "Dropped Weapons");
	SetupValue(chams_player_enabled, "Visuals", "Chams Vis");
	SetupValue(chams_player_ignorez, "Visuals", "Chams XYZ");
	SetupValue(arms_material, "Visuals", "Arms Material");
	SetupValue(player_material, "Visuals", "Player Material");
	SetupColor(player_enemy_visible_shine, "Color Visible");
	SetupColor(color_esp_enemy_visible, "Enemies Visible");
	SetupColor(color_esp_enemy_occluded, "Enemies Occluded");
	SetupColor(color_chams_player_enemy_visible, "Enemy Visible Chams");
	SetupColor(color_chams_player_enemy_occluded, "Enemy Occluded Chams");
	SetupColor(color_chams_player_ally_visible, "Ally Visible");
	SetupColor(color_chams_player_ally_occluded, "Ally Occluded");
	SetupColor(color_chams_strap_visible, "Strap Visible");
	SetupColor(color_chams_strap_occluded, "Strap XYZ");
	SetupValue(chams_strap_enabled, "Visuals", "Strap Enabled");
	SetupValue(strap_material, "Visuals", "Strap Material");
	SetupValue(chams_strap_ignorez, "Visuals", "Strap XYZ");
	SetupValue(chams_arms_enabled, "Visuals", "Arms Enabled");
	SetupValue(chams_arms_ignorez, "Visuals", "Arms XYZ");
	SetupColor(color_chams_arms_visible, "Arms Visible");
	SetupColor(color_chams_arms_occluded, "Arms Occluded");
	SetupValue(chams_sleeve_enabled, "Visuals", "Sleeves Enabled");
	SetupValue(chams_sleeve_ignorez, "Visuals", "Sleeves XYZ");
	SetupColor(color_chams_sleeve_visible, "Sleeves Visible");
	SetupColor(color_chams_sleeve_occluded, "Sleeves Occluded");
	SetupColor(color_name_player, "Name Color");
	SetupColor(color_armour_player, "Armour Color");
	SetupColor(color_esp_weapons, "Weapons Color");
	SetupValue(misc_thirdperson, "Visuals", "Thirdperson");
	SetupValue(misc_thirdperson_dist, "Visuals", "Thirdperson Distance");
	SetupValue(misc_thirdperson_key, "Visuals", "Thirdperson Key");
	SetupColor(colormodulation, "World Color");
	SetupValue(worldglowb, "Visuals", "World Glow Blue");
	SetupValue(worldglowg, "Visuals", "World Glow Green");
	SetupValue(worldglowr, "Visuals", "World Glow Red");
	SetupColor(proprmodulation, "Prop Color");
	SetupValue(fullbright, "Visuals", "Fullbright");
	SetupValue(amibence, "Visuals", "Model Ambience");
	SetupValue(colormodulate, "Visuals", "World Modulation");
	SetupValue(propmodulate, "Visuals", "Prop Modulation");
	SetupValue(propalpha, "Visuals", "Prop Alpha");
	SetupValue(fovchangaaa, "Visuals", "World Fov");
	SetupValue(aspectchange, "Visuals", "Aspect Ratio");
	SetupValue(aspectratio, "Visuals", "Ratio");
	SetupValue(disablepro, "Visuals", "Disable Processing");
	SetupValue(fatassmf, "Visuals", "Reduce Recoil");
	SetupValue(pnade, "Visuals", "Nade Prediction");
	SetupValue(metallica, "Visuals", "Metallic Mode");
	SetupValue(matrix, "Visuals", "Matrix Mode");
	SetupValue(ragfloat, "Visuals", "Floating Ragdolls");
	SetupValue(shotinfo, "Visuals", "Shot Info");
	SetupValue(rcross, "Visuals", "Recoil Crosshair");
	SetupValue(sky_changer, "Visuals", "Sky Changer");
	SetupValue(skyshitsssss, "Visuals", "Sky");
	SetupValue(enablechanger, "Visuals", "Viewmodel Changer");
	SetupValue(viewmodel_fov, "Visuals", "FOV");
	SetupValue(viewmodel_offset_x, "Visuals", "X");
	SetupValue(viewmodel_offset_y, "Visuals", "Y");
	SetupValue(viewmodel_offset_z, "Visuals", "Z");
	SetupValue(sniper_xhair, "Visuals", "Sniper crosshair");
	SetupValue(drawfov, "Visuals", "Draw FOV");
	
}

void Options::SetupMisc()
{
	SetupValue(autoscope, "Aimbot", "Auto Scope");
	SetupValue(slidewalk, "Test", "Fast Shiftwalk");
	SetupValue(misc_chatspam, "Misc", "Chat Spam");
	SetupValue(faketest, "Test", "180 Desync");
	SetupValue(invertaakey, "Rage", "Invert AA Key");
	SetupValue(rageresolver, "Rage", "Rage Resolver");
	SetupValue(fakepingkey, "Rage", "Fake Ping Key");
	SetupValue(ragebot_antiaim_pitch, "Rage", "AntiAim Pitch");
	SetupValue(ragebot_antiaim_yaw, "Rage", "AntiAim Yaw");
	SetupValue(spinspeed, "Rage", "Spin Speed");
	SetupValue(ragebot_antiaim_desync, "Rage", "Desync");
	SetupValue(ragebot_slowwalk_amt, "Rage", "Slowwalk Speed");
	SetupValue(ragebot_slowwalk_key,"Rage", "Slowwalk Key");
	SetupValue(antiobs, "Misc", "AntiOBS");
	SetupValue(playerModelCT, "Misc", "Player Model CT");
	SetupValue(playerModelT, "Misc", "Player Model T");
	SetupColor(Velocitycol, "Velocity");
	SetupValue(misc_watermark, "Misc", "Watermark");
	SetupValue(Velocity, "Misc", "Velocity");
	SetupValue(outline, "Misc", "Outline");
	SetupValue(lastjump, "Misc", "Last jump");
	SetupValue(lastjumpoutline, "Misc", "Last jump outline");
	SetupValue(autoaccept, "Misc", "Auto accept");
	SetupValue(no_flash, "Misc", "No flash");
	SetupValue(no_smoke, "Misc", "Filter smoke");
	SetupValue(misc_bhop, "Misc", "Bunny hop");
	SetupValue(autostrafe, "Misc", "Auto strafe");
	SetupValue(AutoStafe_key, "Misc", "AutoStafe Key");
	SetupValue(edge_bug, "Misc", "Edge bug");
	SetupValue(edge_bug_key, "Misc", "EdgeBug Key");
	SetupValue(jump_bug, "Misc", "Jump Bug");
	SetupValue(jump_bug_key, "Misc", "Jump Bug Key");
	SetupValue(edgejump.enabled, "Misc", "Edge jump");
	SetupValue(edgejump.hotkey, "Misc", "Edge jump Key");
	SetupValue(ducknair, "Misc", "Duck in Air");
	SetupValue(clantag, "Misc", "Clantag");
	SetupValue(clantagtype, "Misc", "Clantag Type");
	SetupValue(nocool, "Misc", "No Duck Cooldown");
	SetupValue(misc_hitmarker, "Misc", "Hitsounds");
	SetupValue(hitmarkersound, "Misc", "Hitsound");
	SetupValue(breaklby, "Misc", "Break LBY");
	SetupValue(removeblur, "Misc", "Reduce Blur");
	SetupColor(menucolor, "Menu Accents");
	SetupValue(fakelag, "Misc", "Fakelag");
	SetupValue(enablebeta, "Misc", "Beta");
	SetupValue(faketicks, "Misc", "FL Ticks");
	SetupValue(bowlsfreshcut, "Misc", "Flash Indicator");
	SetupValue(misc_backtrack, "Misc", "Backtrack");
	SetupValue(fakeping, "Misc", "Fake Ping");
	SetupValue(fakepingzzz, "Misc", "Fake Ping Tix");
	SetupValue(backtix, "Misc", "Backtrack Ticks");
}

void Options::Initialize()
{
	CHAR my_documents[MAX_PATH];
	SHGetFolderPathA(nullptr, CSIDL_PERSONAL, nullptr, SHGFP_TYPE_CURRENT, my_documents);
	folder = my_documents + std::string("\\GugaHacks\\");
	CreateDirectoryA(folder.c_str(), nullptr);
	SetupWeapons();
	SetupVisuals();
	SetupMisc();
}

void Options::SaveSettings(const std::string& szIniFile)
{
	std::string file = folder + szIniFile;

	CreateDirectoryA(folder.c_str(), nullptr);

	for (auto value : ints)
		WritePrivateProfileStringA(value->category.c_str(), value->name.c_str(), std::to_string(*value->value).c_str(), file.c_str());

	for (auto value : bools)
		WritePrivateProfileStringA(value->category.c_str(), value->name.c_str(), *value->value ? "true" : "false", file.c_str());

	for (auto value : floats)
		WritePrivateProfileStringA(value->category.c_str(), value->name.c_str(), std::to_string(*value->value).c_str(), file.c_str());
}

void Options::LoadSettings(const std::string& szIniFile)
{
	std::string file = folder + szIniFile;

	CreateDirectoryA(folder.c_str(), nullptr);

	if (!std::filesystem::exists(file))
		return;

	char value_l[32] = { '\0' };

	for (auto value : ints)
	{
		GetPrivateProfileStringA(value->category.c_str(), value->name.c_str(), "", value_l, 32, file.c_str());
		*value->value = atoi(value_l);
	}

	for (auto value : bools)
	{
		GetPrivateProfileStringA(value->category.c_str(), value->name.c_str(), "", value_l, 32, file.c_str());
		*value->value = !strcmp(value_l, "true");
	}

	for (auto value : floats)
	{
		GetPrivateProfileStringA(value->category.c_str(), value->name.c_str(), "", value_l, 32, file.c_str());
		*value->value = float(atof(value_l));
	}
	g_ClientState->ForceFullUpdate();
}

void Options::DeleteSettings(const std::string& szIniFile)
{
	std::string file = folder + szIniFile;

	CreateDirectoryA(folder.c_str(), nullptr);

	if (!std::filesystem::exists(file))
		return;

	remove(file.c_str());
}