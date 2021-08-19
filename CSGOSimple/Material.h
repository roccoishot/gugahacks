#pragma once

#include <tuple>

#include "../CSGOSimple/VirtualMethod.h"

class MaterialVar {
public:
    VIRTUAL_METHOD(void, setValue, 4, (float value), (this, value))
        VIRTUAL_METHOD(void, setVectorValue, WIN32_LINUX(11, 12), (float x, float y, float z), (this, x, y, z))
        VIRTUAL_METHOD(void, setVecComponentValue, 26, (float value, int component), (this, value, component))
};

enum class MaterialVarFlag {
    NO_DRAW = 1 << 2,
    IGNOREZ = 1 << 15,
    WIREFRAME = 1 << 28
};

class Material {
public:
    VIRTUAL_METHOD(const char*, getName, 0, (), (this))
       
};
