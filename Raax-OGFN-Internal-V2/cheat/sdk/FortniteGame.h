#pragma once
#include "Engine.h"

namespace SDK
{
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


    class AFortPawn : public ACharacter
    {
    public:
        STATICCLASS_DEFAULTOBJECT("FortPawn", AFortPawn);
    };

    class ABuildingContainer : public AActor
    {
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

    class ATiered_Chest_Athena_C : public ABuildingContainer
    {
    public:
        STATICCLASS_DEFAULTOBJECT("Tiered_Chest_Athena_C", ATiered_Chest_Athena_C);
    };

    class ATiered_Ammo_Athena_C : public ABuildingContainer
    {
    public:
        STATICCLASS_DEFAULTOBJECT("Tiered_Ammo_Athena_C", ATiered_Ammo_Athena_C);
    };

    class AFortPickup : public AActor
    {
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

    class FFortItemEntry : public UObject
    {
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

    class UFortItemDefinition : public UObject
    {
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
}
