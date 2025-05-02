#pragma once

#include "Engine.h"

namespace SDK {

enum class EFortItemTier : uint8_t {
    No_Tier = 0,
    I = 1,
    II = 2,
    III = 3,
    IV = 4,
    V = 5,
    VI = 6,
    VII = 7,
    VIII = 8,
    IX = 9,
    X = 10,
    NumItemTierValues = 11,
    EFortItemTier_MAX = 12,
};

class UFortProjectileMovementComponent : public UObject {
  public:
    STATICCLASS_DEFAULTOBJECT("FortProjectileMovementComponent", UFortProjectileMovementComponent)
};

class AFortPawn : public ACharacter {
  public:
    class AFortWeapon* CurrentWeapon() {
        static PropertyInfo Prop = GetPropertyInfo("FortPawn", "CurrentWeapon");
        if (this && Prop.Found)
            return *(AFortWeapon**)((uintptr_t)this + Prop.Offset);
        return nullptr;
    }

  public:
    STATICCLASS_DEFAULTOBJECT("FortPawn", AFortPawn)
};

class ABuildingContainer : public AActor {
  public:
    bool bAlreadySearched() {
        static PropertyInfo Prop = GetPropertyInfo("BuildingContainer", "bAlreadySearched");
        if (this && Prop.Found) {
            if (Prop.ByteMask)
                return *(uint8_t*)((uintptr_t)this + Prop.Offset) & Prop.ByteMask;
            else
                return *(bool*)((uintptr_t)this + Prop.Offset);
        }
        return false;
    }

  public:
    STATICCLASS_DEFAULTOBJECT("BuildingContainer", ABuildingContainer);
};

class ATiered_Chest_Athena_C : public ABuildingContainer {
  public:
    STATICCLASS_DEFAULTOBJECT("Tiered_Chest_Athena_C", ATiered_Chest_Athena_C);
};

class ATiered_Ammo_Athena_C : public ABuildingContainer {
  public:
    STATICCLASS_DEFAULTOBJECT("Tiered_Ammo_Athena_C", ATiered_Ammo_Athena_C);
};

class AFortPickup : public AActor {
  public:
    class FFortItemEntry* PrimaryPickupItemEntry() {
        static PropertyInfo Prop = GetPropertyInfo("FortPickup", "PrimaryPickupItemEntry");
        if (this && Prop.Found)
            return (FFortItemEntry*)((uintptr_t)this + Prop.Offset);
        return nullptr;
    }

  public:
    STATICCLASS_DEFAULTOBJECT("FortPickup", AFortPickup);
};

class AFortWeapon : public AActor {
  public:
    uint8_t WeaponCoreAnimation() {
        static PropertyInfo Prop = GetPropertyInfo("FortWeapon", "WeaponCoreAnimation");
        if (this && Prop.Found)
            return *(uint8_t*)((uintptr_t)this + Prop.Offset);
        return 0;
    }

    class UFortWeaponItemDefinition* WeaponData() {
        static PropertyInfo Prop = GetPropertyInfo("FortWeapon", "WeaponData");
        if (this && Prop.Found)
            return *(UFortWeaponItemDefinition**)((uintptr_t)this + Prop.Offset);
        return nullptr;
    }

  public:
    STATICCLASS_DEFAULTOBJECT("FortWeapon", AFortWeapon);
};

class AFortProjectileBase : public AActor {
  public:
    float GravityScale() {
        static PropertyInfo Prop = GetPropertyInfo("FortProjectileBase", "GravityScale");
        if (this && Prop.Found)
            return *(float*)((uintptr_t)this + Prop.Offset);
        return 0.f;
    }

    class UFortProjectileMovementComponent* ProjectileMovementComponent() {
        static PropertyInfo Prop = GetPropertyInfo("FortProjectileBase", "ProjectileMovementComponent");
        if (this && Prop.Found)
            return *(class UFortProjectileMovementComponent**)((uintptr_t)this + Prop.Offset);
        return nullptr;
    }

    float GetDefaultGravityScale() {
        static UFunction* Func = GetFunction("FortProjectileBase", "GetDefaultGravityScale");
        struct {
            float ReturnValue;
        } params_GetDefaultGravityScale{};

        if (this && Func)
            ProcessEvent(Func, &params_GetDefaultGravityScale);

        return params_GetDefaultGravityScale.ReturnValue;
    }

    float GetDefaultSpeed(float InChargePercent) {
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

  public:
    STATICCLASS_DEFAULTOBJECT("FortProjectileBase", AFortProjectileBase);
};

class FFortItemEntry : public UObject {
  public:
    class UFortItemDefinition* ItemDefinition() {
        static PropertyInfo Prop = GetPropertyInfo("FortItemEntry", "ItemDefinition");
        if (this && Prop.Found)
            return *(UFortItemDefinition**)((uintptr_t)this + Prop.Offset);
        return nullptr;
    }

  public:
    STATICCLASS_DEFAULTOBJECT("FortItemEntry", FFortItemEntry);
};

class UFortItemDefinition : public UObject {
  public:
    FText* DisplayName() {
        static PropertyInfo Prop = GetPropertyInfo("FortItemDefinition", "DisplayName");
        if (this && Prop.Found)
            return (FText*)((uintptr_t)this + Prop.Offset);
        return nullptr;
    }

    EFortItemTier Tier() {
        static PropertyInfo Prop = GetPropertyInfo("FortItemDefinition", "Tier");
        if (this && Prop.Found)
            return *(EFortItemTier*)((uintptr_t)this + Prop.Offset);
        return EFortItemTier::No_Tier;
    }

  public:
    STATICCLASS_DEFAULTOBJECT("FortItemDefinition", UFortItemDefinition);
};

class UFortWeaponItemDefinition : public UFortItemDefinition {
  public:
    class UFortWeaponItemDefinition* AmmoData() {
        static PropertyInfo Prop = GetPropertyInfo("FortWeaponItemDefinition", "AmmoData");
        if (this && Prop.Found) {
            auto SoftObjectPtr = (TSoftObjectPtr<UFortWeaponItemDefinition>*)((uintptr_t)this + Prop.Offset);
            return SoftObjectPtr->Get();
        }
        return nullptr;
    }

  public:
    STATICCLASS_DEFAULTOBJECT("FortWeaponItemDefinition", UFortWeaponItemDefinition);
};

class UFortWeaponRangedItemDefinition : public UFortWeaponItemDefinition {
  public:
    class UClass* ProjectileTemplate() {
        static PropertyInfo Prop = GetPropertyInfo("FortWeaponRangedItemDefinition", "ProjectileTemplate");
        if (this && Prop.Found) {
            auto SoftClassPtr = (TSoftClassPtr<UClass>*)((uintptr_t)this + Prop.Offset);
            return SoftClassPtr->Get();
        }
        return nullptr;
    }

  public:
    STATICCLASS_DEFAULTOBJECT("FortWeaponRangedItemDefinition", UFortWeaponRangedItemDefinition);
};

} // namespace SDK
