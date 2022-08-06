#pragma once
#include "./options.hpp"
#include "valve_sdk/sdk.hpp"

class CNightmode : public Singleton<CNightmode>
{
public:
	std::string OldSkyname = "";
	bool wason = false;
	void PerformNightmode();
	void GtfoNight();
};
