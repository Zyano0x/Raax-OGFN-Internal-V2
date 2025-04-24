#pragma once
#include <cheat/sdk/sdk.h>
#include <unordered_map>

namespace Cache::Pickup
{
    struct PickupInfo
    {
        SDK::AFortPickup* Pickup = nullptr;

        SDK::FVector RootLocation;
        SDK::FVector2D RootScreenLocation;
        SDK::EFortItemTier Tier;
        std::string WeaponName;

        bool WasSeenThisFrame = false;
    };

    const std::unordered_map<void*, PickupInfo>& GetCachedPickups();
    void UpdateCache();
}
