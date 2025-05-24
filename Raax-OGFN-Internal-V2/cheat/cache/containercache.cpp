#include "containercache.h"

#include <optional>
#include <chrono>

#include <config/config.h>

namespace Cache {
namespace Container {

// --- Cache Data ----------------------------------------------------

std::unordered_map<void*, ContainerInfo>     CachedContainers;
std::unordered_map<void*, GameplayActorInfo> CachedGameplayActors;

// --- Cache Utility Functions ---------------------------------------

template <typename InfoClass, typename ActorClass, typename TypeEnum>
std::optional<InfoClass> CreateNewInfo(ActorClass Actor, TypeEnum Type) {
    InfoClass Info;
    Info.Actor = Actor;
    Info.RootComponent = Info.Actor->RootComponent;
    if (!Info.RootComponent)
        return std::nullopt;

    Info.RootWorldLocation = Info.RootComponent->RelativeLocation;
    Info.RootScreenLocation = SDK::Project(Info.RootWorldLocation);

    Info.Type = Type;

    Info.SeenThisFrame = true;
    return Info;
}

template <typename InfoClass> bool UpdateExistingInfo(InfoClass& Info) {
    Info.RootComponent = Info.Actor->RootComponent;
    if (!Info.RootComponent)
        return false;

    Info.RootWorldLocation = Info.RootComponent->RelativeLocation;
    Info.RootScreenLocation = SDK::Project(Info.RootWorldLocation);

    Info.SeenThisFrame = true;
    return true;
}

template <typename InfoMap> void ResetSeenFlags(InfoMap& CacheMap) {
    for (auto& [_, Info] : CacheMap) {
        Info.SeenThisFrame = false;
    }
}

template <typename InfoMap> void RemoveUnseen(InfoMap& CacheMap) {
    std::erase_if(CacheMap, [](const auto& Entry) { return !Entry.second.SeenThisFrame; });
}

template <typename T, typename InfoMap, typename InfoClass, typename ActorPtr, typename EnumType>
void CacheActorsOfType(std::vector<T*>& ActorList, InfoMap& CacheMap, EnumType Type) {
    SDK::GetAllActorsOfClassAllLevels<T>(ActorList);
    for (const auto& Actor : ActorList) {
        if (!CacheMap.contains(Actor)) {
            auto Info = CreateNewInfo<InfoClass, ActorPtr, EnumType>(Actor, Type);
            if (Info.has_value())
                CacheMap[Actor] = Info.value();
        } else {
            auto& Info = CacheMap.at(Actor);
            if (!UpdateExistingInfo(Info))
                CacheMap.erase(Actor);
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
    ResetSeenFlags(CachedContainers);
    ResetSeenFlags(CachedGameplayActors);

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
        CacheActorsOfType<SDK::ATiered_Chest_Athena_C, decltype(CachedContainers), ContainerInfo,
                          SDK::ABuildingContainer*, ContainerType>(ChestList, CachedContainers, ContainerType::Chest);
    }

    if (Config.AmmoBoxText) {
        static std::vector<SDK::ATiered_Ammo_Athena_C*> AmmoBoxList;
        CacheActorsOfType<SDK::ATiered_Ammo_Athena_C, decltype(CachedContainers), ContainerInfo,
                          SDK::ABuildingContainer*, ContainerType>(AmmoBoxList, CachedContainers,
                                                                   ContainerType::AmmoBox);
    }

    if (Config.SupplyDropText) {
        static std::vector<SDK::AAthenaSupplyDrop_C*> SupplyDropList;
        CacheActorsOfType<SDK::AAthenaSupplyDrop_C, decltype(CachedGameplayActors), GameplayActorInfo, SDK::AActor*,
                          GameplayActorType>(SupplyDropList, CachedGameplayActors, GameplayActorType::SupplyDrop);
    }

    if (Config.LlamaText) {
        static std::vector<SDK::AAthenaSupplyDrop_Llama_C*> LlamaList;
        CacheActorsOfType<SDK::AAthenaSupplyDrop_Llama_C, decltype(CachedGameplayActors), GameplayActorInfo,
                          SDK::AActor*, GameplayActorType>(LlamaList, CachedGameplayActors, GameplayActorType::Llama);
    }

    RemoveUnseen(CachedContainers);
    RemoveUnseen(CachedGameplayActors);
}

} // namespace Container
} // namespace Cache
