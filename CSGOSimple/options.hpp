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

struct legitbot_s
{
	bool enabled = false;
	bool deathmatch = false;
	bool silent = false;
	bool silent2 = false;
	bool hsilent = false;

	bool flash_check = false;
	bool smoke_check = false;
	bool backtracking = false;

	float fov = 0.f;
	float silent_fov = 0.f;
	float smooth = 1.f;

	int shot_delay = 0;
	int kill_delay = 0;



	struct
	{
		

		bool head = true;
		bool chest = true;
		bool pelvis = true;
		bool arms = true;
		bool feet = true;
		bool hands = true;
		bool legs = true;
	} hitboxes;

	struct
	{
		bool enabled = false;
		int start = 1;
		int type = 0;
		int x = 100;
		int y = 100;
	} rcs;

	struct
	{
		bool enabled = false;
		int min_damage = 1;
	} autowall;

	struct
	{
		bool enabled = false;
		int min_damage = 1;
	} visible;

	struct
	{
		bool enabled = false;
		int ticks = 6;
	} backtrack;

	struct
	{
		bool enabled = false;
		int hotkey = 0;
	} autofire;
	
};


struct aimbot_settings {
	int autofire_key = 0;
	bool enabled = false;
	bool autofire = false;
	bool on_key = true;
	int key = 0;
	bool check_smoke = false;
	bool check_flash = false;
	bool check_jump = false;
	bool autowall = false;
	bool silent = false;
	bool antiaimlock = false;
	bool rcs = false;
	bool rcs_fov_enabled = false;
	bool rcs_smooth_enabled = false;
	bool humanize = false;
	float curviness = false;
	struct {
		bool enabled = false;
		int ticks = 6;
	} backtrack;
	bool only_in_zoom = false;
	int aim_type = 1;
	int smooth_type;
	int priority = 0;
	int fov_type = 0;
	int rcs_type = 0;
	int hitbox = 1;
	float fov = 0.f;
	float silent_fov = 0.f;
	float rcs_fov = 0.f;
	float smooth = 1;
	float rcs_smooth = 1;
	int shot_delay = 0;
	int kill_delay = 0;
	int rcs_x = 100;
	int rcs_y = 100;
	int rcs_start = 1;
	int min_damage = 1;
};

struct weapons
{
	legitbot_s legit;
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
	std::map<int, aimbot_settings> aimbot = {};
	std::map<short, weapons> weapons;
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
	float modelambience = 0;
	bool rageresolver = false;
	int fakepingkey = 0;
	int vangleyaw = 0;
	int vanglepitch = 0;
	bool teamesp = false;
	bool teamchams = false;
		bool esp_enemies_only = false;
		bool colormodulate = false;
		bool propmodulate = false;
		int menur = 255.f;
		int menug = 255.f;
		int menub = 255.f;
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
		float worldr = 0.7f;
		float worldg = 0.7f;
		float worldb = 0.7f;
		float propr = 0.7f;
		float propg = 0.7f;
		float propb = 0.7f;
		float propalpha = 1.f;
		float worldglowr = 0;
		float worldglowg = 0;
		float worldglowb = 0;
		bool propfix = false;

		// 
		// GLOW
		// 
		bool glow_enabled = false;
		bool glow_enemies_only = false;
		bool glow_players = false;
		bool glow_chickens = false;
		bool glow_c4_carrier = false;
		bool glow_planted_c4 = false;
		bool glow_defuse_kits = false;
		bool glow_weapons = false;
		bool glow_enemiesOC = false;

		//
		// CHAMS
		//
		bool chams_player_enabled = false;
		bool player_enemies_shine = false;
		bool player_enemies_glow = false;

		bool chams_player_enemies_only = false;
		bool chams_player_wireframe = false;
		bool chams_player_scary = false;
		bool chams_player_flat = false;
		bool chams_player_ignorez = false;
		bool chams_player_glass = false;
		bool chams_arms_enabled = false;
		bool chams_arms_wireframe = false;
		bool chams_arms_flat = false;
		bool chams_arms_ignorez = false;
		bool chams_arms_glass = false;

		bool chams_sleeve_enabled = false;
		bool chams_sleeve_ignorez = false;

		bool chams_strap_enabled = false;
		bool chams_strap_wireframe = false;
		bool chams_strap_flat = false;
		bool chams_strap_ignorez = false;
		bool chams_strap_glass = false;

		//
		// MISC
		//
		bool ducknair = false;
		bool fullbright = false;
		bool misc_chatspam = false;
		bool faketest = false;
		bool nocool = false;
		int ragebot_antiaim_pitch = 0;
		int ragebot_antiaim_yaw = 0;
		bool ragebot_antiaim_desync = false;
		int ragebot_slowwalk_amt = 0;
		int ragebot_slowwalk_key = 0;

		bool antiobs = false;
		bool yawaafoward = false;
		bool yawaaback = false;
		bool pitchaaup = false;
		bool pitchaadown = false;
		bool pitchaanone = false;
		bool drawfov = false;
		int backtix = 0;
		bool hitmetallic = false;
		bool hitchicken = false;
		bool hitdog = false;
		bool hitspark = false;
		bool hitfranz = false;
		bool hitfail = false;
		bool hitbeep = false;
		bool hithurt = false;
		bool hitsnap = false;
		bool hitdisagree = false;
		bool hitagree = false;
		bool hitfoundgame = false;

			bool night2 = false;
			bool baggage = false;
			bool tibet = false;
			bool vietnam = false;
			bool sky_lunacy = false;
			bool embassy = false;
			bool italy = false;
			bool jungle = false;
			bool office = false;
			bool daylight = false;
			bool cloudy1 = false;
			bool skydust = false;
			bool pitchshit = false;
		bool misc_backtrack = false;
		bool misc_hitmarker = false;
		bool breaklby = false;
		const char* skyname;
		bool shotinfo = false;
		bool rcross = false;
		int misc_thirdperson_key = 0;
		bool misc_thirdperson = false;
		float misc_thirdperson_dist = 50.f;
		bool pnade = false;
		bool fatassmf = false;
		bool carlosfatcock = true;
		int spinspeed = 1;
		bool shine_oc = false;
		bool sky_changer = false;
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
		bool thebypasscuh = false;
		bool bowlsfreshcut = false;
		bool enablechanger = false;
		bool misc_showranks = false;
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
		bool spectator_list = false;
		int AutoStafe_key;
		bool autostrafe = false;
		int desync_hotkey = 0;
		int faketicks = 0;
		bool fakelag = false;
		bool fakeping = false;
		int fakepingzzz = 0;
		bool menurescale = false;
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
		Color color_esp_ally_visible= {165, 165, 255};
		Color color_esp_enemy_visible= { 96, 255, 43 };
		Color color_esp_ally_occluded= {0, 0, 0};
		Color color_esp_enemy_occluded= { 24, 104, 173 };
		Color color_esp_weapons= {255, 255, 255};
		Color color_esp_defuse= {0, 0, 0};
		Color color_esp_c4= {0, 0, 0};
		Color color_esp_item= {0, 0, 0};
		Color Velocitycol = { 0, 0, 0 };

		Color color_glow_ally= {0, 0, 0};
		Color color_glow_allyOC= {0, 0, 0};

		Color color_glow_enemy= {255, 0, 0};
		Color color_glow_enemyOC= {0, 0, 0};
		Color color_dropped_weapons = { 255, 224, 48 };

		Color color_glow_chickens= {0, 0, 0};
		Color color_glow_c4_carrier= {0, 0, 0};
		Color color_glow_planted_c4= {0, 0, 0};
		Color color_glow_defuse= {0, 0, 0};
		Color color_glow_weapons= {0, 0, 0};

		Color color_armour_player = { 0, 0, 255 };
		Color color_name_player = { 255, 255, 255 };
		Color color_chams_player_ally_visible= {0, 161, 255, 255};
		Color color_chams_player_ally_occluded= {52, 106, 193, 255};
		Color color_chams_player_enemy_visible= { 96, 255, 43 };
		Color color_chams_player_enemy_occluded= { 24, 104, 173 };
		Color color_chams_strap_visible = { 255, 255, 255 };
		Color color_chams_strap_occluded = { 255, 255, 255 };	
		Color color_chams_arms_visible= {0, 0, 0};
		Color color_chams_arms_occluded= {0, 0, 0};
		Color color_chams_sleeve_visible = { 0, 0, 0 };
		Color color_chams_sleeve_occluded = { 0, 0, 0 };
		Color color_watermark= {0, 0, 0}; // no menu config cuz its useless
		Color player_enemy_visible_shine = { 255, 255, 255 };

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