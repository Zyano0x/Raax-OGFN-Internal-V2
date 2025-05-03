#pragma once

#include "aimbot.h"
#include <vector>
#include <unordered_map>
#include <random>

#include <extern/imgui/imgui.h>
#include <cheat/sdk/sdk.h>
#include <cheat/core.h>
#include <cheat/cache/playercache.h>
#include <config/config.h>
#include <drawing/drawing.h>
#include <utils/math.h>

namespace Features {
namespace Aimbot {

// --- State & Config ------------------------------------------------

enum class AmmoType { Shells, Light, Medium, Heavy, Other, Unknown };

struct State {
    AmmoType                                           CurrentAmmo = AmmoType::Shells;
    bool                                               UseProjectile = false;
    SDK::AFortProjectileBase*                          ProjectileTemp = nullptr;
    std::unordered_map<SDK::UClass*, float>            GravityScales;
    float                                              EffectiveSmooth = 1.f;
    bool                                               IsTargeting = false;
    Config::ConfigData::AimbotConfig::AimbotAmmoConfig Config;
    std::mt19937                                       RNG{std::random_device{}()};
    Cache::Player::BoneIdx                             RandomBone = Cache::Player::BoneIdx::Head;
} s;

struct TargetInfo {
    SDK::AFortPawn* Pawn = nullptr;
    SDK::FRotator   RotationDelta;
    float           DistDeg = 0.f;
    float           DistMeters = 0.f;
    float           DistCombined = 0.f;
    bool            InDeadZone = false;

    void reset() { Pawn = nullptr; }
} target;

// --- Utility Functions ---------------------------------------------

inline SDK::FVector PredictProjectile(const SDK::FVector& Origin, const SDK::FVector& Target,
                                      const SDK::FVector& TargetVel, float BulletSpeed, float GravityZ) {
    SDK::FVector Predicted = Target;
    for (int i = 0; i < 10; ++i) {
        SDK::FVector Delta = Predicted - Origin;
        float        t = Delta.Size() / std::fabs(BulletSpeed);
        Predicted = Target + TargetVel * t;
        Predicted.Z -= 0.5f * GravityZ * t * t;
    }
    return Predicted;
}

Cache::Player::BoneIdx PickRandomBone() {
    static std::chrono::time_point<std::chrono::high_resolution_clock> LastBoneRefresh;
    auto                                                               Now = std::chrono::high_resolution_clock::now();
    float Elapsed = std::chrono::duration<float>(Now - LastBoneRefresh).count();
    if (Elapsed >= s.Config.RandomBoneRefreshRate) {
        LastBoneRefresh = Now;
        std::uniform_int_distribution<int> Dist(0, 3);
        switch (Dist(s.RNG)) {
        case 0:
            s.RandomBone = Cache::Player::BoneIdx::Head;
            break;
        case 1:
            s.RandomBone = Cache::Player::BoneIdx::Neck;
            break;
        case 2:
            s.RandomBone = Cache::Player::BoneIdx::Pelvis;
            break;
        case 3:
            s.RandomBone = Cache::Player::BoneIdx::Chest;
            break;
        }
    }
    return s.RandomBone;
}

// --- Per-Frame Updates ---------------------------------------------

void UpdateGravityScales() {
    static std::vector<SDK::AFortProjectileBase*> Projectiles;
    SDK::GetAllActorsOfClass<SDK::AFortProjectileBase>(Projectiles);
    for (auto& Proj : Projectiles) {
        if (Proj->IsA(SDK::AFortProjectileBase::StaticClass())) {
            s.GravityScales[Proj->Class] = Proj->GravityScale();
        }
    }
}

void DetectAmmoType() {
    auto* Pawn = SDK::Cast<SDK::AFortPawn>(SDK::GetLocalPawn());
    if (!Pawn) {
        s.CurrentAmmo = AmmoType::Unknown;
        return;
    }

    auto* Weapon = Pawn->CurrentWeapon();
    if (!Weapon) {
        s.CurrentAmmo = AmmoType::Unknown;
        return;
    }

    static SDK::AFortWeapon* LastWeapon = nullptr;
    if (Weapon == LastWeapon)
        return;
    LastWeapon = Weapon;

    s.UseProjectile = false;
    auto* WeaponData = Weapon->WeaponData();
    if (!WeaponData) {
        s.CurrentAmmo = AmmoType::Unknown;
        return;
    }

    if (auto* RangedWeaponData = SDK::Cast<SDK::UFortWeaponRangedItemDefinition>(WeaponData)) {
        if (auto* ProjTemp = RangedWeaponData->ProjectileTemplate()) {
            if (auto* DefaultProjTemp = SDK::Cast<SDK::AFortProjectileBase>(ProjTemp->ClassDefaultObject())) {
                s.UseProjectile = true;
                s.ProjectileTemp = DefaultProjTemp;
            }
        }
    }

    auto* AmmoData = WeaponData->AmmoData();
    if (!AmmoData) {
        s.CurrentAmmo = AmmoType::Unknown;
        return;
    }

    auto AmmoName = AmmoData->DisplayName()->ToString();
    if (AmmoName == "Ammo: Shells")
        s.CurrentAmmo = AmmoType::Shells;
    else if (AmmoName == "Ammo: Light Bullets")
        s.CurrentAmmo = AmmoType::Light;
    else if (AmmoName == "Ammo: Medium Bullets")
        s.CurrentAmmo = AmmoType::Medium;
    else if (AmmoName == "Ammo: Heavy Bullets")
        s.CurrentAmmo = AmmoType::Heavy;
    else
        s.CurrentAmmo = AmmoType::Other;
}

void UpdateConfig() {
    s.Config = s.CurrentAmmo != AmmoType::Unknown && Config::g_Config.Aimbot.SplitAimbotByAmmo
                   ? [&] {
                         switch (s.CurrentAmmo) {
                         case AmmoType::Shells:
                             return Config::g_Config.Aimbot.ShellsAimbot;
                         case AmmoType::Light:
                             return Config::g_Config.Aimbot.LightAimbot;
                         case AmmoType::Medium:
                             return Config::g_Config.Aimbot.MediumAimbot;
                         case AmmoType::Heavy:
                             return Config::g_Config.Aimbot.HeavyAimbot;
                         default:
                             return Config::g_Config.Aimbot.OtherAimbot;
                         }
                     }()
                   : Config::g_Config.Aimbot.AllAimbot;
}

void UpdateSmoothness() {
    static std::chrono::time_point<std::chrono::high_resolution_clock> LastFrameTime;
    auto                                                               Now = std::chrono::high_resolution_clock::now();
    float Elapsed = std::chrono::duration<float>(Now - LastFrameTime).count();
    LastFrameTime = Now;

    if (s.Config.Smoothness <= 1.f)
        s.EffectiveSmooth = s.Config.Smoothness;
    else
        s.EffectiveSmooth = s.Config.Smoothness * ((1.f / 120.f) / Elapsed);
}

// --- Target Acquisition --------------------------------------------

bool EvaluateTarget(const Cache::Player::PlayerInfo& Info) {
    if (s.Config.VisibleCheck && !Info.HeadVisible) {
        return false;
    }

    int BoneIdx = static_cast<int>(s.Config.Bone == Config::ConfigData::TargetBone::Random
                                       ? PickRandomBone()
                                       : static_cast<Cache::Player::BoneIdx>(s.Config.Bone));

    auto  TargetWorldPos = Info.BoneWorldPos[BoneIdx];
    auto  TargetScreenPos = Info.BoneScreenPos[BoneIdx];
    float DistMeters = Core::g_CameraLocation.Dist(TargetWorldPos) / 100.f;

    if (s.UseProjectile && s.ProjectileTemp && Config::g_Config.Aimbot.BulletPrediction) {
        float GravityZ = SDK::GetWorld()->PersistentLevel()->WorldSettings()->WorldGravityZ() *
                         s.GravityScales[s.ProjectileTemp->Class];
        TargetWorldPos =
            PredictProjectile(Core::g_CameraLocation, TargetWorldPos, Info.Pawn->RootComponent()->ComponentVelocity(),
                              s.ProjectileTemp->GetDefaultSpeed(1.f), GravityZ);
    }

    auto  AimRot = Math::FindLookAtRotation(Core::g_CameraLocation, TargetWorldPos);
    float DistDeg = Math::GetDegreeDistance(Core::g_CameraRotation, AimRot);
    float DistPix = TargetScreenPos.Dist({Core::g_ScreenSizeX / 2.f, Core::g_ScreenSizeY / 2.f});
    float DistCombined = DistDeg + DistMeters * 0.4f;

    if (DistMeters > s.Config.MaxDistance)
        return false;

    bool InDeadZone = false;
    if (Math::IsOnScreen(TargetScreenPos)) {
        if (DistPix > s.Config.FOV * Core::g_PixelsPerDegree)
            return false;
        InDeadZone = DistPix <= s.Config.DeadzoneFOV * Core::g_PixelsPerDegree;
    } else {
        if (DistDeg > s.Config.FOV)
            return false;
        InDeadZone = DistDeg <= s.Config.DeadzoneFOV;
    }

    if (target.Pawn && Info.Pawn != target.Pawn) {
        if ((s.Config.Selection == Config::ConfigData::TargetSelection::Distance && DistMeters > target.DistMeters) ||
            (s.Config.Selection == Config::ConfigData::TargetSelection::Degrees && DistDeg > target.DistDeg) ||
            (s.Config.Selection == Config::ConfigData::TargetSelection::Combined && DistCombined > target.DistCombined))
            return false;
    }

    // accept this target
    target.Pawn = Info.Pawn;
    target.DistDeg = DistDeg;
    target.DistMeters = DistMeters;
    target.DistCombined = DistCombined;
    target.InDeadZone = InDeadZone;

    SDK::FRotator RotationDelta(AimRot.Pitch - Core::g_CameraRotation.Pitch, AimRot.Yaw - Core::g_CameraRotation.Yaw,
                                0.f);
    RotationDelta.Normalize();
    RotationDelta = RotationDelta / s.EffectiveSmooth;
    RotationDelta.Normalize();
    target.RotationDelta = RotationDelta;
    return true;
}

void AcquireTarget() {
    bool Found = false;
    if (target.Pawn) {
        for (auto& [_, Info] : Cache::Player::GetCachedPlayers()) {
            if (Info.DistanceM >= Config::g_Config.Visuals.Player.MaxDistance)
                continue;

            if (Info.Pawn == target.Pawn) {
                Found = EvaluateTarget(Info);
                break;
            }
        }
    }
    if (!Found)
        target.reset();
    if (Found && s.IsTargeting && s.Config.StickyTarget)
        return;

    for (auto& [_, Info] : Cache::Player::GetCachedPlayers()) {
        if (Info.Pawn == SDK::GetLocalPawn() || Info.DistanceM >= Config::g_Config.Visuals.Player.MaxDistance)
            continue;
        EvaluateTarget(Info);
    }
}

// --- Public Tick Functions -----------------------------------------

void TickGameThread() {
    UpdateGravityScales();
    DetectAmmoType();
    UpdateConfig();
    UpdateSmoothness();
    AcquireTarget();

    // Aim at target
    if (s.Config.Enabled && s.CurrentAmmo != AmmoType::Unknown && target.Pawn && s.IsTargeting) {
        if (!s.Config.UseDeadzone || !target.InDeadZone) {
            auto* Controller = SDK::GetLocalController();
            Controller->AddPitchInput(target.RotationDelta.Pitch / Controller->InputPitchScale());
            Controller->AddYawInput(target.RotationDelta.Yaw / Controller->InputYawScale());
        }
    }
}

void TickRenderThread() {
    if (!s.Config.Enabled || s.CurrentAmmo == AmmoType::Unknown)
        return;
    s.IsTargeting = ImGui::IsKeyDown((ImGuiKey)Config::g_Config.Aimbot.AimbotKeybind);
    auto Center = SDK::FVector2D(Core::g_ScreenSizeX / 2.f, Core::g_ScreenSizeY / 2.f);
    if (s.Config.ShowFOV)
        Drawing::Circle(Center, s.Config.FOV * Core::g_PixelsPerDegree, 64);
    if (s.Config.UseDeadzone && s.Config.ShowDeadzoneFOV)
        Drawing::Circle(Center, s.Config.DeadzoneFOV * Core::g_PixelsPerDegree, 64, SDK::FLinearColor::Red);
}

} // namespace Aimbot
} // namespace Features
