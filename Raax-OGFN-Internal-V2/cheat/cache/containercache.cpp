#include "containercache.h"
#include <config/config.h>
#include <chrono>

namespace Cache {
namespace Container {

// --- Cache Data ----------------------------------------------------

std::unordered_map<void*, ContainerInfo>     CachedContainers;
std::unordered_map<void*, GameplayActorInfo> CachedGameplayActors;

// --- Cache Utility Functions ---------------------------------------

ContainerInfo CreateNewContainerInfo(SDK::ABuildingContainer* Container, ContainerType Type) {
    ContainerInfo Info;
    Info.Container = Container;
    Info.RootWorldLocation = Container->RootComponent()->RelativeLocation();
    Info.RootScreenLocation = SDK::Project(Info.RootWorldLocation);
    Info.Type = Type;
    Info.SeenThisFrame = true;
    return Info;
}

GameplayActorInfo CreateNewGameplayActorInfo(SDK::AActor* GameplayActor, GameplayActorType Type) {
    GameplayActorInfo Info;
    Info.GameplayActor = GameplayActor;
    Info.RootWorldLocation = GameplayActor->RootComponent()->RelativeLocation();
    Info.RootScreenLocation = SDK::Project(Info.RootWorldLocation);
    Info.Type = Type;
    Info.SeenThisFrame = true;
    return Info;
}

void UpdateExistingContainerInfo(ContainerInfo& Info, SDK::ABuildingContainer* Container) {
    Info.RootWorldLocation = Container->RootComponent()->RelativeLocation();
    Info.RootScreenLocation = SDK::Project(Info.RootWorldLocation);
    Info.SeenThisFrame = true;
}

void UpdateExistingGameplayActorInfo(GameplayActorInfo& Info, SDK::AActor* GameplayActor) {
    Info.RootWorldLocation = GameplayActor->RootComponent()->RelativeLocation();
    Info.RootScreenLocation = SDK::Project(Info.RootWorldLocation);
    Info.SeenThisFrame = true;
}

void ResetContainerSeenFlags() {
    for (auto& [_, Cache] : CachedContainers) {
        Cache.SeenThisFrame = false;
    }
}

void ResetGameplayActorSeenFlags() {
    for (auto& [_, Cache] : CachedGameplayActors) {
        Cache.SeenThisFrame = false;
    }
}

void RemoveUnseenContainers() {
    std::erase_if(CachedContainers, [](const auto& Cache) { return !Cache.second.SeenThisFrame; });
}

void RemoveUnseenGameplayActors() {
    std::erase_if(CachedGameplayActors, [](const auto& Cache) { return !Cache.second.SeenThisFrame; });
}

template <typename T> void CacheContainersOfType(std::vector<T*>& ContainerList, ContainerType Type) {
    SDK::GetAllActorsOfClassAllLevels<T>(ContainerList);
    for (const auto& Container : ContainerList) {
        auto It = CachedContainers.find(Container);
        if (It == CachedContainers.end()) {
            CachedContainers[Container] = CreateNewContainerInfo(Container, Type);
        } else {
            UpdateExistingContainerInfo(It->second, Container);
        }
    }
}

template <typename T> void CacheGameplayActorsOfType(std::vector<T*>& GameplayActorList, GameplayActorType Type) {
    SDK::GetAllActorsOfClassAllLevels<T>(GameplayActorList);
    for (const auto& GameplayActor : GameplayActorList) {
        auto It = CachedGameplayActors.find(GameplayActor);
        if (It == CachedGameplayActors.end()) {
            CachedGameplayActors[GameplayActor] = CreateNewGameplayActorInfo(GameplayActor, Type);
        } else {
            UpdateExistingGameplayActorInfo(It->second, GameplayActor);
        }
    }
}

template <typename T> bool CheckIfStaticClassIsLoaded(float CheckDelayS) {
    static bool Found = false;
    if (Found)
        return true;

    static std::chrono::time_point<std::chrono::high_resolution_clock> LastCheckTime;
    auto                                                               Now = std::chrono::steady_clock::now();
    auto Elapsed = std::chrono::duration_cast<std::chrono::seconds>(Now - LastCheckTime).count();
    if (Elapsed < CheckDelayS)
        return false;

    LastCheckTime = Now;

    if (T::StaticClass() && T::StaticClass()) {
        Found = true;
        return true;
    }

    return false;
}

// --- Public Cache Functions ----------------------------------------

const std::unordered_map<void*, ContainerInfo>& GetCachedContainers() {
    return CachedContainers;
}

const std::unordered_map<void*, GameplayActorInfo>& GetCachedGameplayActors() {
    return CachedGameplayActors;
}

void UpdateCache() {
    ResetContainerSeenFlags();
    ResetGameplayActorSeenFlags();

    // For some reason, the ammo box class is loaded once you get in game (tested on 14.40)
    // To fix performance issues we only check if the class is loaded every 5 seconds
    if (!CheckIfStaticClassIsLoaded<SDK::ATiered_Chest_Athena_C>(5.f) ||
        !CheckIfStaticClassIsLoaded<SDK::ATiered_Ammo_Athena_C>(5.f) ||
        !CheckIfStaticClassIsLoaded<SDK::AAthenaSupplyDrop_C>(5.f) ||
        !CheckIfStaticClassIsLoaded<SDK::AAthenaSupplyDrop_Llama_C>(5.f)) {
        return;
    }

    const auto& Config = Config::g_Config.Visuals.Loot;
    if (Config.ChestText) {
        static std::vector<SDK::ATiered_Chest_Athena_C*> ChestList;
        CacheContainersOfType<SDK::ATiered_Chest_Athena_C>(ChestList, ContainerType::Chest);
    }

    if (Config.AmmoBoxText) {
        static std::vector<SDK::ATiered_Ammo_Athena_C*> AmmoBoxList;
        CacheContainersOfType<SDK::ATiered_Ammo_Athena_C>(AmmoBoxList, ContainerType::AmmoBox);
    }

    if (Config.SupplyDropText) {
        static std::vector<SDK::AAthenaSupplyDrop_C*> SupplyDropList;
        CacheGameplayActorsOfType<SDK::AAthenaSupplyDrop_C>(SupplyDropList, GameplayActorType::SupplyDrop);
    }

    if (Config.LlamaText) {
        static std::vector<SDK::AAthenaSupplyDrop_Llama_C*> LlamaList;
        CacheGameplayActorsOfType<SDK::AAthenaSupplyDrop_Llama_C>(LlamaList, GameplayActorType::Llama);
    }

    RemoveUnseenContainers();
    RemoveUnseenGameplayActors();
}

} // namespace Container
} // namespace Cache
