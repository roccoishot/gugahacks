#pragma once

#define NOMINMAX
#include <Windows.h>
#include <string>
#include <initializer_list>
#include "../valve_sdk/sdk.hpp"

namespace Utils {
	unsigned int FindInDataMap(datamap_t * pMap, const char * name);
    /*
     * @brief Create console
     *
     * Create and attach a console window to the current process
     */
	void AttachConsole();

    /*
     * @brief Detach console
     *
     * Detach and destroy the attached console
     */
    void DetachConsole();

    /*
     * @brief Print to console
     *
     * Replacement to printf that works with the newly created console
     */
    bool ConsolePrint(const char* fmt, ...);
    
    /*
     * @brief Blocks execution until a key is pressed on the console window
     *
     */
    char ConsoleReadKey();

    /*
     * @brief Wait for all the given modules to be loaded
     *
     * @param timeout How long to wait
     * @param modules List of modules to wait for
     *
     * @returns See WaitForSingleObject return values.
     */
    int WaitForModules(std::int32_t timeout, const std::initializer_list<std::wstring>& modules);

    /*
     * @brief Scan for a given byte pattern on a module
     *
     * @param module    Base of the module to search
     * @param signature IDA-style byte array pattern
     *
     * @returns Address of the first occurence
     */
    std::uint8_t* PatternScan(void* module, const char* signature);

    /*
     * @brief Set player clantag
     *
     * @param tag New clantag
     */
    void SetClantag(const char* tag);

    /*
     * @brief Set player name
     *
     * @param name New name
     */
    bool SetName(const char* name);

	/*
	* @brief Set player name with delay
	*
	* @param reconnect
	* @param name New name
	* @param delay to set New names
	*/
	bool SetName2(bool reconnect, const char* newName, float delay);

    /*
     * @brief Reveal the ranks of all players on the server
     *
     */
    void RankRevealAll();
    int random(const int& min, const int& max);
    float random(const float& min, const float& max);
    template <typename T = void*>
    inline constexpr T GetVFunc(void* thisptr, std::size_t nIndex)
    {
        return (T)((*(std::uintptr_t**)thisptr)[nIndex]);
    }
}
