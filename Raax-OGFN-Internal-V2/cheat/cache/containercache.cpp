#include "containercache.h"
#include <chrono>

std::unordered_map<void*, Cache::Container::ContainerInfo> g_CachedContainers;

const std::unordered_map<void*, Cache::Container::ContainerInfo>& Cache::Container::GetCachedContainers() {
    return g_CachedContainers;
}


void ResetContainerSeenFlags() {
    for (auto& [Pawn, Cache] : g_CachedContainers) {
        Cache.WasSeenThisFrame = false;
    }
}

Cache::Container::ContainerInfo CreateNewContainerInfo(SDK::ABuildingContainer* Container, Cache::Container::ContainerType Type) {
    Cache::Container::ContainerInfo Info;
    Info.Container = Container;
    Info.Type = Type;
    Info.RootLocation = Container->RootComponent()->RelativeLocation();
    Info.RootScreenLocation = SDK::Project(Info.RootLocation);
    Info.WasSeenThisFrame = true;
    return Info;
}

void UpdateExistingContainerInfo(Cache::Container::ContainerInfo& Info, SDK::ABuildingContainer* Container) {
    Info.RootLocation = Container->RootComponent()->RelativeLocation();
    Info.RootScreenLocation = SDK::Project(Info.RootLocation);
    Info.WasSeenThisFrame = true;
}

void RemoveUnseenContainers() {
    for (auto it = g_CachedContainers.begin(); it != g_CachedContainers.end(); ) {
        if (!it->second.WasSeenThisFrame)
            it = g_CachedContainers.erase(it);
        else
            ++it;
    }
}


void Cache::Container::UpdateCache() {
    // For some reason the ammo box class is loaded once you get in game (14.40)
    // so i just put them both in a check for safety and performance

    ResetContainerSeenFlags();
    if (SDK::ATiered_Chest_Athena_C::StaticClass()) {
        static std::vector<SDK::ATiered_Chest_Athena_C*> Chests;
        SDK::GetAllActorsOfClassAllLevels<SDK::ATiered_Chest_Athena_C>(Chests);
        for (const auto& Container : Chests) {
            auto it = g_CachedContainers.find(Container);
            if (it == g_CachedContainers.end())
                g_CachedContainers[Container] = CreateNewContainerInfo(Container, ContainerType::Chest);
            else
                UpdateExistingContainerInfo(it->second, Container);
        }
    }

    if (SDK::ATiered_Ammo_Athena_C::StaticClass()) {
        static std::vector<SDK::ATiered_Ammo_Athena_C*> AmmoBoxes;
        SDK::GetAllActorsOfClassAllLevels<SDK::ATiered_Ammo_Athena_C>(AmmoBoxes);
        for (const auto& Container : AmmoBoxes) {
            auto it = g_CachedContainers.find(Container);
            if (it == g_CachedContainers.end())
                g_CachedContainers[Container] = CreateNewContainerInfo(Container, ContainerType::AmmoBox);
            else
                UpdateExistingContainerInfo(it->second, Container);
        }
    }
    RemoveUnseenContainers();
}
