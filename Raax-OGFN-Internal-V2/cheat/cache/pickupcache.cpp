#include "pickupcache.h"
#include <chrono>
#include <config/config.h>

namespace Cache {
namespace Pickup {

// --- Cache Data ----------------------------------------------------

std::unordered_map<void*, PickupInfo> CachedPickups;

// --- Cache Utility Functions ---------------------------------------

PickupInfo CreateNewPickupInfo(SDK::AFortPickup* Pickup) {
    PickupInfo Info;
    Info.Pickup = Pickup;
    Info.RootWorldLocation = Pickup->RootComponent()->RelativeLocation();
    Info.RootScreenLocation = SDK::Project(Info.RootWorldLocation);

    SDK::FFortItemEntry ItemEntry = Pickup->PrimaryPickupItemEntry;

    SDK::UFortItemDefinition* ItemDefinition = ItemEntry.ItemDefinition;
    if (!ItemDefinition)
        return {};

    Info.WeaponName = ItemDefinition->DisplayName.ToString();
    Info.Tier = ItemDefinition->Tier;
    Info.SeenThisFrame = true;
    return Info;
}

void UpdateExistingPickupInfo(PickupInfo& Info, SDK::AFortPickup* Pickup) {
    Info.RootWorldLocation = Pickup->RootComponent()->RelativeLocation();
    Info.RootScreenLocation = SDK::Project(Info.RootWorldLocation);
    Info.SeenThisFrame = true;
}

void ResetPickupSeenFlags() {
    for (auto& [_, Cache] : CachedPickups) {
        Cache.SeenThisFrame = false;
    }
}

void RemoveUnseenPickups() {
    std::erase_if(CachedPickups, [](const auto& Cache) {
        return !Cache.second.SeenThisFrame;
        });
}

// --- Public Cache Functions ----------------------------------------

const std::unordered_map<void*, PickupInfo>& GetCachedPickups() {
    return CachedPickups;
}

void UpdateCache() {
    ResetPickupSeenFlags();

    const auto& Config = Config::g_Config.Visuals.Loot;
    if (Config.LootText) {
        static std::vector<SDK::AFortPickup*> PickupList;
        SDK::GetAllActorsOfClass<SDK::AFortPickup>(PickupList);
        for (const auto& Pickup : PickupList) {
            auto It = CachedPickups.find(Pickup);
            if (It == CachedPickups.end()) {
                CachedPickups[Pickup] = CreateNewPickupInfo(Pickup);
            } else {
                UpdateExistingPickupInfo(It->second, Pickup);
            }
        }
    }

    RemoveUnseenPickups();
}

} // namespace Pickup
} // namespace Cache
