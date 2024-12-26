#pragma once

#include <string>
#include "singleton.hpp"
#include "imgui/imgui.h"
#include "xor.h"
#include <d3dx9tex.h>

#ifdef ENABLE_XOR
#define XorStr _xor_ 
#else
#define XorStr
#endif
#pragma intrinsic(_ReturnAddress)  

struct IDirect3DDevice9;

template <class T>
static T* FindHudElement(const char* name)
{
	static DWORD* pThis = nullptr;

	if (!pThis)
	{
		static auto pThisSignature = Utils::PatternScan2(XorStr("client.dll"), XorStr("B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B 5D 08"));

		if (pThisSignature)
			pThis = *reinterpret_cast<DWORD**>(pThisSignature + 0x1);
	}

	if (!pThis)
		return 0;

	static auto find_hud_element = reinterpret_cast<DWORD(__thiscall*)(void*, const char*)>(Utils::PatternScan2(XorStr("client.dll"), XorStr("55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28")));
	return (T*)find_hud_element(pThis, name); //-V204
}

class Menu
    : public Singleton<Menu>
{
public:
    void Initialize();
    void Shutdown();

    void OnDeviceLost();
    void OnDeviceReset();

    void Render();
    void SpecList();
    void Toggle();

    bool IsVisible() const { return _visible; }

    float alpha = 0.f;
    float widget_alpha = 0.f;
private:
    void CreateStyle();

    ImGuiStyle        _style;
    bool              _visible;

    LPDIRECT3DTEXTURE9 bg = nullptr;
    LPDIRECT3DTEXTURE9 guger = nullptr;
};