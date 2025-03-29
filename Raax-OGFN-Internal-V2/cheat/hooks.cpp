#include "hooks.h"
#include <extern/minhook/include/MinHook.h>

bool Hooks::Init()
{
    return MH_Initialize() == MH_OK;
}

void Hooks::Destroy()
{
    MH_Uninitialize();
}


bool Hooks::CreateHook(void* Target, void* Detour, void** Original)
{
    return MH_CreateHook(Target, Detour, Original) == MH_OK;
}

bool Hooks::EnableHook(void* Target)
{
    return MH_EnableHook(Target) == MH_OK;
}

bool Hooks::RemoveHook(void* Target)
{
    return MH_RemoveHook(Target) == MH_OK;
}

bool Hooks::DisableHook(void* Target)
{
    return MH_DisableHook(Target) == MH_OK;
}
