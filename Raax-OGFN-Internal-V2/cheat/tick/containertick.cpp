#include "containertick.h"
#include <format>

#include <cheat/cache/containercache.h>
#include <cheat/core.h>
#include <drawing/drawing.h>
#include <config/config.h>

namespace Tick {
namespace Container {

// --- Container Utility Functions -----------------------------------

void RenderContainer(const Cache::Container::ContainerInfo& Info, const std::string& FormatStr, float DistM, float MaxDist, float FadeStart) {
    if (DistM <= MaxDist) {
        float Alpha = 1.0f;
        if (DistM > FadeStart)
            Alpha = 1.0f - ((DistM - FadeStart) / (MaxDist - FadeStart));

        SDK::FLinearColor Color(0.95f, 0.68f, 0.1f, Alpha);
        SDK::FLinearColor OutlineColor = SDK::FLinearColor::Black;
        OutlineColor.A = Color.A;

        char Buffer[64];
        snprintf(Buffer, sizeof(Buffer), FormatStr.c_str(), static_cast<int>(DistM));
        Drawing::Text(Buffer, Info.RootScreenLocation, Color, 12.f, true, true, true, 1.f, OutlineColor);
    }
}

// --- Public Tick Functions -----------------------------------------

void TickGameThread() {
    Cache::Container::UpdateCache();
}
void TickRenderThread() {
    const auto& LootConfig = Config::g_Config.Visuals.Loot;
    bool ChestText = LootConfig.ChestText;
    bool AmmoBoxText = LootConfig.AmmoBoxText;
    if (!ChestText && !AmmoBoxText)
        return;

    float MaxDistChest = LootConfig.ChestMaxDistance;
    float MaxDistAmmoBox = LootConfig.AmmoBoxMaxDistance;
    float FadeStartChest = MaxDistChest * 0.2f;
    float FadeStartAmmoBox = MaxDistAmmoBox * 0.2f;

    for (const auto& [_, Info] : Cache::Container::GetCachedContainers()) {
        if (Info.Container->bAlreadySearched())
            continue;

        float DistM = Info.RootWorldLocation.Dist(Core::g_CameraLocation) / 100.f;
        if (Info.Type == Cache::Container::ContainerType::Chest && ChestText) {
            RenderContainer(Info, "Chest [{} m]", DistM, MaxDistChest, FadeStartChest);
        } else if (Info.Type == Cache::Container::ContainerType::AmmoBox && AmmoBoxText) {
            RenderContainer(Info, "Ammo Box [{} m]", DistM, MaxDistAmmoBox, FadeStartAmmoBox);
        }
    }
}

} // namespace Container
} // namespace Tick
