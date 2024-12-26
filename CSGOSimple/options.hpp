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
	bool enabled = false;
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
			auto get_icon_override(const std::string original) const -> const char*
			{
				return m_icon_overrides.count(original) ? m_icon_overrides.at(original).data() : nullptr;
			}
		}skin;
	}changers;
	// 
	// ESP
	// 
//	bool esp_enabled = false;

	struct
	{
		int afkey = 0;
		bool enabled = false;
		int aimkey = 1;
		int fov = 0;
		int smoof = 0;
		int hitboxes = 0;
		bool head = true;
		bool neck = true;
		bool chest = true;
		bool stomach = true;
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
		int mindmg = 1;
		bool rcs = false;
		int rcstype = 1;
		int x = 100;
		int y = 100;
		bool autorevolver1 = false;
		bool autorevolver2 = false;
		bool fastaimbot = false;
		bool zeusbot = false;
		bool safehead = false;
		bool rageaim = false;
		bool dt = false;
		int dthotkey = 0;
		int dtticks = 6;
		bool hs = false;
		int hshotkey = 0;
		bool rageresolver = false;

		bool backtrack = false;
		bool aimatbt = false;
		int dmgovrd = 1;
		int dmgovrdk = 0;
		bool autostop = false;
		bool humanized = false;
		bool noaonspec = false;
		int dthc = 0;
		bool printresolver = false;
	} aimbot;

	bool flonpeek = false;
	int flop = 0;

	bool showbt = false;

	bool randomizefake = false;
	bool urrstuck = false;
	int urrstuckkey;
	bool nopacketonshot = false;

	bool playerind = false;
	bool velgraphline = false;
	bool drugs = false;
	bool slidebruh = false;
	bool fogoverride = false;
	int spectators = 0;
	int specx = 1;
	int specy = 1;
	int fakespinsp = 1;
	bool ragebot_enabled = false;
	bool ragebot_backtrack = false;
	int ragebot_fov = 0;
	bool ragebot_autozeus = false;
	bool dormantesp = false;
	int autostrafemode = 0;
	bool headthing = false;
	/*bool ragebot_hitbox[8][9] = {false, false, false, false, false, false, false, false};
	float ragebot_hitbox_multipoint_scale[8][9] = { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
	int ragebot_selection[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	bool ragebot_autoscope[9] = { false, false, false, false, false, false, false, false };
	bool ragebot_autostop[9] = { false, false, false, false, false, false, false, false };
	bool ragebot_autocrouch[9] = { false, false, false, false, false, false, false, false };
	float ragebot_mindamage[9] = { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
	float ragebot_hitchance[9] = { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
	int ragebot_baim_after_shots[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };*/
	int invertaa = 0;
	bool speclist = false;
	bool faresp = false;
	bool noscope = false;
	bool fovscope = false;
	bool viewmodelinscope = false;
	int fakedegree = 34;
	int fakepreset;
	int indicatorhb = 0;
	bool blockbot = false;
	int bbkey = 0;
	int diarkey = 0;
	int fogfardamn = 0;
	int fogdens = 0;
	bool flashkillcheck = false;
	float amibence = 0;
	bool quickstop = false;
	int fakepingkey = 0;
	bool localchams = false;
	bool teamchams = false;
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
	bool skeletonesp = false;
	bool snaplines = false;
	float skelethicc = 1.f;
	float snapthicc = 1.f;
	int boxtype = 0;
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
	bool chams_fake_enabled = false;
	bool chams_strap_enabled = false;

	bool drawogplaya = false;

	//
	// MISC
	//
	bool forceangles = false;
	int movementrecord = 0;
	int movementplay = 0;

	int spinrollspeed = 1;
	int spinroll2speed = 1;
	bool spinroll = false;
	bool spinroll2 = false;
	int jitterdegree = 0;

	int chatspamtype = 0;
	int customreal = 0;
	bool enabletest = false;
	float fart = 0;
	bool rankreveal = false;
	bool thirdinspec = false;
	bool propstoo = false;
	bool fakeduck = false;
	int fdkey = 0;
	bool changemats = false;
	bool ducknair = false;
	bool fullbright = false;
	bool misc_chatspam = false;
	bool nocool = false;
	int ragebot_antiaim_pitch = 0;
	int ragebot_antiaim_yaw = 0;
	bool ragebot_antiaim_desync = false;
	int ragebot_slowwalk_amt = 0;
	int ragebot_slowwalk_key = 0;

	bool glow_enabled = true;
	bool glow_enemies_only = true;
	bool glow_players = true;
	bool glow_chickens = false;
	bool glow_c4_carrier = false;
	bool glow_planted_c4 = false;
	bool glow_nades = false;
	bool glow_defuse_kits = false;
	bool glow_weapons = false;

	Color color_glow_ally = { 255, 255, 255, 255 };
	Color color_glow_enemy = { 255, 255, 255, 255 };
	Color color_glow_chickens = { 255, 255, 255, 255 };
	Color color_glow_c4_carrier = { 255, 255, 255, 255 };
	Color color_glow_planted_c4 = { 255, 255, 255, 255 };
	Color color_glow_defuse = { 255, 255, 255, 255 };
	Color color_glow_weapons = { 255, 255, 255, 255 };

	Color color_glow_nades = { 255, 255, 255, 255 };
	
	bool fakelag = false;
	float roll = 0.f;
	float realroll = 0.f;
	bool slowwalk = false;
	bool antiobs = false;
	bool drawfov = false;
	int clantagtype = 0;
	int hitmarkersound = 0;
	bool misc_hitmarker = false;
	bool breaklby = false;
	int breakertp = 0;
	bool rcross = false;
	int misc_thirdperson_key = 0;
	bool misc_thirdperson = false;
	bool pnade = false;
	bool fatassmf = false;
	int spinspeed = 1;
	bool sky_changer = false;
	int skyshitsssss = 0;
	bool removeblur = false;
	bool misc_bhop = false;
	bool misc_bhop2 = false;
	int playerModelT{ 0 };
	int playerModelCT{ 0 };
	float viewmodel_fov = 0;
	float viewmodel_offset_x = 3.5;
	float viewmodel_offset_y = 3;
	float viewmodel_offset_z = -3.5;
	float viewmodel_offset_roll = 0.f;
	int fovchangaaa = 0;
	bool metallica = false;
	bool ragfloat = false;
	bool matrix = false;
	bool misc_watermark = true;
	bool clantag = false;
	bool skychange = false;
	bool namec = false;
	bool pearlescent = false;
	bool Velocity = false;
	bool outline = false;
	bool lastjump = false;
	bool lastjumpoutline = false;
	bool autoaccept = false;
	bool no_flash = false;
	bool no_smoke = false;
	int AutoStafe_key = 0;
	bool autostrafe = false;
	int faketicks = 1;
	bool fakeping = false;
	int fakepingzzz = 0;
	struct
	{
		bool enabled = false;

		int hotkey = 0;
	} edgejump;
	bool edge_bug;
	int edge_bug_key;
	bool longjump = false;
	int ljkey;
	bool jump_bug = false;
	int jump_bug_key;
	int glow_enemies_type = 0;
	bool sniper_xhair = false;

	// 
	// COLORS
	// 
	Color color_indicator = { 255, 255, 255, 255 };
	Color color_backtrackind = { 255, 255, 255, 255 };
	Color color_worldglow = {0, 0, 0, 255};
	Color color_cumming = { 255, 255, 255, 255 };
	Color color_skemletom = { 255, 255, 255, 255 };
	Color nightmodecolor = { 255, 255, 255, 255 };
	Color propcolor = { 255, 255, 255, 255 };
	Color color_esp_ally_visible = { 0, 95, 255, 255 };
	Color color_esp_enemy_visible = { 135,255,0,255 };
	Color color_esp_ally_occluded = { 255, 0, 75, 255 };
	Color color_esp_enemy_occluded = { 0,105,255,175 };
	Color color_esp_weapons = { 255, 255, 255, 255 };
	Color Velocitycol = { 255, 255, 255 , 255 };
	Color color_dropped_weapons = { 255, 255, 255 , 255 };
	Color color_armour_player = { 0, 35, 255, 255 };
	Color color_name_player = { 255, 255, 255, 255 };
	Color color_chams_player_ally_visible = { 0, 95, 255, 255 };
	Color color_chams_player_ally_occluded = { 255, 0, 75, 255 };
	Color color_chams_player_local_visible = { 255, 130, 0, 255 };
	Color color_chams_player_local_occluded = { 155, 30, 0, 170 };
	Color color_chams_player_enemy_visible = { 135,255,0,255 };
	Color color_chams_player_enemy_occluded = { 0,105,255,175 };

	Color color_chams_fake = { 255,255,255,255 };

	Color color_chams_strap_visible = { 255, 255, 255, 255 };
	Color color_chams_arms_visible = { 152,255,0,255 };
	Color color_chams_sleeve_visible = { 152,255,0,255 };
	Color player_enemy_visible_shine = { 255, 255, 255, 255 };
	Color ohthuhshine = { 255, 255, 255, 255 };
	Color glowcolor = { 255, 255, 255 };
	Color glowcolorenemy = { 255, 255, 255 };
	Color glowcolorlocal = { 255, 255, 255 };
	Color glowcolorarms = { 255, 255, 255 };
	Color glowcolorstrap = { 255, 255, 255 };
	Color fogcoluh = { 255, 255, 255, 255 };
	Color menucolor = { 145,255,0,255 };
	int testint = 10;
	int testint2 = 10;
	int testint3 = 0;
	int testint4 = 0;

protected:
	//std::vector<ConfigValue<char>*> chars;
	std::vector<ConfigValue<int>*> ints;
	std::vector<ConfigValue<bool>*> bools;
	std::vector<ConfigValue<float>*> floats;
	std::vector<ConfigValue<char>*> chars;
private:
	//	void SetupValue(char value, std::string category, std::string name);
	void SetupValue(int& value, std::string category, std::string name);
	void SetupValue(bool& value, std::string category, std::string name);
	void SetupValue(float& value, std::string category, std::string name);
	void SetupColor(Color& value, const std::string& name);
	void SetupChar(char& value, std::string category, std::string name);
	void SetupWeapons();
	void SetupVisuals();
	void SetupMisc();
public:
	void Initialize();
	void LoadSettings(const std::string& szIniFile);
	void SaveSettings(const std::string& szIniFile);
	void DeleteSettings(const std::string& szIniFile);

	std::string folder;
};

inline Options g_Options;
inline bool   g_Unload;