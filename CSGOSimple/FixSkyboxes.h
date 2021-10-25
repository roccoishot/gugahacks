#include "options.hpp"
#include "./singleton.hpp"

class Fixed : public Singleton<Fixed>
{
public:
	std::string OldSkyname = "";
	bool NightmodeDone = true;
	void PerformNightmode();
};
