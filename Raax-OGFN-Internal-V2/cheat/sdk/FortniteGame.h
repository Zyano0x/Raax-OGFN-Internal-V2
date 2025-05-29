#pragma once

#include "Engine.h"

namespace SDK {

// --- Classes & Structs ---------------------------------------------

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

// TODO - Make these not inherit from UObject, as that is wrong

struct FFortItemEntry : public UObject {
  public:
    STATICCLASS_DEFAULTOBJECT("FortItemEntry", FFortItemEntry);

  public:
    UPROPERTY(class UFortItemDefinition*, ItemDefinition);
};

struct FFortBaseWeaponStats : public UObject {
  public:
    STATICCLASS_DEFAULTOBJECT("FortBaseWeaponStats", FFortBaseWeaponStats);

  public:
    UPROPERTY(float, ReloadTime);
    UPROPERTY(int32_t, AmmoCostPerFire);
};

struct FFortMeleeWeaponStats : public FFortBaseWeaponStats {
  public:
    STATICCLASS_DEFAULTOBJECT("FortMeleeWeaponStats", FFortMeleeWeaponStats);

  public:
    UPROPERTY(float, SwingPlaySpeed);
};

struct FFortRangedWeaponStats : public FFortBaseWeaponStats {
  public:
    STATICCLASS_DEFAULTOBJECT("FortRangedWeaponStats", FFortRangedWeaponStats);

  public:
    UPROPERTY(float, Spread);
    UPROPERTY(float, SpreadDownsights);
    UPROPERTY(float, StandingStillSpreadMultiplier);
    UPROPERTY(float, AthenaCrouchingSpreadMultiplier);
    UPROPERTY(float, AthenaJumpingFallingSpreadMultiplier);
    UPROPERTY(float, AthenaSprintingSpreadMultiplier);
    UPROPERTY(float, MinSpeedForSpreadMultiplier);
    UPROPERTY(float, MaxSpeedForSpreadMultiplier);
    UPROPERTY(float, RecoilVert);
    UPROPERTY(float, RecoilVertScaleGamepad);
    UPROPERTY(float, RecoilHoriz);
    UPROPERTY(int32_t, BulletsPerCartridge);
    UPROPERTY(float, FiringRate);
};

class FZiplinePawnState : public UObject {
  public:
    STATICCLASS_DEFAULTOBJECT("ZiplinePawnState", FZiplinePawnState);

  public:
    UPROPERTY_BITFIELD(bIsZiplining);
};

class AFortPlayerController : public APlayerController {
  public:
    STATICCLASS_DEFAULTOBJECT("FortPlayerController", AFortPlayerController);

  public:
    static void (*pFire_Press)(void*);
    static void (*pFire_Release)(void*);

  public:
    void Fire_Press();
    void Fire_Release();
};

class AFortPawn : public ACharacter {
  public:
    STATICCLASS_DEFAULTOBJECT("FortPawn", AFortPawn);

  public:
    UPROPERTY(class AFortWeapon*, CurrentWeapon);
    UPROPERTY_BITFIELD(bIsDying);
    UPROPERTY_BITFIELD(bIsDBNO);
};

class AFortPlayerPawn : public AFortPawn {
  public:
    STATICCLASS_DEFAULTOBJECT("FortPlayerPawn", AFortPlayerPawn);

  public:
    UPROPERTY(FZiplinePawnState, ZiplineState);
};

class AFortPlayerPawnAthena : public AFortPlayerPawn {
  public:
    STATICCLASS_DEFAULTOBJECT("FortPlayerPawnAthena", AFortPlayerPawnAthena);

  public:
    UPROPERTY(float, ReviveFromDBNOTime);
};

class AFortPlayerState : public APlayerState {
  public:
    STATICCLASS_DEFAULTOBJECT("FortPlayerState", AFortPlayerState);

  public:
    UPROPERTY(FString, Platform);
};

class AFortPlayerStateAthena : public AFortPlayerState {
  public:
    STATICCLASS_DEFAULTOBJECT("FortPlayerStateAthena", AFortPlayerStateAthena);

  public:
    UPROPERTY(uint8_t, TeamIndex);
};

class ABuildingContainer : public AActor {
  public:
    STATICCLASS_DEFAULTOBJECT("BuildingContainer", ABuildingContainer);

  public:
    UPROPERTY_BITFIELD(bAlreadySearched);
};

class ATiered_Chest_Athena_C : public ABuildingContainer {
  public:
    STATICCLASS_DEFAULTOBJECT("Tiered_Chest_Athena_C", ATiered_Chest_Athena_C);
};

class ATiered_Ammo_Athena_C : public ABuildingContainer {
  public:
    STATICCLASS_DEFAULTOBJECT("Tiered_Ammo_Athena_C", ATiered_Ammo_Athena_C);
};

class AFortAthenaSupplyDrop : public AActor {
  public:
    STATICCLASS_DEFAULTOBJECT("FortAthenaSupplyDrop", AFortAthenaSupplyDrop);
};

class AAthenaSupplyDrop_C : public AFortAthenaSupplyDrop {
  public:
    STATICCLASS_DEFAULTOBJECT("AthenaSupplyDrop_C", AAthenaSupplyDrop_C);
};

class AAthenaSupplyDrop_Llama_C : public AFortAthenaSupplyDrop {
  public:
    STATICCLASS_DEFAULTOBJECT("AthenaSupplyDrop_Llama_C", AAthenaSupplyDrop_Llama_C);
};

class AFortPickup : public AActor {
  public:
    STATICCLASS_DEFAULTOBJECT("FortPickup", AFortPickup);

  public:
    UPROPERTY(struct FFortItemEntry, PrimaryPickupItemEntry);
};

class AFortWeapon : public AActor {
  public:
    STATICCLASS_DEFAULTOBJECT("FortWeapon", AFortWeapon);

  public:
    static inline int GetWeaponStats_Idx;

  public:
    UPROPERTY(class UFortWeaponItemDefinition*, WeaponData);
    UPROPERTY(int32_t, AmmoCount);
    UPROPERTY(float, LastFireTime);

  public:
    int32_t GetBulletsPerClip();

  public:
    FFortBaseWeaponStats* GetWeaponStats();
};

class AFortWeaponRanged : public AFortWeapon {
  public:
    STATICCLASS_DEFAULTOBJECT("FortWeaponRanged", AFortWeaponRanged);
};

class AB_Melee_Generic_C : public AFortWeapon {
  public:
    STATICCLASS_DEFAULTOBJECT("B_Melee_Generic_C", AB_Melee_Generic_C);
};

class AFortProjectileBase : public AActor {
  public:
    STATICCLASS_DEFAULTOBJECT("FortProjectileBase", AFortProjectileBase);

  public:
    UPROPERTY(float, GravityScale);

  public:
    float GetDefaultGravityScale();
    float GetDefaultSpeed(float InChargePercent);
};

class UFortItemDefinition : public UObject {
  public:
    STATICCLASS_DEFAULTOBJECT("FortItemDefinition", UFortItemDefinition);

  public:
    UPROPERTY(FText, DisplayName);
    UPROPERTY(EFortItemTier, Tier);
};

enum class EFortWeaponTriggerType : uint8_t {
    OnPress = 0,
    Automatic = 1,
    OnRelease = 2,
    OnPressAndRelease = 3,
    EFortWeaponTriggerType_MAX = 4,
};

class UFortWeaponItemDefinition : public UFortItemDefinition {
  public:
    STATICCLASS_DEFAULTOBJECT("FortWeaponItemDefinition", UFortWeaponItemDefinition);

  public:
    UPROPERTY(TSoftObjectPtr<UFortWeaponItemDefinition>, AmmoData);
    UPROPERTY(EFortWeaponTriggerType, TriggerType);
};

class UFortWeaponRangedItemDefinition : public UFortWeaponItemDefinition {
  public:
    STATICCLASS_DEFAULTOBJECT("FortWeaponRangedItemDefinition", UFortWeaponRangedItemDefinition);

  public:
    UPROPERTY(TSoftClassPtr<UClass>, ProjectileTemplate);
};

} // namespace SDK
