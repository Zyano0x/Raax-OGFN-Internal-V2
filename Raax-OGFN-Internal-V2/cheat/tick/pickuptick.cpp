#include "pickuptick.h"
#include <cheat/cache/pickupcache.h>
#include <cheat/core.h>
#include <drawing/drawing.h>
#include <config/config.h>

SDK::FLinearColor GetTierColor(SDK::EFortItemTier Tier) {
	switch (Tier) {
		// Common
	case SDK::EFortItemTier::I:
		return SDK::FLinearColor(0.74f, 0.74f, 0.71f, 1.0f);

		// Uncommon
	case SDK::EFortItemTier::II:
		return SDK::FLinearColor(0.12f, 0.87f, 0.11f, 1.0f);

		// Rare
	case SDK::EFortItemTier::III:
		return SDK::FLinearColor(0.29f, 0.33f, 0.95f, 1.0f);

		// Epic
	case SDK::EFortItemTier::IV:
		return SDK::FLinearColor(0.65f, 0.27f, 0.82f, 1.0f);

		// Legendary
	case SDK::EFortItemTier::V:
		return SDK::FLinearColor(0.95f, 0.40f, 0.07f, 1.0f);

		// Mythic
	case SDK::EFortItemTier::VI:
		return SDK::FLinearColor(0.98f, 0.85f, 0.29f, 1.0f);

	default:
		return SDK::FLinearColor(0.74f, 0.74f, 0.71f, 1.0f);
	}

	return SDK::FLinearColor(1.f, 1.f, 1.f, 1.f);
}


void Tick::Pickup::TickGameThread() {
    Cache::Pickup::UpdateCache();
}

void Tick::Pickup::TickRenderThread() {
    for (const auto& [Id, Info] : Cache::Pickup::GetCachedPickups()) {
        if (Config::g_Config.Visuals.Loot.LootText && static_cast<int>(Info.Tier) >= static_cast<int>(Config::g_Config.Visuals.Loot.MinLootTier))
            Drawing::Text(Info.WeaponName.c_str(), Info.RootScreenLocation, GetTierColor(Info.Tier));
    }
}
