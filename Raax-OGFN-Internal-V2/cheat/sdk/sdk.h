#pragma once
#include "FMemory.h"
#include "CoreUObject.h"
#include "Engine.h"
#include "FortniteGame.h"

namespace SDK
{
    bool Init();

    inline float EngineVersion = 0.f;
    inline float GameVersion = 0.f;
    inline int CL = 0;
}
