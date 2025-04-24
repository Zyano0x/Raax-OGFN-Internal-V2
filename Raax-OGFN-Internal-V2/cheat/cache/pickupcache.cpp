#include "pickupcache.h"
#include <chrono>

std::unordered_map<void*, Cache::Pickup::PickupInfo> g_CachedPickups;

const std::unordered_map<void*, Cache::Pickup::PickupInfo>& Cache::Pickup::GetCachedPickups() {
    return g_CachedPickups;
}


void ResetPickupSeenFlags() {
    for (auto& [Pawn, Cache] : g_CachedPickups) {
        Cache.WasSeenThisFrame = false;
    }
}

Cache::Pickup::PickupInfo CreateNewPickupInfo(SDK::AFortPickup* Pickup) {
    Cache::Pickup::PickupInfo Info;
    Info.Pickup = Pickup;
    Info.RootLocation = Pickup->RootComponent()->RelativeLocation();
    Info.RootScreenLocation = SDK::Project(Info.RootLocation);

    SDK::FFortItemEntry* FortItemEntry = Pickup->PrimaryPickupItemEntry();
    if (!FortItemEntry)
        return {};

    SDK::UFortItemDefinition* FortItemDefinition = FortItemEntry->ItemDefinition();
    if (!FortItemDefinition)
        return {};

    Info.WeaponName = FortItemDefinition->DisplayName()->ToString();
    Info.Tier = FortItemDefinition->Tier();

    Info.WasSeenThisFrame = true;
    return Info;
}

void UpdateExistingPickupInfo(Cache::Pickup::PickupInfo& Info, SDK::AFortPickup* Pickup) {
    Info.RootLocation = Pickup->RootComponent()->RelativeLocation();
    Info.RootScreenLocation = SDK::Project(Info.RootLocation);
    Info.WasSeenThisFrame = true;
}

void RemoveUnseenPickups() {
    for (auto it = g_CachedPickups.begin(); it != g_CachedPickups.end(); ) {
        if (!it->second.WasSeenThisFrame)
            it = g_CachedPickups.erase(it);
        else
            ++it;
    }
}


void Cache::Pickup::UpdateCache() {
    ResetPickupSeenFlags();
    static std::vector<SDK::AFortPickup*> Pickups;
    SDK::GetAllActorsOfClassAllLevels<SDK::AFortPickup>(Pickups);
    for (const auto& Pickup : Pickups) {
        auto it = g_CachedPickups.find(Pickup);
        if (it == g_CachedPickups.end())
            g_CachedPickups[Pickup] = CreateNewPickupInfo(Pickup);
        else
            UpdateExistingPickupInfo(it->second, Pickup);
    }
    RemoveUnseenPickups();
}
