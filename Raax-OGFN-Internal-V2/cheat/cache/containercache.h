#pragma once

#include <unordered_map>
#include <cheat/sdk/sdk.h>

namespace Cache {
namespace Container {

// --- Cache Structures ----------------------------------------------

enum class ContainerType { Chest, AmmoBox };

struct ContainerInfo {
    SDK::ABuildingContainer* Actor = nullptr;
    SDK::USceneComponent*    RootComponent = nullptr;

    SDK::FVector   RootWorldLocation;
    SDK::FVector2D RootScreenLocation;

    ContainerType Type = ContainerType::Chest;

    bool SeenThisFrame = false;
};

enum class GameplayActorType { Llama, SupplyDrop };

struct GameplayActorInfo {
    SDK::AActor*          Actor = nullptr;
    SDK::USceneComponent* RootComponent = nullptr;

    SDK::FVector   RootWorldLocation;
    SDK::FVector2D RootScreenLocation;

    GameplayActorType Type = GameplayActorType::SupplyDrop;

    bool SeenThisFrame = false;
};

// --- Public Cache Functions ----------------------------------------

const std::unordered_map<void*, ContainerInfo>&     GetCachedContainers();
const std::unordered_map<void*, GameplayActorInfo>& GetCachedGameplayActors();

void UpdateCache();

} // namespace Container
} // namespace Cache
