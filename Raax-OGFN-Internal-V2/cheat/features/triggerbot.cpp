#include "triggerbot.h"
#include <chrono>

#include <utils/log.h>
#include <utils/math.h>
#include <drawing/drawing.h>
#include <cheat/core.h>
#include <cheat/cache/playercache.h>
#include <cheat/features/weaponutils.h>
#include <config/config.h>

namespace Features {
namespace TriggerBot {

// --- Trigger Bot Data ----------------------------------------------

bool PressingKey = false;

// --- Trigger Bot Utility Functions ---------------------------------

bool ShouldRun() {
    auto&       s = WeaponUtils::GetState();
    const auto& Config = Config::g_Config.TriggerBot;
    return Config.Enabled && s.CurrentAmmo != WeaponUtils::AmmoType::Unknown && s.Config.UseInTriggerBot;
}

bool IsCandidate(const Cache::Player::PlayerInfo& Info) {
    const auto& Config = Config::g_Config.TriggerBot;
    if (!Info.HeadVisible || Info.Pawn == SDK::GetLocalPawn() || Info.DistanceM > Config.MaxDistance) {
        return false;
    }

    int BoneIdx = (int)Cache::Player::BoneIdx::Head;

    auto  TargetWorldPos = Info.BoneWorldPos[BoneIdx];
    auto  TargetScreenPos = Info.BoneScreenPos[BoneIdx];
    float DistMeters = Info.DistanceM;

    auto  AimRot = Math::FindLookAtRotation(Core::g_CameraLocation, TargetWorldPos);
    float DistDeg = Math::GetDegreeDistance(Core::g_CameraRotation, AimRot);
    float DistPix = TargetScreenPos.Dist({(float)Core::g_ScreenCenterX, (float)Core::g_ScreenCenterY});
    float DistCombined = DistDeg + DistMeters * 0.25f;

    if (Math::IsOnScreen(TargetScreenPos) && (DistPix > Config.FOV * Core::g_PixelsPerDegree)) {
        return false;
    } else if (DistDeg > Config.FOV) {
        return false;
    }

    return true;
}

// --- Public Tick Functions -----------------------------------------

void TickGameThread() {
    if (!ShouldRun()) {
        return;
    }

    bool FoundCandidate = false;
    for (const auto& [_, Info] : Cache::Player::GetCachedPlayers()) {
        if (IsCandidate(Info)) {
            FoundCandidate = true;
            break;
        }
    }

    static std::chrono::time_point<std::chrono::high_resolution_clock> LastVisibleTime;
    auto                                                               Now = std::chrono::high_resolution_clock::now();
    float Elapsed = std::chrono::duration<float>(Now - LastVisibleTime).count();

    const auto& Config = Config::g_Config.TriggerBot;
    if (FoundCandidate && (PressingKey || !Config.UseKeybind)) {
        if (Elapsed >= Config.FireDelayS) {
            auto* Controller = SDK::Cast<SDK::AFortPlayerController>(SDK::GetLocalController());
            if (Controller) {
                Controller->Fire_Press();
                Controller->Fire_Release();
            }
        }
    } else {
        LastVisibleTime = Now;
    }
}

void TickRenderThread() {
    if (!ShouldRun()) {
        return;
    }

#ifndef _ENGINE
    const auto& Config = Config::g_Config.TriggerBot;
    PressingKey = ImGui::IsKeyDown((ImGuiKey)Config.Keybind);

    SDK::FVector2D Center = SDK::FVector2D(Core::g_ScreenCenterX, Core::g_ScreenCenterY);
    Drawing::Circle(Center, Config.FOV * Core::g_PixelsPerDegree, 64, SDK::FLinearColor::Green);
#endif
}

} // namespace TriggerBot
} // namespace Features
