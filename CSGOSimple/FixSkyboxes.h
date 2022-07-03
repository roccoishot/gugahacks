#include "options.hpp"
#include "./singleton.hpp"

class Fixed : public Singleton<Fixed>
{
public:
	bool NightmodeDone = true;
	void PerformNightmode();
};
