#include "Globals.h"
#include "helpers/utils.hpp"

namespace Globals
{
	QAngle aim_punch_old;
	QAngle view_punch_old;
	QAngle LastAngle = QAngle(0, 0, 0);
	bool ThirdPersponToggle = true;
	bool Unload = false;
	bool MenuOpened = false;
	bool freezetime = false;
	bool clear = false;
	bool serverspawn = false;
	bool hitplayer = false;
	bool candt = false;
	bool hitplayer2 = false;
	bool flcur = false;
	bool abfr = false;
	int playerid = 0;
	int bestbtid = -1;
	bool PlayerListOpened = false;
	bool RadioOpened = false;
	std::string cfgfldr;
	bool EBD = false;
	bool WeaponTabValid = false;
	bool cuhbedointhat = false;
	float roll = 0.f;
	IGameEvent* eventgl;
	float realroll = 0.f;
	bool valve = false;
	bool headthing = false;
	bool killedplayer = false;
	bool discham = false;
	bool stepping = false;
	CUserCmd* m_cmd = nullptr;
	bool flicky = false;
	bool bSendPacket = false;
	bool dting = false;
	bool rendered = false;
	float real_angle, fake_angle = 0.f;
	QAngle real = {0,0,0}; 
	QAngle fake = { 0,0,0 };
	Vector realabs = { 0,0,0 };
	Vector fakeabs = { 0,0,0 };
	bool startcharge, is_shifting = false;
	int tocharge, tochargeamount, ticks_allowed, shift_ticks = 0;

	bool inprediction;
}