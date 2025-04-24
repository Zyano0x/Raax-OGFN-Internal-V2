#include "containertick.h"
#include <cheat/cache/containercache.h>
#include <cheat/core.h>
#include <drawing/drawing.h>
#include <config/config.h>

void Tick::Container::TickGameThread() {
    Cache::Container::UpdateCache();
}

void Tick::Container::TickRenderThread() {
    for (const auto& [Id, Info] : Cache::Container::GetCachedContainers()) {
        if (Info.Container->bAlreadySearched())
            continue;

        if (Info.Type == Cache::Container::ContainerType::Chest && Config::g_Config.Visuals.Loot.ChestText)
            Drawing::Text("Chest", Info.RootScreenLocation, SDK::FLinearColor(0.95f, 0.68f, 0.1f, 1.f));
        else if (Info.Type == Cache::Container::ContainerType::AmmoBox && Config::g_Config.Visuals.Loot.AmmoBoxText)
            Drawing::Text("Ammo Box", Info.RootScreenLocation, SDK::FLinearColor(0.7f, 0.7f, 0.7f, 1.f));
    }
}
