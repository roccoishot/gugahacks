#include "Globals.h"
#include "helpers/utils.hpp"
namespace Globals
{
	QAngle LastAngle = QAngle(0, 0, 0);
	bool ThirdPersponToggle = true;
	bool Unload = false;
	bool MenuOpened = false;
	bool PlayerListOpened = false;
	bool RadioOpened = false;
	bool WeaponTabValid = false;
	CUserCmd* m_cmd = nullptr;
	float real_angle, fake_angle = 0.f;
}
