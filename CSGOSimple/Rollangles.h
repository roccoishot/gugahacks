#include "./valve_sdk/csgostructs.hpp"
#include "./helpers/math.hpp"
#include "./options.hpp"
#include <d3dx9math.h>

class RollAngles : public Singleton<RollAngles>
{
public:
	void Roll(CUserCmd* cmd, bool& bSendPacket);
};