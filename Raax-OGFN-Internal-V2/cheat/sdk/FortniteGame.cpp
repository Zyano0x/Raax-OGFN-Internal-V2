#include "FortniteGame.h"

namespace SDK {

// --- Classes & Structs ---------------------------------------------

void (*AFortPlayerController::pFire_Press)(void*);
void (*AFortPlayerController::pFire_Release)(void*);
void AFortPlayerController::Fire_Press() {
    if (this && pFire_Press) {
        pFire_Press(this);
    }
}
void AFortPlayerController::Fire_Release() {
    if (this && pFire_Release) {
        pFire_Release(this);
    }
}

int32_t AFortWeapon::GetBulletsPerClip() {
    static UFunction* Func = GetFunction("FortWeapon", "GetBulletsPerClip");
    struct {
        int32_t ReturnValue;
    } params_GetBulletsPerClip{};

    if (this && Func)
        ProcessEvent(Func, &params_GetBulletsPerClip);

    return params_GetBulletsPerClip.ReturnValue;
}

float AFortProjectileBase::GetDefaultGravityScale() {
    static UFunction* Func = GetFunction("FortProjectileBase", "GetDefaultGravityScale");
    struct {
        float ReturnValue;
    } params_GetDefaultGravityScale{};

    if (this && Func)
        ProcessEvent(Func, &params_GetDefaultGravityScale);

    return params_GetDefaultGravityScale.ReturnValue;
}

float AFortProjectileBase::GetDefaultSpeed(float InChargePercent) {
    static UFunction* Func = GetFunction("FortProjectileBase", "GetDefaultSpeed");
    struct {
        float InChargePercent;
        float ReturnValue;
    } params_GetDefaultSpeed{};

    params_GetDefaultSpeed.InChargePercent = InChargePercent;

    if (this && Func)
        ProcessEvent(Func, &params_GetDefaultSpeed);

    return params_GetDefaultSpeed.ReturnValue;
}

} // namespace SDK
