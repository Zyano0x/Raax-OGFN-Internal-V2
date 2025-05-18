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

class AFortPlayerController : public APlayerController {
  public:
    static void (*pFire_Press)(void*);
    static void (*pFire_Release)(void*);

  public:
    void Fire_Press();
    void Fire_Release();

  public:
    STATICCLASS_DEFAULTOBJECT("FortPlayerController", AFortPlayerController)
};

class AFortPawn : public ACharacter {
  public:
    class AFortWeapon* CurrentWeapon();

  public:
    STATICCLASS_DEFAULTOBJECT("FortPawn", AFortPawn)
};

class AFortPlayerState : public APlayerState {
  public:
    FString Platform();

  public:
    STATICCLASS_DEFAULTOBJECT("FortPlayerState", AFortPlayerState)
};

class ABuildingContainer : public AActor {
  public:
    bool bAlreadySearched();

  public:
    STATICCLASS_DEFAULTOBJECT("BuildingContainer", ABuildingContainer)
};

class ATiered_Chest_Athena_C : public ABuildingContainer {
  public:
    STATICCLASS_DEFAULTOBJECT("Tiered_Chest_Athena_C", ATiered_Chest_Athena_C)
};

class ATiered_Ammo_Athena_C : public ABuildingContainer {
  public:
    STATICCLASS_DEFAULTOBJECT("Tiered_Ammo_Athena_C", ATiered_Ammo_Athena_C)
};

class AFortAthenaSupplyDrop : public AActor
{
  public:
    STATICCLASS_DEFAULTOBJECT("FortAthenaSupplyDrop", AFortAthenaSupplyDrop);
};

class AFortAthenaSupplyDropBalloon : public AActor
{
    public:
       STATICCLASS_DEFAULTOBJECT("FortAthenaSupplyDropBalloon", AFortAthenaSupplyDropBalloon);
};

class AAthenaSupplyDrop_Llama : public AActor
{
    public:
    STATICCLASS_DEFAULTOBJECT("AthenaSupplyDrop_Llama", AAthenaSupplyDrop_Llama);
};

class AFortPickup : public AActor {
  public:
    class FFortItemEntry* PrimaryPickupItemEntry();

  public:
    STATICCLASS_DEFAULTOBJECT("FortPickup", AFortPickup)
};

class AFortWeapon : public AActor {
  public:
    uint8_t                          WeaponCoreAnimation();
    class UFortWeaponItemDefinition* WeaponData();
    int32_t                          AmmoCount();
    int32_t                          GetBulletsPerClip();

  public:
    STATICCLASS_DEFAULTOBJECT("FortWeapon", AFortWeapon)
};

class AFortProjectileBase : public AActor {
  public:
    float GravityScale();
    float GetDefaultGravityScale();
    float GetDefaultSpeed(float InChargePercent);

  public:
    STATICCLASS_DEFAULTOBJECT("FortProjectileBase", AFortProjectileBase)
};

class FFortItemEntry : public UObject {
  public:
    class UFortItemDefinition* ItemDefinition();

  public:
    STATICCLASS_DEFAULTOBJECT("FortItemEntry", FFortItemEntry)
};

class UFortItemDefinition : public UObject {
  public:
    FText*        DisplayName();
    EFortItemTier Tier();

  public:
    STATICCLASS_DEFAULTOBJECT("FortItemDefinition", UFortItemDefinition)
};

class UFortWeaponItemDefinition : public UFortItemDefinition {
  public:
    UFortWeaponItemDefinition* AmmoData();

  public:
    STATICCLASS_DEFAULTOBJECT("FortWeaponItemDefinition", UFortWeaponItemDefinition)
};

class UFortWeaponRangedItemDefinition : public UFortWeaponItemDefinition {
  public:
    UClass* ProjectileTemplate();

  public:
    STATICCLASS_DEFAULTOBJECT("FortWeaponRangedItemDefinition", UFortWeaponRangedItemDefinition)
};

} // namespace SDK
