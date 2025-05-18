#include "containertick.h"
#include <format>

#include <cheat/cache/containercache.h>
#include <cheat/core.h>
#include <drawing/drawing.h>
#include <config/config.h>

namespace Tick {
namespace Container {

// --- Container Utility Functions -----------------------------------

void RenderContainer(const Cache::Container::ContainerInfo& Info, const std::string& FormatStr,
                     const SDK::FLinearColor& Color, float DistM, float MaxDist, float FadeStart) {
    if (DistM <= MaxDist) {
        float Alpha = 1.0f;
        if (DistM > FadeStart)
            Alpha = 1.0f - ((DistM - FadeStart) / (MaxDist - FadeStart));

        SDK::FLinearColor OutlineColor = SDK::FLinearColor::Black;
        OutlineColor.A = Alpha;

        char Buffer[64];
        snprintf(Buffer, sizeof(Buffer), FormatStr.c_str(), static_cast<int>(DistM));
        Drawing::Text(Buffer, Info.RootScreenLocation, SDK::FLinearColor(Color.R, Color.G, Color.B, Alpha), 12.f, true,
                      true, true, 1.f, OutlineColor);
    }
}

void RenderGameplayActor(const Cache::Container::GameplayActorInfo& Info, const std::string& FormatStr, const SDK::FLinearColor& Color, float DistM, float MaxDist, float FadeStart)
{
    if (DistM <= MaxDist) {
        float Alpha = 1.0f;
        if (DistM > FadeStart)
            Alpha = 1.0f - ((DistM - FadeStart) / (MaxDist - FadeStart));

        SDK::FLinearColor OutlineColor = SDK::FLinearColor::Black;
        OutlineColor.A = Alpha;

        char Buffer[64];
        snprintf(Buffer, sizeof(Buffer), FormatStr.c_str(), static_cast<int>(DistM));
        Drawing::Text(Buffer, Info.RootScreenLocation, SDK::FLinearColor(Color.R, Color.G, Color.B, Alpha), 12.f, true,
                      true, true, 1.f, OutlineColor);
    }
}

// --- Public Tick Functions -----------------------------------------

void TickGameThread() {
    Cache::Container::UpdateCache();
}
void TickRenderThread() {
    const auto& LootConfig = Config::g_Config.Visuals.Loot;
    bool        ChestText = LootConfig.ChestText;
    bool        AmmoBoxText = LootConfig.AmmoBoxText;
    bool        SupplyDropText = LootConfig.SupplyDropText;
    if (!ChestText && !AmmoBoxText)
        return;

    float MaxDistChest = LootConfig.ChestMaxDistance;
    float MaxDistAmmoBox = LootConfig.AmmoBoxMaxDistance;
    float MaxDistSupplyDrop = LootConfig.SupplyDropMaxDistance;
    float FadeStartChest = MaxDistChest * 0.2f;
    float FadeStartAmmoBox = MaxDistAmmoBox * 0.2f;
    float FadeStartSupplyDrop = MaxDistSupplyDrop * 0.2;

    for (const auto& [_, Info] : Cache::Container::GetCachedContainers()) {
        if (Info.Container->bAlreadySearched())
            continue;

        float DistM = Info.RootWorldLocation.Dist(Core::g_CameraLocation) / 100.f;
        if (Info.Type == Cache::Container::ContainerType::Chest && ChestText) {
            RenderContainer(Info, "Chest [%d m]", SDK::FLinearColor(0.95f, 0.68f, 0.1f, 1.f), DistM, MaxDistChest,
                            FadeStartChest);
        } else if (Info.Type == Cache::Container::ContainerType::AmmoBox && AmmoBoxText) {
            RenderContainer(Info, "Ammo Box [%d m]", SDK::FLinearColor(0.1f, 0.6f, 0.f, 1.f), DistM, MaxDistAmmoBox,
                            FadeStartAmmoBox);
        }
    }
    for (const auto& [_, Info] : Cache::Container::GetCachedGameplayActors())
    {
        float DistM = Info.RootWorldLocation.Dist(Core::g_CameraLocation / 100.f);
        if (Info.Type == Cache::Container::GameplayActorType::SupplyDrop && SupplyDropText) {
            RenderGameplayActor(Info, "SupplyDrop [%d m]", SDK::FLinearColor(0.95, 0.68, 0.1f, 1.f), DistM,
                                MaxDistAmmoBox, FadeStartSupplyDrop);
        }
        else if (Info.Type == Cache::Container::GameplayActorType::Llama && SupplyDropText) {
            RenderGameplayActor(Info, "Llama [%d m]", SDK::FLinearColor(0.95, 0.68, 0.1f, 1.f), DistM,
                                MaxDistAmmoBox, FadeStartSupplyDrop);
        }
    }
}

} // namespace Container
} // namespace Tick
