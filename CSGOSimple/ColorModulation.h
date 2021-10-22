#include "./options.hpp"
#include "singleton.hpp"

class CNightmode : public Singleton<CNightmode>
{
public:
	std::string OldSkyname = "";
	bool NightmodeDone = false;
	void PerformNightmode();
};