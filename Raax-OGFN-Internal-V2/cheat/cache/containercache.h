#pragma once
#include <cheat/sdk/sdk.h>
#include <unordered_map>

namespace Cache::Container
{
    enum class ContainerType
    {
        Chest = 0,
        AmmoBox
    };

    struct ContainerInfo
    {
        SDK::ABuildingContainer* Container = nullptr;
        ContainerType Type = ContainerType::Chest;
        SDK::FVector RootLocation;
        SDK::FVector2D RootScreenLocation;

        bool WasSeenThisFrame = false;
    };

    const std::unordered_map<void*, ContainerInfo>& GetCachedContainers();
    void UpdateCache();
}
