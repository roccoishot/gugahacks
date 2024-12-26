#include <ShlObj.h>
#include <filesystem>
#include "options.hpp"
#include "valve_sdk/misc/Enums.hpp"
#include "valve_sdk/sdk.hpp"
#include "xor.h"
#include "Globals.h"
#pragma comment(lib, "Shell32.lib")
#ifdef ENABLE_XOR
#define XorStr _xor_ 
#else
#define XorStr
#endif
#pragma intrinsic(_ReturnAddress)  
//weapon configs
const std::map<int, const char*> config_names = {
	//Pistols
	{WEAPON_CZ75A, XorStr("CZ75 Auto")},
	{WEAPON_DEAGLE, XorStr("Desert Eagle")},
	{WEAPON_ELITE, XorStr("Dual Berettas")},
	{WEAPON_FIVESEVEN, XorStr("Five-SeveN")},
	{WEAPON_HKP2000, XorStr("P2000")},
	{WEAPON_P250, XorStr("P250")},
	{WEAPON_USP_SILENCER, XorStr("USP-S")},
	{WEAPON_TEC9, XorStr("Tec-9")},
	{WEAPON_REVOLVER, XorStr("R8 Revolver")},
	{WEAPON_GLOCK, XorStr("Glock-18")},

	//Heavy SH
	{WEAPON_MAG7, XorStr("MAG-7")},
	{WEAPON_NOVA, XorStr("Nova")},
	{WEAPON_SAWEDOFF, XorStr("Sawed-Off")},
	{WEAPON_XM1014, XorStr("XM1014")},

	//Smg
	{WEAPON_UMP45, XorStr("UMP-45")},
	{WEAPON_P90, XorStr("P90")},
	{WEAPON_BIZON, XorStr("PP-Bizon")},
	{WEAPON_MAC10, XorStr("MAC-10")},
	{WEAPON_MP7, XorStr("MP7")},
	{WEAPON_MP9, XorStr("MP9")},
	{WEAPON_MP5, XorStr("MP5-SD")},

	//Heavy RF
	{WEAPON_M249, XorStr("M249")},
	{WEAPON_NEGEV, XorStr("Negev")},

	//Rifles
	{WEAPON_AK47, XorStr("AK-47")},
	{WEAPON_AUG, XorStr("AUG")},
	{WEAPON_GALILAR, XorStr("Galil AR")},
	{WEAPON_M4A1_SILENCER, XorStr("M4A1-S")},
	{WEAPON_M4A1, XorStr("M4A4")},
	{WEAPON_SG556, XorStr("SG 553")},
	{WEAPON_FAMAS, XorStr("FAMAS")},

	//Snipers
	{WEAPON_AWP, XorStr("AWP")},
	{WEAPON_G3SG1, XorStr("G3SG1")},
	{WEAPON_SCAR20, XorStr("SCAR-20")},
	{WEAPON_SSG08, XorStr("SSG 08")},

	//Others
	{WEAPON_KNIFE, XorStr("Knife")},
	{WEAPON_KNIFE_T, XorStr("TKnife")},

	{GLOVE_T_SIDE, XorStr("Glove")},
};

const std::map<int, const char*> k_weapon_names = {
	{WEAPON_KNIFE, XorStr("knife ct")},
	{WEAPON_KNIFE_T, XorStr("knife t")},
	{GLOVE_CT_SIDE, XorStr("glove ct")},
	{GLOVE_T_SIDE, XorStr("glove t")},
	{61, XorStr("USP")},
	{32, XorStr("P100")},
	{4, XorStr("GLOCK")},
	{2, XorStr("DUALIES")},
	{36, XorStr("P250")},
	{3, XorStr("FIVE SEVEN")},
	{30, XorStr("TEC 9")},
	{63, XorStr("CZ75")},
	{64, XorStr("REVOLVER")},
	{1, XorStr("DEAGLE")},

	{34, XorStr("MP9")},
	{17, XorStr("MAC-10")},
	{23, XorStr("MP5")},
	{33, XorStr("MP7")},
	{24, XorStr("UMP-45")},
	{19, XorStr("P90")},
	{26, XorStr("PP BIZON")},

	{7, XorStr("AK-47")},
	{60, XorStr("M4A1-S")},
	{16, XorStr("M4A4")},
	{8, XorStr("AUG")},
	{39, XorStr("SG553")},
	{10, XorStr("FAMAS")},
	{13, XorStr("GALIL")},

	{40, XorStr("SCOUT")},
	{38, XorStr("SCAR-20")},
	{9, XorStr("AWP")},
	{11, XorStr("G3SG1")},

	{14, XorStr("M249")},
	{28, XorStr("NEGEV")},

	{27, XorStr("MAG-7")},
	{35, XorStr("NOVA")},
	{29, XorStr("SAWED OFF")},
	{25, XorStr("XM1014")},
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

void Options::SetupChar(char& value, std::string category, std::string name) {
	chars.push_back(new ConfigValue<char>(std::move(category), std::move(name), &value));
}

void Options::SetupColor(Color& value, const std::string& name)
{
	SetupValue(value._CColor[0], XorStr("Colors"), name + XorStr("_r"));
	SetupValue(value._CColor[1], XorStr("Colors"), name + XorStr("_g"));
	SetupValue(value._CColor[2], XorStr("Colors"), name + XorStr("_b"));
	SetupValue(value._CColor[3], XorStr("Colors"), name + XorStr("_a"));
}



void Options::SetupWeapons()
{
	for (auto& [val, key] : config_names)
	{
		if (val != WEAPON_KNIFE && val != WEAPON_KNIFE_T && val != GLOVE_T_SIDE && val != GLOVE_CT_SIDE)
		{
			SetupValue(aimbot.enabled, key, XorStr("Aimbot"));
			SetupValue(aimbot.aimkey, key, XorStr("Aim Key"));
			SetupValue(aimbot.silent, key, XorStr("Silent"));

			SetupValue(aimbot.fov, key, XorStr("Fov"));
			SetupValue(aimbot.silentfov, key, XorStr("Silent Fov"));
			SetupValue(aimbot.smoof, key, XorStr("Smooth"));
			SetupValue(aimbot.humanized, key, XorStr("Humanized"));

			SetupValue(aimbot.head, key, XorStr("Head"));
			SetupValue(aimbot.neck, key, XorStr("Neck"));
			SetupValue(aimbot.chest, key, XorStr("Chest"));
			SetupValue(aimbot.stomach, key, XorStr("Stomach"));
			SetupValue(aimbot.pelvis, key, XorStr("Pelvis"));
			SetupValue(aimbot.arms, key, XorStr("Arms"));
			SetupValue(aimbot.hands, key, XorStr("Hands"));
			SetupValue(aimbot.legs, key, XorStr("Legs"));
			SetupValue(aimbot.feet, key, XorStr("Feet"));

			SetupValue(aimbot.rcs, key, XorStr("RCS"));
			SetupValue(aimbot.x, key, XorStr("RCS X"));
			SetupValue(aimbot.y, key, XorStr("RCS Y"));

			SetupValue(aimbot.autowall, key, XorStr("AutoWall Enabled"));
			SetupValue(aimbot.autowallmin, key, XorStr("AutoWall MinDamage"));

			SetupValue(aimbot.mindmg, key, XorStr("Min Dmg"));
			SetupValue(aimbot.dmgovrdk, key, XorStr("Damage Override Key"));
			SetupValue(aimbot.dmgovrd, key, XorStr("Damage Override"));

			SetupValue(aimbot.noaonspec, key, XorStr("No Aimbot On Spec"));
			SetupValue(aimbot.zeusbot, key, XorStr("Zeusbot"));
			SetupValue(aimbot.safehead, key, XorStr("Safe Head"));
			SetupValue(aimbot.dt, key, XorStr("Doubletap"));
			SetupValue(aimbot.dthc, key, XorStr("DT HC"));
			SetupValue(aimbot.dtticks, key, XorStr("DT Ticks"));
			SetupValue(aimbot.dthotkey, key, XorStr("DT Hotkey"));
			SetupValue(aimbot.hs, key, XorStr("Hide Shots"));
			SetupValue(aimbot.hshotkey, key, XorStr("HS Hotkey"));
			SetupValue(aimbot.rageresolver, XorStr("Rage"), XorStr("Resolver"));
			SetupValue(aimbot.printresolver, XorStr("Rage"), XorStr("Print Resolver"));
			SetupValue(aimbot.fastaimbot, key, XorStr("Fast Aimbot"));
			SetupValue(aimbot.autofire, key, XorStr("Autofire"));
			SetupValue(aimbot.afkey, key, XorStr("Autofire Key"));
			SetupValue(aimbot.autostop, key, XorStr("Autostop"));
			SetupValue(aimbot.autorevolver1, key, XorStr("Autorevolver"));
			SetupValue(aimbot.hc, key, XorStr("Hitchance"));
			SetupValue(aimbot.hitchance, key, XorStr("Hitchance %"));
			SetupValue(aimbot.backtrack, key, XorStr("Backtrack"));
			SetupValue(aimbot.aimatbt, key, XorStr("Aim at Backtrack"));
			SetupValue(aimbot.rageaim, key, XorStr("Rage Aim"));

		}
	}

	for (auto& [key, val] : k_weapon_names) {
		auto& option = changers.skin.m_items[key];
		SetupValue(option.enabled, val, XorStr("Enabled"));
		SetupValue(option.definition_vector_index, val, XorStr("d_vec_index"));
		SetupValue(option.definition_index, val, XorStr("d_index"));
		SetupValue(option.paint_kit_vector_index, val, XorStr("pk_vec_index"));
		SetupValue(option.paint_kit_index, val, XorStr("pk_index"));
		SetupValue(option.definition_override_index, val, XorStr("do_index"));
		SetupValue(option.definition_override_vector_index, val, XorStr("do_vec_index"));
		SetupValue(option.seed, val, XorStr("Seed"));
		SetupValue(option.stat_trak, val, XorStr("stat_trak_val"));
		SetupValue(option.wear, val, XorStr("Wear"));
	}

}

void Options::SetupVisuals()
{
	SetupColor(color_backtrackind, XorStr("Backtrack Color"));
	SetupValue(showbt, XorStr("Visuals"), XorStr("Show BT"));
	SetupValue(playerind, XorStr("Visuals"), XorStr("Player Indicator"));
	SetupColor(color_indicator, XorStr("Indicator Color"));
	SetupValue(boxtype, XorStr("Visuals"), XorStr("Box Style"));
	SetupValue(snapthicc, XorStr("Visuals"), XorStr("Snapline Thickness"));
	SetupValue(snaplines, XorStr("Visuals"), XorStr("Snaplines"));
	SetupColor(color_cumming, XorStr("Snapline Color"));
	SetupValue(skelethicc, XorStr("Visuals"), XorStr("Skeleton Thickness"));
	SetupValue(skeletonesp, XorStr("Visuals"), XorStr("Skeleton ESP"));
	SetupColor(color_skemletom, XorStr("Skeletons Color"));
	SetupValue(fovchangaaa, XorStr("Visuals"), XorStr("World FOV"));
	SetupValue(thirdinspec, XorStr("Visuals"), XorStr("Thirdperson Spectate"));
	SetupValue(dormantesp, XorStr("Visuals"), XorStr("Dormant ESP"));
	SetupValue(faresp, XorStr("Visuals"), XorStr("Far ESP"));
	SetupValue(noscope, XorStr("Visuals"), XorStr("Remove Scope"));
	SetupValue(fovscope, XorStr("Visuals"), XorStr("FOV In Scope"));
	SetupValue(flashkillcheck, XorStr("Visuals"), XorStr("Flash Kill Check"));
	SetupValue(teamchams, XorStr("Visuals"), XorStr("Team Chams"));
	SetupValue(localchams, XorStr("Visuals"), XorStr("Local Chams"));
	SetupValue(esp_player_boxes, XorStr("Visuals"), XorStr("Boxes"));
	SetupValue(esp_player_boxesOccluded, XorStr("Visuals"), XorStr("Occluded"));
	SetupValue(esp_player_names, XorStr("Visuals"), XorStr("Names"));
	SetupValue(esp_player_health, XorStr("Visuals"), XorStr("Health"));
	SetupValue(esp_player_armour, XorStr("Visuals"), XorStr("Armour"));
	SetupValue(esp_player_weapons, XorStr("Visuals"), XorStr("Weapon"));
	SetupValue(esp_dropped_weapons, XorStr("Visuals"), XorStr("Dropped Weapons"));
	SetupValue(drawogplaya, XorStr("Visuals"), XorStr("Draw OG Playa"));
	SetupValue(chams_fake_enabled, XorStr("Visuals"), XorStr("Fake Chams"));
	SetupValue(chams_player_enabled, XorStr("Visuals"), XorStr("Chams Vis"));
	SetupValue(chams_player_ignorez, XorStr("Visuals"), XorStr("Chams XQZ"));
	SetupValue(arms_material, XorStr("Visuals"), XorStr("Arms Material"));
	SetupColor(glowcolor, XorStr("Double Color Team"));
	SetupColor(glowcolorenemy, XorStr("Double Color Enemy"));
	SetupColor(glowcolorlocal, XorStr("Double Color Local"));
	SetupColor(glowcolorarms, XorStr("Double Color Arms"));
	SetupColor(glowcolorstrap, XorStr("Double Color Strap"));
	SetupValue(player_material, XorStr("Visuals"), XorStr("Player Material"));
	SetupColor(color_esp_enemy_visible, XorStr("Enemies Visible"));
	SetupColor(color_esp_enemy_occluded, XorStr("Enemies Occluded"));
	SetupColor(color_chams_fake, XorStr("Fake Chams Color"));
	SetupColor(color_chams_player_enemy_visible, XorStr("Enemy Visible Chams"));
	SetupColor(color_chams_player_enemy_occluded, XorStr("Enemy Occluded Chams"));
	SetupColor(color_chams_player_local_visible, XorStr("Local Visible Chams"));
	SetupColor(color_chams_player_local_occluded, XorStr("Local Occluded Chams"));
	SetupColor(color_chams_player_ally_visible, XorStr("Ally Visible"));
	SetupColor(color_chams_player_ally_occluded, XorStr("Ally Occluded"));
	SetupColor(color_chams_strap_visible, XorStr("Strap Visible"));
	SetupValue(chams_strap_enabled, XorStr("Visuals"), XorStr("Strap Enabled"));
	SetupValue(strap_material, XorStr("Visuals"), XorStr("Strap Material"));
	SetupValue(chams_arms_enabled, XorStr("Visuals"), XorStr("Arms Enabled"));
	SetupColor(color_chams_arms_visible, XorStr("Arms Visible"));
	SetupColor(color_chams_sleeve_visible, XorStr("Sleeves Visible"));
	SetupColor(color_name_player, XorStr("Name Color"));
	SetupColor(color_armour_player, XorStr("Armour Color"));
	SetupColor(color_esp_weapons, XorStr("Weapons Color"));
	SetupValue(misc_thirdperson, XorStr("Visuals"), XorStr("Thirdperson"));
	SetupValue(misc_thirdperson_key, XorStr("Visuals"), XorStr("Thirdperson Key"));
	SetupColor(color_worldglow, XorStr("World Glow"));
	SetupValue(fullbright, XorStr("Visuals"), XorStr("Fullbright"));
	SetupValue(amibence, XorStr("Visuals"), XorStr("Model Ambience"));
	SetupValue(colormodulate, XorStr("Visuals"), XorStr("Color Modulation"));
	SetupValue(propstoo, XorStr("Visuals"), XorStr("Prop Modulation"));
	SetupColor(nightmodecolor, XorStr("World Color"));
	SetupColor(propcolor, XorStr("Prop Color"));
	SetupValue(aspectchange, XorStr("Visuals"), XorStr("Aspect Ratio"));
	SetupValue(aspectratio, XorStr("Visuals"), XorStr("Ratio"));
	SetupValue(disablepro, XorStr("Visuals"), XorStr("Disable Processing"));
	SetupValue(fatassmf, XorStr("Visuals"), XorStr("Reduce Recoil"));
	SetupValue(pnade, XorStr("Visuals"), XorStr("Nade Prediction"));
	SetupValue(metallica, XorStr("Visuals"), XorStr("Metallic Mode"));
	SetupValue(matrix, XorStr("Visuals"), XorStr("Matrix Mode"));
	SetupValue(ragfloat, XorStr("Visuals"), XorStr("Floating Ragdolls"));
	SetupValue(rcross, XorStr("Visuals"), XorStr("Recoil Crosshair"));
	SetupValue(sky_changer, XorStr("Visuals"), XorStr("Sky Changer"));
	SetupValue(skyshitsssss, XorStr("Visuals"), XorStr("Sky"));
	SetupValue(viewmodel_fov, XorStr("Visuals"), XorStr("Viewmodel FOV"));
	SetupValue(viewmodel_offset_x, XorStr("Visuals"), XorStr("X"));
	SetupValue(viewmodel_offset_y, XorStr("Visuals"), XorStr("Y"));
	SetupValue(viewmodel_offset_z, XorStr("Visuals"), XorStr("Z"));
	SetupValue(viewmodel_offset_roll, XorStr("Visuals"), XorStr("Roll"));
	SetupValue(viewmodelinscope, XorStr("Visuals"), XorStr("Viewmodel In Scope"));
	SetupValue(sniper_xhair, XorStr("Visuals"), XorStr("Sniper crosshair"));
	SetupValue(drawfov, XorStr("Visuals"), XorStr("Draw FOV"));
	SetupValue(fogdens, XorStr("World"), XorStr("Fog Density"));
	SetupValue(fogfardamn, XorStr("World"), XorStr("Fog Distance"));
	SetupValue(fogoverride, XorStr("World"), XorStr("Override Fog"));
	SetupColor(fogcoluh, XorStr("Fog Color"));

}

void Options::SetupMisc()
{
	SetupValue(spinroll2speed, XorStr("Rage"), XorStr("Spin Fake Roll Speed"));
	SetupValue(spinrollspeed, XorStr("Rage"), XorStr("Spin Roll Speed"));
	SetupValue(spinroll2, XorStr("Rage"), XorStr("Spin Fake Roll"));
	SetupValue(spinroll, XorStr("Rage"), XorStr("Spin Roll"));
	SetupValue(roll, XorStr("Rage"), XorStr("Fake Roll"));
	SetupValue(realroll, XorStr("Rage"), XorStr("Real Roll"));
	SetupValue(drugs, XorStr("Misc"), XorStr("Preserve Killfeed"));
	SetupValue(longjump, XorStr("Misc"), XorStr("Longjump"));
	SetupValue(ljkey, XorStr("Misc"), XorStr("Longjump Key"));
	SetupValue(slidebruh, XorStr("Rage"), XorStr("Slidewalk"));
	SetupValue(jitterdegree, XorStr("Rage"), XorStr("Jitter Degree"));
	SetupValue(customreal, XorStr("Rage"), XorStr("Custom Real Degree"));
	SetupValue(enabletest, XorStr("Misc"), XorStr("Test Features"));
	SetupValue(specx, XorStr("Misc"), XorStr("Spectator List X"));
	SetupValue(specy, XorStr("Misc"), XorStr("Spectator List Y"));
	SetupValue(fakespinsp, XorStr("Rage"), XorStr("Fake Spin Speed"));
	SetupValue(slowwalk, XorStr("Rage"), XorStr("Slowwalk"));
	SetupValue(rankreveal, XorStr("Misc"), XorStr("Rank Reveal"));
	SetupValue(indicatorhb, XorStr("Rage"), XorStr("Desync Indicator Hitbox"));
	SetupValue(fakelag, XorStr("Rage"), XorStr("Fakelag"));
	SetupValue(faketicks, XorStr("Rage"), XorStr("Fakelag Ticks"));
	SetupValue(flonpeek, XorStr("Rage"), XorStr("Fakelag On Peek"));
	SetupValue(flop, XorStr("Rage"), XorStr("Fakelag On Peek Ticks"));
	SetupValue(fakeduck, XorStr("Rage"), XorStr("Fakeduck"));
	SetupValue(fdkey, XorStr("Rage"), XorStr("Fakeduck Key"));
	SetupValue(fakedegree, XorStr("Rage"), XorStr("Custom Fake °"));
	SetupValue(fakepreset, XorStr("Rage"), XorStr("Fake Preset"));
	SetupValue(nopacketonshot, XorStr("Sexdick"), XorStr("No Packet on Shot"));
	SetupValue(urrstuck, XorStr("Sexdick"), XorStr("Air Stuck"));
	SetupValue(urrstuckkey, XorStr("Sexdick"), XorStr("Air Stuck Key"));
	SetupValue(randomizefake, XorStr("Sexdick"), XorStr("Randomize Fake"));
	SetupValue(headthing, XorStr("Sexdick"), XorStr("No Pitch on Land"));
	SetupValue(invertaa, XorStr("Antiaim"), XorStr("Invert AA Key"));
	SetupValue(speclist, XorStr("Misc"), XorStr("Spectator List"));
	SetupValue(blockbot, XorStr("Misc"), XorStr("Blockbot"));
	SetupValue(bbkey, XorStr("Misc"), XorStr("BlockBot Key"));
	SetupValue(quickstop, XorStr("Test"), XorStr("Quick Stop"));
	SetupValue(misc_chatspam, XorStr("Misc"), XorStr("Chat Spam"));
	SetupValue(chatspamtype, XorStr("Misc"), XorStr("Chat Spam Type"));
	SetupValue(fakepingkey, XorStr("Rage"), XorStr("Fake Ping Key"));
	SetupValue(ragebot_antiaim_pitch, XorStr("Rage"), XorStr("AntiAim Pitch"));
	SetupValue(ragebot_antiaim_yaw, XorStr("Rage"), XorStr("AntiAim Yaw"));
	SetupValue(spinspeed, XorStr("Rage"), XorStr("Spin Speed"));
	SetupValue(ragebot_antiaim_desync, XorStr("Rage"), XorStr("Desync"));
	SetupValue(ragebot_slowwalk_amt, XorStr("Rage"), XorStr("Slowwalk Speed"));
	SetupValue(ragebot_slowwalk_key, XorStr("Rage"), XorStr("Slowwalk Key"));
	SetupValue(antiobs, XorStr("Misc"), XorStr("AntiOBS"));
	SetupValue(playerModelCT, XorStr("Misc"), XorStr("Player Model CT"));
	SetupValue(playerModelT, XorStr("Misc"), XorStr("Player Model T"));
	SetupColor(Velocitycol, XorStr("Velocity"));
	SetupValue(misc_watermark, XorStr("Misc"), XorStr("Watermark"));
	SetupValue(velgraphline, XorStr("Misc"), XorStr("Velocity Graph Lines"));
	SetupValue(Velocity, XorStr("Misc"), XorStr("Velocity"));
	SetupValue(outline, XorStr("Misc"), XorStr("Outline"));
	SetupValue(lastjump, XorStr("Misc"), XorStr("Last jump"));
	SetupValue(lastjumpoutline, XorStr("Misc"), XorStr("Last jump outline"));
	SetupValue(autoaccept, XorStr("Misc"), XorStr("Auto accept"));
	SetupValue(no_flash, XorStr("Misc"), XorStr("No flash"));
	SetupValue(no_smoke, XorStr("Misc"), XorStr("No smoke"));
	SetupValue(misc_bhop, XorStr("Misc"), XorStr("Bunny hop"));
	SetupValue(autostrafemode, XorStr("Misc"), XorStr("Autostrafe Mode"));
	SetupValue(autostrafe, XorStr("Misc"), XorStr("Auto strafe"));
	SetupValue(AutoStafe_key, XorStr("Misc"), XorStr("AutoStafe Key"));
	SetupValue(edge_bug, XorStr("Misc"), XorStr("Edge bug"));
	SetupValue(edge_bug_key, XorStr("Misc"), XorStr("EdgeBug Key"));
	SetupValue(jump_bug, XorStr("Misc"), XorStr("Jump Bug"));
	SetupValue(jump_bug_key, XorStr("Misc"), XorStr("Jump Bug Key"));
	SetupValue(edgejump.enabled, XorStr("Misc"), XorStr("Edge jump"));
	SetupValue(edgejump.hotkey, XorStr("Misc"), XorStr("Edge jump Key"));
	SetupValue(forceangles, XorStr("Recorder"), XorStr("Force Angles"));
	SetupValue(movementplay, XorStr("Recorder"), XorStr("Play Movement"));
	SetupValue(movementrecord, XorStr("Recorder"), XorStr("Record Movement"));
	SetupValue(ducknair, XorStr("Misc"), XorStr("Duck in Air"));
	SetupValue(diarkey, XorStr("Misc"), XorStr("Duck in Air Key"));
	SetupValue(clantag, XorStr("Misc"), XorStr("Clantag"));
	SetupValue(clantagtype, XorStr("Misc"), XorStr("Clantag Type"));
	SetupValue(nocool, XorStr("Misc"), XorStr("No Duck Cooldown"));
	SetupValue(misc_hitmarker, XorStr("Misc"), XorStr("Hitsounds"));
	SetupValue(hitmarkersound, XorStr("Misc"), XorStr("Hitsound"));
	SetupValue(breaklby, XorStr("Rage"), XorStr("Body Breaker"));
	SetupValue(breakertp, XorStr("Rage"), XorStr("Body Breaker Type"));
	SetupValue(removeblur, XorStr("World"), XorStr("Remove Blur"));
	SetupColor(menucolor, XorStr("Menu Accents"));
	SetupValue(fakeping, XorStr("Rage"), XorStr("Fake Ping"));
	SetupValue(fakepingzzz, XorStr("Rage"), XorStr("Fake Ping Tix"));
}

void Options::Initialize()
{
	CHAR my_documents[MAX_PATH];
	SHGetFolderPathA(nullptr, CSIDL_PERSONAL, nullptr, SHGFP_TYPE_CURRENT, my_documents);
	folder = my_documents + std::string(XorStr("\\GugaHacks\\"));
	CreateDirectoryA(folder.c_str(), nullptr);
	Globals::cfgfldr = folder;
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
		WritePrivateProfileStringA(value->category.c_str(), value->name.c_str(), *value->value ? XorStr("true") : XorStr("false"), file.c_str());

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
		*value->value = !strcmp(value_l, XorStr("true"));
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