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

AFortWeapon* AFortPawn::CurrentWeapon() {
    static PropertyInfo Prop = GetPropertyInfo("FortPawn", "CurrentWeapon");
    if (this && Prop.Found)
        return *(AFortWeapon**)((uintptr_t)this + Prop.Offset);
    return nullptr;
}

void AFortPlayerPawnAthena::Set_ReviveFromDBNOTime(float Value) {
    static PropertyInfo Prop = GetPropertyInfo("FortPlayerPawnAthena", "ReviveFromDBNOTime");
    if (this && Prop.Found)
        *(float*)((uintptr_t)this + Prop.Offset) = Value;
    return;
}

FString AFortPlayerState::Platform() {
    static PropertyInfo Prop = GetPropertyInfo("FortPlayerState", "Platform");
    if (this && Prop.Found)
        return *(FString*)((uintptr_t)this + Prop.Offset);
    return {};
}

bool ABuildingContainer::bAlreadySearched() {
    static PropertyInfo Prop = GetPropertyInfo("BuildingContainer", "bAlreadySearched");
    if (this && Prop.Found) {
        if (Prop.ByteMask)
            return *(uint8_t*)((uintptr_t)this + Prop.Offset) & Prop.ByteMask;
        else
            return *(bool*)((uintptr_t)this + Prop.Offset);
    }
    return false;
}

FFortItemEntry* AFortPickup::PrimaryPickupItemEntry() {
    static PropertyInfo Prop = GetPropertyInfo("FortPickup", "PrimaryPickupItemEntry");
    if (this && Prop.Found)
        return (FFortItemEntry*)((uintptr_t)this + Prop.Offset);
    return nullptr;
}

uint8_t AFortWeapon::WeaponCoreAnimation() {
    static PropertyInfo Prop = GetPropertyInfo("FortWeapon", "WeaponCoreAnimation");
    if (this && Prop.Found)
        return *(uint8_t*)((uintptr_t)this + Prop.Offset);
    return 0;
}
UFortWeaponItemDefinition* AFortWeapon::WeaponData() {
    static PropertyInfo Prop = GetPropertyInfo("FortWeapon", "WeaponData");
    if (this && Prop.Found)
        return *(UFortWeaponItemDefinition**)((uintptr_t)this + Prop.Offset);
    return nullptr;
}
int32_t AFortWeapon::AmmoCount() {
    static PropertyInfo Prop = GetPropertyInfo("FortWeapon", "AmmoCount");
    if (this && Prop.Found)
        return *(int32_t*)((uintptr_t)this + Prop.Offset);
    return 0;
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

float AFortWeapon::LastFireTime() {
    static PropertyInfo Prop = GetPropertyInfo("FortWeapon", "LastFireTime");

    if (this && Prop.Found)
        return *(float*)(uint64_t(this) + Prop.Offset);

    return 0.f;
}

float AFortWeapon::LastFireTimeVerified() {
    static PropertyInfo Prop = GetPropertyInfo("FortWeapon", "LastFireTimeVerified");

    if (this && Prop.Found)
        return *(float*)(uint64_t(this) + Prop.Offset);
    return 0.f;
}

void AFortWeapon::Set_LastFireTime(float Value) {
    static PropertyInfo Prop = GetPropertyInfo("FortWeapon", "LastFireTime");

    if (this && Prop.Found)
        (*(float*)(uint64_t(this) + Prop.Offset) = Value);

    return;
}

void AFortWeapon::Set_CustomTimeDilation(float Value) {
    static PropertyInfo Prop = GetPropertyInfo("Actor", "CustomTimeDilation");

    if (this && Prop.Found)
        (*(float*)(uint64_t(this) + Prop.Offset) = Value);

    return;
}

float AFortProjectileBase::GravityScale() {
    static PropertyInfo Prop = GetPropertyInfo("FortProjectileBase", "GravityScale");
    if (this && Prop.Found)
        return *(float*)((uintptr_t)this + Prop.Offset);
    return 0.f;
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

UFortItemDefinition* FFortItemEntry::ItemDefinition() {
    static PropertyInfo Prop = GetPropertyInfo("FortItemEntry", "ItemDefinition");
    if (this && Prop.Found)
        return *(UFortItemDefinition**)((uintptr_t)this + Prop.Offset);
    return nullptr;
}

FText* UFortItemDefinition::DisplayName() {
    static PropertyInfo Prop = GetPropertyInfo("FortItemDefinition", "DisplayName");
    if (this && Prop.Found)
        return (FText*)((uintptr_t)this + Prop.Offset);
    return nullptr;
}
EFortItemTier UFortItemDefinition::Tier() {
    static PropertyInfo Prop = GetPropertyInfo("FortItemDefinition", "Tier");
    if (this && Prop.Found)
        return *(EFortItemTier*)((uintptr_t)this + Prop.Offset);
    return EFortItemTier::No_Tier;
}

UFortWeaponItemDefinition* UFortWeaponItemDefinition::AmmoData() {
    static PropertyInfo Prop = GetPropertyInfo("FortWeaponItemDefinition", "AmmoData");
    if (this && Prop.Found) {
        auto SoftObjectPtr = (TSoftObjectPtr<UFortWeaponItemDefinition>*)((uintptr_t)this + Prop.Offset);
        return SoftObjectPtr->Get();
    }
    return nullptr;
}

void UFortWeaponItemDefinition::SetTriggerType(EFortWeaponTriggerType TriggerType) {
    static PropertyInfo Prop = GetPropertyInfo("FortWeaponItemDefinition", "TriggerType");
    if (this && Prop.Found) {
        *(EFortWeaponTriggerType*)(uint64_t(this) + Prop.Offset) = TriggerType;
    }
    return;
}
UClass* UFortWeaponRangedItemDefinition::ProjectileTemplate() {
    static PropertyInfo Prop = GetPropertyInfo("FortWeaponRangedItemDefinition", "ProjectileTemplate");
    if (this && Prop.Found) {
        auto SoftClassPtr = (TSoftClassPtr<UClass>*)((uintptr_t)this + Prop.Offset);
        return SoftClassPtr->Get();
    }
    return nullptr;
}

} // namespace SDK
