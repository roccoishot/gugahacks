#pragma once

#include "../singleton.hpp"
#include "../valve_sdk/sdk.hpp"
#include <stdio.h>
#include <string>
#include <iostream>


class IMatRenderContext;
struct DrawModelState_t;
struct ModelRenderInfo_t;
class matrix3x4_t;
class IMaterial;
class Color;

class Chams
    : public Singleton<Chams>
{
    friend class Singleton<Chams>;
    Chams();
    ~Chams();

public:
    void OnDrawModelExecute(
        IMatRenderContext* ctx,
        const DrawModelState_t& state,
        const ModelRenderInfo_t& pInfo,
        matrix3x4_t* pCustomBoneToWorld);

public:
    void OnDrawModelExecute(void* pResults, DrawModelInfo_t* pInfo, matrix3x4_t* pBoneToWorld, float* flpFlexWeights, float* flpFlexDelayedWeights, Vector& vrModelOrigin, int32_t iFlags);

private:
    void OverrideMaterial(bool ignoreZ, bool flat, bool wireframe, bool glass, const Color& rgba);

    void OnDrawModelExecute(void* pResults, DrawModelInfo_t* pInfo, matrix3x4_t* pBoneToWorld, float* flpFlexWeights, float* flpFlexDelayedWeights, Vector& vrModelOrigin, int32_t iFlags, IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* matrix);

    IMaterial* materialRegular = nullptr;
    IMaterial* materialFlat = nullptr;
};