#include "Utils.hpp"

#define NOMINMAX
#include <Windows.h>
#include <stdio.h>
#include <string>
#include <vector>

#include "../valve_sdk/csgostructs.hpp"
#include "Math.hpp"
#include <map>
#include <algorithm>


HANDLE _out = NULL, _old_out = NULL;
HANDLE _err = NULL, _old_err = NULL;
HANDLE _in = NULL, _old_in = NULL;
std::map<const char*, HMODULE> modules;

namespace Utils {
	std::vector<char> HexToBytes(const std::string& hex) {
		std::vector<char> res;

		for (auto i = 0u; i < hex.length(); i += 2) {
			std::string byteString = hex.substr(i, 2);
			char byte = (char)strtol(byteString.c_str(), NULL, 16);
			res.push_back(byte);
		}

		return res;
	}
	std::string BytesToString(unsigned char* data, int len) {
		constexpr char hexmap[] = { '0', '1', '2', '3', '4', '5', '6', '7',
                                    '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
		std::string res(len * 2, ' ');
		for (int i = 0; i < len; ++i) {
			res[2 * i] = hexmap[(data[i] & 0xF0) >> 4];
			res[2 * i + 1] = hexmap[data[i] & 0x0F];
		}
		return res;
	}
	std::vector<std::string> Split(const std::string& str, const char* delim) {
		std::vector<std::string> res;
		char* pTempStr = _strdup(str.c_str());
		char* context = NULL;
		char* pWord = strtok_s(pTempStr, delim, &context);
		while (pWord != NULL) {
			res.push_back(pWord);
			pWord = strtok_s(NULL, delim, &context);
		}

		free(pTempStr);

		return res;
	}

	unsigned int FindInDataMap(datamap_t *pMap, const char *name) {
		while (pMap) {
			for (int i = 0; i<pMap->dataNumFields; i++) {
				if (pMap->dataDesc[i].fieldName == NULL)
					continue;

				if (strcmp(name, pMap->dataDesc[i].fieldName) == 0)
					return pMap->dataDesc[i].fieldOffset[TD_OFFSET_NORMAL];

				if (pMap->dataDesc[i].fieldType == FIELD_EMBEDDED) {
					if (pMap->dataDesc[i].td) {
						unsigned int offset;

						if ((offset = FindInDataMap(pMap->dataDesc[i].td, name)) != 0)
							return offset;
					}
				}
			}
			pMap = pMap->baseMap;
		}

		return 0;
	}
    /*
     * @brief Create console
     *
     * Create and attach a console window to the current process
     */
    void AttachConsole()
    {
        _old_out = GetStdHandle(STD_OUTPUT_HANDLE);
        _old_err = GetStdHandle(STD_ERROR_HANDLE);
        _old_in  = GetStdHandle(STD_INPUT_HANDLE);

        ::AllocConsole() && ::AttachConsole(GetCurrentProcessId());

        _out     = GetStdHandle(STD_OUTPUT_HANDLE);
        _err     = GetStdHandle(STD_ERROR_HANDLE);
        _in      = GetStdHandle(STD_INPUT_HANDLE);

        SetConsoleMode(_out,
            ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);

        SetConsoleMode(_in,
            ENABLE_INSERT_MODE | ENABLE_EXTENDED_FLAGS |
            ENABLE_PROCESSED_INPUT | ENABLE_QUICK_EDIT_MODE);
    }

    /*
     * @brief Detach console
     *
     * Detach and destroy the attached console
     */
    void DetachConsole()
    {
        if(_out && _err && _in) {
            FreeConsole();

            if(_old_out)
                SetStdHandle(STD_OUTPUT_HANDLE, _old_out);
            if(_old_err)
                SetStdHandle(STD_ERROR_HANDLE, _old_err);
            if(_old_in)
                SetStdHandle(STD_INPUT_HANDLE, _old_in);
        }
    }

    /*
     * @brief Print to console
     *
     * Replacement to printf that works with the newly created console
     */
    bool ConsolePrint(const char* fmt, ...)
    {
        if(!_out) 
            return false;

        char buf[1024];
        va_list va;

        va_start(va, fmt);
        _vsnprintf_s(buf, 1024, fmt, va);
        va_end(va);

        return !!WriteConsoleA(_out, buf, static_cast<DWORD>(strlen(buf)), nullptr, nullptr);
    }

    /*
     * @brief Blocks execution until a key is pressed on the console window
     *
     */
    char ConsoleReadKey()
    {
        if(!_in)
            return false;

        auto key = char{ 0 };
        auto keysread = DWORD{ 0 };

        ReadConsoleA(_in, &key, 1, &keysread, nullptr);

        return key;
    }

    /*
     * @brief Wait for all the given modules to be loaded
     *
     * @param timeout How long to wait
     * @param modules List of modules to wait for
     *
     * @returns See WaitForSingleObject return values.
     */
    int WaitForModules(std::int32_t timeout, const std::initializer_list<std::wstring>& modules)
    {
        bool signaled[32] = { 0 };
        bool success = false;

        std::uint32_t totalSlept = 0;

        if(timeout == 0) {
            for(auto& mod : modules) {
                if(GetModuleHandleW(std::data(mod)) == NULL)
                    return WAIT_TIMEOUT;
            }
            return WAIT_OBJECT_0;
        }

        if(timeout < 0)
            timeout = INT32_MAX;

        while(true) {
            for(auto i = 0u; i < modules.size(); ++i) {
                auto& module = *(modules.begin() + i);
                if(!signaled[i] && GetModuleHandleW(std::data(module)) != NULL) {
                    signaled[i] = true;

                    //
                    // Checks if all modules are signaled
                    //
                    bool done = true;
                    for(auto j = 0u; j < modules.size(); ++j) {
                        if(!signaled[j]) {
                            done = false;
                            break;
                        }
                    }
                    if(done) {
                        success = true;
                        goto exit;
                    }
                }
            }
            if(totalSlept > std::uint32_t(timeout)) {
                break;
            }
            Sleep(10);
            totalSlept += 10;
        }

    exit:
        return success ? WAIT_OBJECT_0 : WAIT_TIMEOUT;
    }

    /*
     * @brief Scan for a given byte pattern on a module
     *
     * @param module    Base of the module to search
     * @param signature IDA-style byte array pattern
     *
     * @returns Address of the first occurence
     */
    HMODULE GetModule(const char* name)
    {
        if (!modules.count(name) || !modules[name])
            modules[name] = GetModuleHandleA(name);

        return modules[name];
    }
    std::uint8_t* PatternScan2(const char* module_name, const char* signature)
    {
        auto module = GetModule(module_name);

        static auto pattern_to_byte = [](const char* pattern) {
            auto bytes = std::vector<int>{};
            auto start = const_cast<char*>(pattern);
            auto end = const_cast<char*>(pattern) + strlen(pattern);

            for (auto current = start; current < end; ++current) {
                if (*current == '?') {
                    ++current;
                    if (*current == '?')
                        ++current;
                    bytes.push_back(-1);
                }
                else {
                    bytes.push_back(strtoul(current, &current, 16));
                }
            }
            return bytes;
        };

        auto dosHeader = (PIMAGE_DOS_HEADER)module;
        auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)module + dosHeader->e_lfanew);

        auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
        auto patternBytes = pattern_to_byte(signature);
        auto scanBytes = reinterpret_cast<std::uint8_t*>(module);

        auto s = patternBytes.size();
        auto d = patternBytes.data();

        for (auto i = 0ul; i < sizeOfImage - s; ++i) {
            bool found = true;
            for (auto j = 0ul; j < s; ++j) {
                if (scanBytes[i + j] != d[j] && d[j] != -1) {
                    found = false;
                    break;
                }
            }
            if (found) {
                return &scanBytes[i];
            }
        }
        return nullptr;
    }

    std::uint8_t* PatternScan(void* module, const char* signature)
    {
        static auto pattern_to_byte = [](const char* pattern) {
            auto bytes = std::vector<int>{};
            auto start = const_cast<char*>(pattern);
            auto end = const_cast<char*>(pattern) + strlen(pattern);

            for(auto current = start; current < end; ++current) {
                if(*current == '?') {
                    ++current;
                    if(*current == '?')
                        ++current;
                    bytes.push_back(-1);
                } else {
                    bytes.push_back(strtoul(current, &current, 16));
                }
            }
            return bytes;
        };

        auto dosHeader = (PIMAGE_DOS_HEADER)module;
        auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)module + dosHeader->e_lfanew);

        auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
        auto patternBytes = pattern_to_byte(signature);
        auto scanBytes = reinterpret_cast<std::uint8_t*>(module);

        auto s = patternBytes.size();
        auto d = patternBytes.data();

        for(auto i = 0ul; i < sizeOfImage - s; ++i) {
            bool found = true;
            for(auto j = 0ul; j < s; ++j) {
                if(scanBytes[i + j] != d[j] && d[j] != -1) {
                    found = false;
                    break;
                }
            }
            if(found) {
                return &scanBytes[i];
            }
        }
        return nullptr;
    }
   
    unsigned int find_in_datamap(datamap_t* map, const char* name)
    {
        while (map)
        {
            for (auto i = 0; i < map->dataNumFields; ++i)
            {
                if (!map->dataDesc[i].fieldName)
                    continue;

                if (!strcmp(name, map->dataDesc[i].fieldName))
                    return map->dataDesc[i].fieldOffset[TD_OFFSET_NORMAL];

                if (map->dataDesc[i].fieldType == FIELD_EMBEDDED)
                {
                    if (map->dataDesc[i].td)
                    {
                        unsigned int offset;

                        if (offset = find_in_datamap(map->dataDesc[i].td, name))
                            return offset;
                    }
                }
            }

            map = map->baseMap;
        }

        return 0;
    }

    /*
     * @brief Set player clantag
     *
     * @param tag New clantag
     */
    void SetClantag(const char* tag)
    {
        static auto fnClantagChanged = (int(__fastcall*)(const char*, const char*))PatternScan(GetModuleHandleW(L"engine.dll"), "53 56 57 8B DA 8B F9 FF 15");

        fnClantagChanged(tag, tag);
    }

    /*
     * @brief Set player name
     *
     * @param name New name
     */
    void SetName(const char* name)
    {
        static auto nameConvar = g_CVar->FindVar("name");
        nameConvar->m_fnChangeCallbacks.m_Size = 0;

        // Fix so we can change names how many times we want
        // This code will only run once because of `static`
        static auto do_once = (nameConvar->SetValue("\n���"), true);

        nameConvar->SetValue(name);
    }

    std::vector<int> GetObservervators(int playerId)
    {
        std::vector<int> SpectatorList;

        C_BasePlayer* pPlayer = (C_BasePlayer*)g_EntityList->GetClientEntity(playerId);

        if (!pPlayer)
            return SpectatorList;

        if (!pPlayer->IsAlive())
        {
            C_BasePlayer* pObserverTarget = (C_BasePlayer*)g_EntityList->GetClientEntityFromHandle(pPlayer->m_hObserverTarget());

            if (!pObserverTarget)
                return SpectatorList;

            pPlayer = pObserverTarget;
        }

        for (int PlayerIndex = 0; PlayerIndex < 65; PlayerIndex++)
        {
            C_BasePlayer* pCheckPlayer = (C_BasePlayer*)g_EntityList->GetClientEntity(PlayerIndex);

            if (!pCheckPlayer)
                continue;

            if (pCheckPlayer->IsDormant() || pCheckPlayer->IsAlive())
                continue;

            C_BasePlayer* pObserverTarget = (C_BasePlayer*)g_EntityList->GetClientEntityFromHandle(pCheckPlayer->m_hObserverTarget());

            if (!pObserverTarget)
                continue;

            if (pPlayer != pObserverTarget)
                continue;

            SpectatorList.push_back(PlayerIndex);
        }

        return SpectatorList;
    }

    bool IsDangerZone()
    {
        return g_GameTypes->GetCurrentGameType() == 6;
    }

    struct hud_weapons_t {
        std::int32_t* get_weapon_count() {
            return reinterpret_cast<std::int32_t*>(std::uintptr_t(this) + 0x80);
        }
    };
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

    void ForceFullUpdate() {
        //return;

        static auto clear_hud_weapon_icon_fn =
            reinterpret_cast<std::int32_t(__thiscall*)(void*, std::int32_t)>(
                Utils::PatternScan2("client.dll", "55 8B EC 51 53 56 8B 75 08 8B D9 57 6B"));

        auto element = FindHudElement<std::uintptr_t*>("CCSGO_HudWeaponSelection");

        if (!element)
            return;

        auto hud_weapons = reinterpret_cast<hud_weapons_t*>(std::uintptr_t(element) - 0xa0);
        if (hud_weapons == nullptr)
            return;

        if (!*hud_weapons->get_weapon_count())
            return;

        for (std::int32_t i = 0; i < *hud_weapons->get_weapon_count(); i++)
            i = clear_hud_weapon_icon_fn(hud_weapons, i);

        // typedef void(*ForceUpdate) (void);
        // static ForceUpdate FullUpdate = (ForceUpdate)Utils::PatternScan(GetModuleHandleA("engine.dll"), "A1 ? ? ? ? B9 ? ? ? ? 56 FF 50 14 8B 34 85");
       //  FullUpdate();
        g_ClientState->ForceFullUpdate();
    }

}
