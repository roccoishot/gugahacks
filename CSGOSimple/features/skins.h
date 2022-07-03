#pragma once
#include "item_definitions.h"
#include "../valve_sdk/csgostructs.hpp"
#include "../options.hpp"
#include "../valve_sdk/sdk.hpp"
#include "../imgui/imgui.h"

static const char* weaponnames(const short id)
{
	switch (id)
	{
	case WEAPON_DEAGLE:
		return "Deagle";
	case WEAPON_ELITE:
		return "Dualies";
	case WEAPON_FIVESEVEN:
		return "Fiveseven";
	case WEAPON_GLOCK:
		return "Glock";
	case WEAPON_AK47:
		return "AK47";
	case WEAPON_AUG:
		return "AUG";
	case WEAPON_AWP:
		return "AWP";
	case WEAPON_FAMAS:
		return "Famas";
	case WEAPON_G3SG1:
		return "Auto";
	case WEAPON_GALILAR:
		return "Galil";
	case WEAPON_M249:
		return "M249";
	case WEAPON_M4A1_SILENCER:
		return "M4A1-S";
	case WEAPON_M4A1:
		return "M4A4";
	case WEAPON_MAC10:
		return "Mac-10";
	case WEAPON_P90:
		return "P90";
	case WEAPON_UMP45:
		return "UMP-45";
	case WEAPON_XM1014:
		return "XM1014";
	case WEAPON_BIZON:
		return "Bizon";
	case WEAPON_MAG7:
		return "Mag7";
	case WEAPON_NEGEV:
		return "Negev";
	case WEAPON_SAWEDOFF:
		return "Sawed Off";
	case WEAPON_TEC9:
		return "Tec 9";
	case WEAPON_HKP2000:
		return "P2000";
	case WEAPON_MP5:
		return "MP5-S";
	case WEAPON_MP7:
		return "MP7";
	case WEAPON_MP9:
		return "MP9";
	case WEAPON_NOVA:
		return "Nova";
	case WEAPON_P250:
		return "P250";
	case WEAPON_SCAR20:
		return "Scar";
	case WEAPON_SG556:
		return "SG553";
	case WEAPON_SSG08:
		return "Scout";
	case WEAPON_USP_SILENCER:
		return "USP-S";
	case WEAPON_CZ75A:
		return "CZ-75A";
	case WEAPON_REVOLVER:
		return "Revolver";
	case WEAPON_KNIFE:
		return "CT Knife";
	case WEAPON_KNIFE_T:
		return "T Knife";
	case WEAPON_KNIFE_M9_BAYONET:
		return "M9 Bayonet";
	case WEAPON_KNIFE_BAYONET:
		return "Bayonet";
	case WEAPON_KNIFE_CSS:
		return "Classic Knife";
	case WEAPON_KNIFE_CORD:
		return "Cord Knife";
	case WEAPON_KNIFE_CANIS:
		return "Canis Knife";
	case WEAPON_KNIFE_OUTDOOR:
		return "Outdoor Knife";
	case WEAPON_KNIFE_SKELETON:
		return "Skeleton Knife";
	case WEAPON_KNIFE_FLIP:
		return "Flip Knife";
	case WEAPON_KNIFE_GUT:
		return "Gut Knife";
	case WEAPON_KNIFE_KARAMBIT:
		return "Karambit";
	case WEAPON_KNIFE_TACTICAL:
		return "Tactical Knife";
	case WEAPON_KNIFE_FALCHION:
		return "Falchion Knife";
	case WEAPON_KNIFE_SURVIVAL_BOWIE:
		return "Bowie Knife";
	case WEAPON_KNIFE_BUTTERFLY:
		return "Butterfly Knife";
	case WEAPON_KNIFE_PUSH:
		return "Push Knife";
	case WEAPON_KNIFE_URSUS:
		return "Ursus Knife";
	case WEAPON_KNIFE_GYPSY_JACKKNIFE:
		return "Jackknife";
	case WEAPON_KNIFE_STILETTO:
		return "Stiletto Knife";
	case WEAPON_KNIFE_WIDOWMAKER:
		return "Widowmaker Knife";
	case GLOVE_STUDDED_BLOODHOUND:
		return "Bloodhound Gloves";
	case GLOVE_T_SIDE:
		return "T Gloves";
	case GLOVE_CT_SIDE:
		return "CT Gloves";
	case GLOVE_SPORTY:
		return "Sporty Gloves";
	case GLOVE_SLICK:
		return "Slick Gloves";
	case GLOVE_LEATHER_WRAP:
		return "Leather Wraps";
	case GLOVE_MOTORCYCLE:
		return "Motorcycle Gloves";
	case GLOVE_SPECIALIST:
		return "Specialist Gloves";
	case GLOVE_HYDRA:
		return "Hydra Gloves";
	case STUDDED_BROKENFANG_GLOVES:
		return "Broken Fang Gloves";
	default:
		return "";
	}
}

namespace skins
{
	ImVec4 get_color_ratiry(int rar);
	bool get_skins_cur(int weaponidx, std::string& weap_name);
	const char* get_icon_override(const std::string original);
	void on_frame_stage_notify(bool frame_end);
	int GetNewAnimation(std::string model, const int sequence);

};