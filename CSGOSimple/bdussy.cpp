#include "./valve_sdk/csgostructs.hpp"
#include "./helpers/math.hpp"
#include "./options.hpp"
#include <d3dx9math.h>
#include "sexynutssexyfuckdickshitnigga.h"
#include <iomanip>
#include <sstream>
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
#include "Globals.h"
#include <ShlObj.h>
#include <filesystem>
namespace fs = std::experimental::filesystem;

void movement_recorder::run(CUserCmd* cmd) {
	record_movement(cmd);
	play_movement(cmd);
}


void movement_recorder::record_movement(CUserCmd* cmd) {

	static bool in_record = false;
	static int init = 0;

	auto local = g_LocalPlayer;

	if (!g_EngineClient->IsInGame() || !local || !local->IsAlive()) {
		in_record = false;
		return;
	}

	static record recording;

	static int number = 0;

	if (GetKeyState(g_Options.movementrecord))
		in_record = true;

	if (in_record) {
		if (!init)
			recording.start_pos = local->m_vecOrigin();

		init++;

		record_info command;
		command.buttons = cmd->buttons;
		command.forwardmove = cmd->forwardmove;
		command.sidemove = cmd->sidemove;
		command.viewangle = cmd->viewangles;

		command.number = init;

		recording.info.push_back(command);
	}

	if (in_record && init) {
		if (!GetKeyState(g_Options.movementrecord)) {

			recording.size = init + 1;

			//recordings.push_back(recording);
			save_record(recording);

			load_records();


			double pi = TICKS_TO_TIME(recording.info.size() + 1);
			std::stringstream stream;
			stream << std::fixed << std::setprecision(2) << pi;
			std::string s = stream.str();
			g_ChatElement->ChatPrintf(0,0, std::string(" \x01 [\x06 GUGAHACKS \x01]\x01 Saved recording with \x06" + std::to_string(recording.info.size() + 1) + "\x01 commands, \x06" + s + "\x01s.").c_str());

			recording.start_pos = Vector(0, 0, 0);
			recording.info.clear();

			init = 0;
			in_record = false;
		}
	}

}

void movement_recorder::play_movement(CUserCmd* cmd) {

	static bool playback = false;

	static int command = 0;

	auto local = g_LocalPlayer;


	current_recordings.clear();

	for (auto rec : recordings) {
		//if (strcmp(rec.map, g_EngineClient->GetLevelNameShort()) == 0)
		current_recordings.push_back(rec);
	}


	if (!g_EngineClient->IsInGame() || !local || !local->IsAlive()) {
		playback = false;
		command = 0;
		return;
	}

	if (!current_recordings.size()) {
		playback = false;
		command = 0;
		return;
	}


	if (GetAsyncKeyState(g_Options.movementplay))
		playback = true;


	static int at_start = 0;

	static record play_record;

	if (!at_start)
		play_record = record(Vector(FLT_MAX, FLT_MAX, FLT_MAX));

	for (auto rec : current_recordings) {
		if (!at_start && local->m_vecOrigin().DistTo(rec.start_pos) < local->m_vecOrigin().DistTo(play_record.start_pos))
			play_record = rec;
	}


	if (local->m_vecOrigin().DistTo(play_record.start_pos) < 0.1f && local->m_vecVelocity().Length() < 1.f)
		at_start++;


	if (!at_start && playback) {
		float wish_yaw = cmd->viewangles.yaw;
		Vector difference = local->m_vecOrigin() - play_record.start_pos;

		auto translatedVelocity = Vector(difference.x * cos(wish_yaw / 180.0f * M_PI) + difference.y * sin(wish_yaw / 180.0f * M_PI), difference.y * cos(wish_yaw / 180.0f * M_PI) - difference.x * sin(wish_yaw / 180.0f * M_PI), difference.z);

		cmd->forwardmove = Math::clamp(-translatedVelocity.x * 11.f, -250.f, 250.f);
		cmd->sidemove = Math::clamp(translatedVelocity.y * 11.f, -250.f, 250.f);
	}


	if (playback && at_start) {
		record_info info = play_record.info.at(command);


		cmd->buttons = info.buttons;
		cmd->forwardmove = info.forwardmove;
		cmd->sidemove = info.sidemove;
		cmd->viewangles = info.viewangle;

		if (!g_Input->m_fCameraInThirdPerson && g_Options.forceangles)
			g_EngineClient->SetViewAngles(&cmd->viewangles);

		if (!(info.number == play_record.info.back().number))
			command++;

	}


	if (playback) {
		if (!GetAsyncKeyState(g_Options.movementplay)) {
			playback = false;
			command = 0;
			at_start = 0;
		}
	}
}

void movement_recorder::save_record(record rec) {

	std::string name(g_EngineClient->GetLevelNameShort() + std::string("_") + std::to_string(Math::random_int(0, INT_MAX)));
	//std::string name("recording");
	std::string folder;
	if (!Globals::cfgfldr.empty())
		folder = Globals::cfgfldr;

	std::string file(folder + std::string("\\records\\") + name + std::string(".rec"));

	WritePrivateProfileStringA("record", "map", std::string(g_EngineClient->GetLevelNameShort()).c_str(), file.c_str());
	WritePrivateProfileStringA("record", "start_pos_x", std::to_string(rec.start_pos.x).c_str(), file.c_str());
	WritePrivateProfileStringA("record", "start_pos_y", std::to_string(rec.start_pos.y).c_str(), file.c_str());
	WritePrivateProfileStringA("record", "start_pos_z", std::to_string(rec.start_pos.z).c_str(), file.c_str());

	WritePrivateProfileStringA("record", "size", std::to_string(rec.size).c_str(), file.c_str());

	for (auto info : rec.info) {

#define catagory std::string("cmd " + std::to_string(info.number)).c_str()
		WritePrivateProfileStringA(catagory, "cmd number", std::to_string(info.number).c_str(), file.c_str());

		WritePrivateProfileStringA(catagory, "forwardmove", std::to_string(info.forwardmove).c_str(), file.c_str());
		WritePrivateProfileStringA(catagory, "sidemove", std::to_string(info.sidemove).c_str(), file.c_str());
		WritePrivateProfileStringA(catagory, "buttons", std::to_string(info.buttons).c_str(), file.c_str());

		WritePrivateProfileStringA(catagory, "viewangle_x", std::to_string(info.viewangle.pitch).c_str(), file.c_str());
		WritePrivateProfileStringA(catagory, "viewangle_y", std::to_string(info.viewangle.yaw).c_str(), file.c_str());
		WritePrivateProfileStringA(catagory, "viewangle_z", std::to_string(info.viewangle.roll).c_str(), file.c_str());
	}
}

void movement_recorder::load_records() {

	//recordings.push_back(recording);

	auto local = g_LocalPlayer;

	if (!g_EngineClient->IsInGame() || !local)
		return;


	recordings.clear();

	static std::vector<std::string> configs;

	std::vector<std::string> items = {};

	std::string folder;
	if (!Globals::cfgfldr.empty())
		folder = Globals::cfgfldr;

	std::string path(folder + std::string("\\records"));
	if (!fs::is_directory(path))
		fs::create_directories(path);

	for (auto& p : fs::directory_iterator(path)) {
		if (p.path().extension() == XorStr(".rec")) {
			items.push_back(p.path().string().substr(path.length() + 1));
		}
	}

	configs = items;


	for (auto& config : configs) {

		record load_record;

		std::string folder;
		if (!Globals::cfgfldr.empty())
			folder = Globals::cfgfldr;

		std::string file = folder + std::string("\\records\\") + config;

		char map[32] = { '\0' };
		GetPrivateProfileStringA("record", "map", "", map, 32, file.c_str());

		if (!(strcmp(map, g_EngineClient->GetLevelNameShort()) == 0))
			continue;

		load_record.map = map;

		char value_l[32] = { '\0' };

		GetPrivateProfileStringA("record", "start_pos_x", "", value_l, 32, file.c_str());
		load_record.start_pos.x = atof(value_l);

		GetPrivateProfileStringA("record", "start_pos_y", "", value_l, 32, file.c_str());
		load_record.start_pos.y = atof(value_l);

		GetPrivateProfileStringA("record", "start_pos_z", "", value_l, 32, file.c_str());
		load_record.start_pos.z = atof(value_l);

		GetPrivateProfileStringA("record", "size", "", value_l, 32, file.c_str());
		load_record.size = atoi(value_l);

		//load_record.name = (char*)config.c_str();
		load_record.name = strdup(config.c_str());


		for (int i = 1; i < (load_record.size - 1); i++) {
			record_info info;

#define catagory std::string("cmd " + std::to_string(i)).c_str()

			GetPrivateProfileStringA(catagory, "cmd number", "", value_l, 32, file.c_str());
			info.number = atoi(value_l);

			GetPrivateProfileStringA(catagory, "forwardmove", "", value_l, 32, file.c_str());
			info.forwardmove = atof(value_l);

			GetPrivateProfileStringA(catagory, "sidemove", "", value_l, 32, file.c_str());
			info.sidemove = atof(value_l);

			GetPrivateProfileStringA(catagory, "buttons", "", value_l, 32, file.c_str());
			info.buttons = atoi(value_l);

			GetPrivateProfileStringA(catagory, "viewangle_x", "", value_l, 32, file.c_str());
			info.viewangle.pitch = atof(value_l);

			GetPrivateProfileStringA(catagory, "viewangle_y", "", value_l, 32, file.c_str());
			info.viewangle.yaw = atof(value_l);

			GetPrivateProfileStringA(catagory, "viewangle_z", "", value_l, 32, file.c_str());
			info.viewangle.roll = atof(value_l);



			load_record.info.push_back(info);
		}
		recordings.push_back(load_record);
	}
}