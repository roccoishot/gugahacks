#include "valve_sdk/sdk.hpp"

class CHudChat
{
public:
	char pad_0x0000[0x4C];
	int m_timesOpened;
	char pad_0x0050[0x8];
	bool m_isOpen;
	char pad_0x0059[0x427];

	void ChatPrintf(int iPlayerIndex, int iFilter, const char* fmt, ...)
	{
		CallVFunction<void(__cdecl*)(void*, int, int, const char*, ...)>(this, 27)(this, iPlayerIndex, iFilter, fmt);
	}
};
