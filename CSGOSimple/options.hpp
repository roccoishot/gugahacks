#pragma once

#include <Windows.h>
#include <string>
#include <memory>
#include <map>
#include <unordered_map>
#include <vector>
#include <set>
#include "valve_sdk/Misc/Color.hpp"

#define A( s ) #s
#define OPTION(type, var, val) Var<type> var = {A(var), val}

template <typename T>
class ConfigValue
{
public:
	ConfigValue(std::string category_, std::string name_, T* value_)
	{
		category = category_;
		name = name_;
		value = value_;
	}

	std::string category, name;
	T* value;
};

template <typename T = bool>
class Var {
public:
	std::string name;
	std::shared_ptr<T> value;
	int32_t size;
	Var(std::string name, T v) : name(name) {
		value = std::make_shared<T>(v);
		size = sizeof(T);
	}
	operator T() { return *value; }
	operator T* () { return &*value; }
	operator T() const { return *value; }
	//operator T*() const { return value; }
};

struct statrack_setting
{
	int definition_index = 1;
	struct
	{
		int counter = 0;
	}statrack_new;
};
struct item_setting
{
	char name[32] = "Default";
	//bool enabled = false;
	int stickers_place = 0;
	int definition_vector_index = 0;
	int definition_index = 0;
	bool   enabled_stickers = 0;
	int paint_kit_vector_index = 0;
	int paint_kit_index = 0;
	int definition_override_vector_index = 0;
	int definition_override_index = 0;
	int seed = 0;
	bool stat_trak = 0;
	float wear = FLT_MIN;
	char custom_name[32] = "";
};
class Options
{
public:
	struct
	{

		struct
		{
			bool skin_preview = false;
			bool show_cur = true;

			std::map<int, statrack_setting> statrack_items = { };
			std::map<int, item_setting> m_items = { };
			std::map<std::string, std::string> m_icon_overrides = { };
		}skin;
	}changers;
		// 
		// ESP
		// 
	//	bool esp_enabled = false;
	
	struct
	{
		bool enabled = false;
		float fov = 0;
		float smoof = 0;
		int hitboxes = 0;
		bool head = true;
		bool chest = true;
		bool pelvis = true;
		bool arms = true;
		bool feet = true;
		bool hands = true;
		bool legs = true;
		bool hc = false;
		int rcsstart = 1;
		int hitchance = 0;
		bool silent = false;
		float silentfov = 0;
		bool autofire = false;
		bool autowall = false;
		int autowallmin = 0;
		bool rcs = false;
		int rcstype = 0;
		int x = 100;
		int y = 100;
		bool autorevolver1 = false;
		bool autorevolver2 = false;
		bool fastaimbot = false;
	} aimbot;

	bool ragebot_enabled = false;
	bool ragebot_backtrack = false;
	int ragebot_fov = 0;
	bool ragebot_autozeus = false;

	bool ragebot_hitbox[8][9] = { false, false, false, false, false, false, false, false };
	float ragebot_hitbox_multipoint_scale[8][9] = { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
	int ragebot_selection[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	bool ragebot_autoscope[9] = { false, false, false, false, false, false, false, false };
	bool ragebot_autostop[9] = { false, false, false, false, false, false, false, false };
	bool ragebot_autocrouch[9] = { false, false, false, false, false, false, false, false };
	float ragebot_mindamage[9] = { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
	float ragebot_hitchance[9] = { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
	int ragebot_baim_after_shots[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	int invertaa = 0;
	bool speclist = false;
	bool faresp = false;
	bool noscope = false;
	bool fovscope = false;
	bool ebdetection = false;
	int ebmode = 0;
	bool blockbot = false;
	int bbkey = 0;
	bool fogchanga = false;
	int fogfardamn = 0;
	int fogdens = 0;
	bool flashkillcheck = false;
	float amibence = 0;
	bool slidewalk = false;
	bool rageresolver = true;
	int fakepingkey = 0;
	bool localchams = false;
	bool teamchams = false;
	bool asusprops = false;
		bool enemychams = false;
		bool colormodulate = false;
		bool disablepro = false;
		bool esp_player_boxes = false;
		bool esp_player_boxesOccluded = false;
		bool esp_player_names = false;
		bool esp_player_health = false;
		bool esp_player_armour = false;
		bool esp_player_weapons = false;
		bool esp_player_snaplines = false;
		bool esp_dropped_weapons = false;
		bool esp_defuse_kit = false;
		bool esp_planted_c4 = false;
		bool esp_items = false;
		bool aspectchange = false;
		float aspectratio = 0;
		float worldglowr = 0;
		float worldglowg = 0;
		float worldglowb = 0;
		//
		// CHAMS
		//
		int arms_material = 0;
		int strap_material = 0;
		int player_material = 0;

		bool chams_player_enabled = false;
		bool chams_player_enemies_only = false;
		bool chams_player_ignorez = false;
		bool chams_arms_enabled = false;
		bool chams_arms_ignorez = false;

		bool chams_strap_enabled = false;
		bool chams_strap_ignorez = false;

		//
		// MISC
		//
		bool colormodulation = false;
		bool changemats = false;
		bool teamesp = false;
		bool ducknair = false;
		bool fullbright = false;
		bool misc_chatspam = false;
		bool nocool = false;
		int ragebot_antiaim_pitch = 0;
		int ragebot_antiaim_yaw = 0;
		bool ragebot_antiaim_desync = false;
		int ragebot_slowwalk_amt = 0;
		int ragebot_slowwalk_key = 0;

		int skybox_scale = 100;
		int nightmode_value_power = 50;
		bool antiobs = false;
		bool drawfov = false;
		int backtix = 0;
		int clantagtype = 0;
		int hitmarkersound = 0;
		bool misc_backtrack = false;
		bool misc_hitmarker = false;
		bool breaklby = false;
		bool shotinfo = false;
		bool rcross = false;
		int misc_thirdperson_key = 0;
		bool misc_thirdperson = false;
		float misc_thirdperson_dist = 50.f;
		bool pnade = false;
		bool fatassmf = false;
		int spinspeed = 1;
		bool sky_changer = false;
		int skyshitsssss = 0;
		float misc_nightmode = 1.0f;
		bool removeblur = false;
		bool misc_bhop = false;
		bool misc_bhop2 = false;
		int playerModelT{0 };
		int playerModelCT{ 0 };
		int viewmodel_fov = 68;
		int viewmodel_offset_x = 2.5;
		int viewmodel_offset_y = 2;
		int viewmodel_offset_z = -2;
		int fovchangaaa = 0;
		bool metallica = false;
		bool ragfloat = false;
		bool matrix = false;
		bool bowlsfreshcut = false;
		bool enablechanger = false;
		bool misc_watermark = true;
		bool clantag = false;
		bool skychange = false;
		bool namec = false;
		bool Velocity = false;
		bool outline = false;
		bool lastjump = false;
		bool lastjumpoutline = false;
		bool autoaccept = false;
		bool no_flash = false;
		bool no_smoke = false;
		int AutoStafe_key;
		bool autostrafe = false;
		int faketicks = 0;
		bool fakelag = false;
		bool fakeping = false;
		int fakepingzzz = 0;
		bool enablebeta = false;
		struct
		{
			bool enabled = false;

			int hotkey = 0;
		} edgejump;
		bool edge_bug;
		int edge_bug_key;
		bool jump_bug = false;
		int jump_bug_key;
		int glow_enemies_type;
		bool sniper_xhair = false;

		// 
		// COLORS
		// 
		Color color_esp_ally_visible= {0, 0, 255, 255};
		Color color_esp_enemy_visible= { 152,255,0,255 };
		Color color_esp_ally_occluded= {165, 165, 255, 255 };
		Color color_esp_enemy_occluded= {52,255,0,110 };
		Color color_esp_weapons= {255, 255, 255, 255 };
		Color color_esp_defuse= {100, 100, 255, 255 };
		Color color_esp_c4= {255, 0, 0};
		Color color_esp_item= {0, 0, 0};
		Color Velocitycol = { 0, 0, 0 , 255 };

		Color color_glow_ally= {255, 255, 255};
		Color color_glow_allyOC= {255, 255, 255};

		Color color_glow_enemy= {255, 255, 255};
		Color color_glow_enemyOC= {255, 255, 255};
		Color color_dropped_weapons = { 255, 224, 48 , 255};

		Color color_glow_chickens= {0, 0, 0};
		Color color_glow_c4_carrier= {0, 0, 0};
		Color color_glow_planted_c4= {0, 0, 0};
		Color color_glow_defuse= {0, 0, 0};
		Color color_glow_weapons= {0, 0, 0};

		Color color_armour_player = { 0, 0, 255, 255 };
		Color color_name_player = { 255, 255, 255, 255 };
		Color color_chams_player_ally_visible= { 0, 0, 255, 255 };
		Color color_chams_player_ally_occluded= { 165, 165, 255 };

		Color color_chams_player_local_visible = { 255, 130, 0, 255 };
		Color color_chams_player_local_occluded = { 155, 30, 0, 170 };

		Color color_chams_player_enemy_visible= { 152,255,0,255 };
		Color color_chams_player_enemy_occluded= { 52,255,0,110 };
		Color color_chams_strap_visible = { 255, 255, 255, 255 };
		Color color_chams_strap_occluded = { 255, 255, 255, 100 };	
		Color color_chams_arms_visible= { 152,255,0,255 };
		Color color_chams_arms_occluded= { 52,255,0,110 };
		Color player_enemy_visible_shine = { 180, 180, 180, 255 };
		Color ohthuhshine = { 255, 255, 255, 255 };
		Color glowcolor = { 255, 255, 255 };
		Color glowcolorenemy = { 255, 255, 255 };
		Color glowcolorlocal = { 255, 255, 255 };
		Color glowcolorarms = { 255, 255, 255 };
		Color glowcolorstrap = { 255, 255, 255 };
		Color fogcoluh = { 255, 255, 255, 255 };
		Color menucolor = { 255,255,255,255 };

protected:
	//std::vector<ConfigValue<char>*> chars;
	std::vector<ConfigValue<int>*> ints;
	std::vector<ConfigValue<bool>*> bools;
	std::vector<ConfigValue<float>*> floats;
private:
//	void SetupValue(char value, std::string category, std::string name);
	void SetupValue(int& value, std::string category, std::string name);
	void SetupValue(bool& value, std::string category, std::string name);
	void SetupValue(float& value, std::string category, std::string name);
	void SetupColor(Color& value, const std::string& name);
	void SetupWeapons();
	void SetupVisuals();
	void SetupMisc();
	void SetupColors();
public:
	void Initialize();
	void LoadSettings(const std::string& szIniFile);
	void SaveSettings(const std::string& szIniFile);
	void DeleteSettings(const std::string& szIniFile);

	std::string folder;
};

inline Options g_Options;
inline bool   g_Unload;