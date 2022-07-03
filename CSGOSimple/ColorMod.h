#pragma once
#include "./options.hpp"
#include "valve_sdk/sdk.hpp"

class CNightmode : public Singleton<CNightmode>
{
public:
	std::string OldSkyname = "";
	bool wason = false;
	void PerformNightmode();
	void OnDME(void* pResults, DrawModelInfo_t* pInfo, matrix3x4_t* pBoneToWorld, float* flpFlexWeights, float* flpFlexDelayedWeights, Vector& vrModelOrigin, int32_t iFlags);
	void GtfoNight();
};
