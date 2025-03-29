#include "hooks.h"
#include <extern/minhook/include/MinHook.h>
#include <utils/log.h>

bool Hooks::Init()
{
    MH_STATUS Result = MH_Initialize();
    if (Result == MH_OK)
        return true;

    LOG(LOG_WARN, "MH_Initialize failed! (%d, %s)", Result, MH_StatusToString(Result));
    return false;
}

void Hooks::Destroy()
{
    MH_Uninitialize();
}


bool Hooks::CreateHook(void* Target, void* Detour, void** Original)
{
    MH_STATUS Result = MH_CreateHook(Target, Detour, Original);
    if (Result == MH_OK)
        return true;

    LOG(LOG_WARN, "CreateHook failed! (%d, %s)", Result, MH_StatusToString(Result));
    return false;
}

bool Hooks::EnableHook(void* Target)
{
    MH_STATUS Result = MH_EnableHook(Target);
    if (Result == MH_OK)
        return true;

    LOG(LOG_WARN, "EnableHook failed! (%d, %s)", Result, MH_StatusToString(Result));
    return false;
}

bool Hooks::RemoveHook(void* Target)
{
    MH_STATUS Result = MH_RemoveHook(Target);
    if (Result == MH_OK)
        return true;

    LOG(LOG_WARN, "RemoveHook failed! (%d, %s)", Result, MH_StatusToString(Result));
    return false;
}

bool Hooks::DisableHook(void* Target)
{
    MH_STATUS Result = MH_DisableHook(Target);
    if (Result == MH_OK)
        return true;

    LOG(LOG_WARN, "DisableHook failed! (%d, %s)", Result, MH_StatusToString(Result));
    return false;
}
