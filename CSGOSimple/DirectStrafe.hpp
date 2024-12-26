#include "singleton.hpp"
#include "options.hpp"
#include "valve_sdk/csgostructs.hpp"
#define M_RADPI 57.295779513082f

class airstrafe : public Singleton <airstrafe>
{
public:
	void create_move(CUserCmd* m_pcmd);
};