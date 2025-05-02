#pragma once

#include <unordered_map>
#include <cheat/sdk/sdk.h>

namespace Cache {
namespace Container {

// --- Cache Structures ----------------------------------------------

enum class ContainerType { Chest, AmmoBox };

struct ContainerInfo {
    SDK::ABuildingContainer* Container = nullptr;
    SDK::FVector             RootWorldLocation;
    SDK::FVector2D           RootScreenLocation;
    ContainerType            Type = ContainerType::Chest;
    bool                     SeenThisFrame = false;
};

// --- Public Cache Functions ----------------------------------------

const std::unordered_map<void*, ContainerInfo>& GetCachedContainers();
void                                            UpdateCache();

} // namespace Container
} // namespace Cache
