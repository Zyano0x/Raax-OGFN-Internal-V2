#include "hooks.h"
#include <extern/minhook/include/MinHook.h>
#include <utils/log.h>

namespace Hooks {

// --- Initialization ------------------------------------------------

bool Init() {
    LOG(LOG_TRACE, "Setting up Hooks...");

    MH_STATUS result = MH_Initialize();
    if (result == MH_OK)
        return true;

    LOG(LOG_WARN, "MH_Initialize failed! (%d, %s)", result, MH_StatusToString(result));
    return false;
}

void Destroy() {
    LOG(LOG_TRACE, "Destroying Hooks...");
    MH_Uninitialize();
}

// --- Hook Management -----------------------------------------------

bool CreateHook(void* target, void* detour, void** original) {
    MH_STATUS result = MH_CreateHook(target, detour, original);
    if (result == MH_OK)
        return true;

    LOG(LOG_WARN, "MH_CreateHook failed! (%d, %s)", result, MH_StatusToString(result));
    return false;
}

bool EnableHook(void* target) {
    MH_STATUS result = MH_EnableHook(target);
    if (result == MH_OK)
        return true;

    LOG(LOG_WARN, "MH_EnableHook failed! (%d, %s)", result, MH_StatusToString(result));
    return false;
}

bool RemoveHook(void* target) {
    MH_STATUS result = MH_RemoveHook(target);
    if (result == MH_OK)
        return true;

    LOG(LOG_WARN, "MH_RemoveHook failed! (%d, %s)", result, MH_StatusToString(result));
    return false;
}

bool DisableHook(void* target) {
    MH_STATUS result = MH_DisableHook(target);
    if (result == MH_OK)
        return true;

    LOG(LOG_WARN, "MH_DisableHook failed! (%d, %s)", result, MH_StatusToString(result));
    return false;
}

} // namespace Hooks
