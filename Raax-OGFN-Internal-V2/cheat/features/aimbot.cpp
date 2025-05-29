#include "aimbot.h"
#include <vector>
#include <unordered_map>
#include <random>

#include <cheat/features/weaponutils.h>
#include <cheat/Input.h>
#include <extern/imgui/imgui.h>
#include <cheat/sdk/sdk.h>
#include <cheat/core.h>
#include <cheat/cache/playercache.h>
#include <config/config.h>
#include <drawing/drawing.h>
#include <utils/math.h>

namespace Features {
namespace Aimbot {

// --- Target & Global Data ------------------------------------------

struct TargetInfo {
    SDK::AFortPawn* Pawn = nullptr;
    SDK::FRotator   RotationDelta;
    SDK::FVector2D  ScreenPos;
    float           DistDeg = 0.f;
    float           DistanceM = 0.f;
    float           DistCombined = 0.f;
    bool            InDeadZone = false;
    bool            Visible = false;

    void Reset() { Pawn = nullptr; }
} target;

float WorldGravityZ = 0.f;

// --- Global Data Functions -----------------------------------------

void UpdateWorldGravityZ() {
    SDK::UWorld* World = SDK::GetWorld();
    if (!World)
        return;

    SDK::ULevel* PersistentLevel = World->PersistentLevel;
    if (!PersistentLevel)
        return;

    SDK::AWorldSettings* WorldSettings = PersistentLevel->WorldSettings;
    if (!WorldSettings)
        return;

    WorldGravityZ = WorldSettings->WorldGravityZ;
}

// --- Target Functions ----------------------------------------------

bool EvaluateTarget(const Cache::Player::PlayerInfo& Info) {
    auto& s = WeaponUtils::GetState();
    if (Info.DistanceM > s.Config.MaxDistance) {
        return false;
    }

    if (s.Config.VisibleCheck && !Info.HeadVisible) {
        return false;
    }

    int BoneIdx = static_cast<int>(s.CurrentBone);

    SDK::FVector   TargetWorldPos = Info.BoneWorldPos[BoneIdx];
    SDK::FVector2D TargetScreenPos = Info.BoneScreenPos[BoneIdx];
    float          DistanceM = Info.DistanceM;

    if (s.UseProjectile && s.ProjectileTemp && Config::g_Config.Aimbot.BulletPrediction) {
        float GravityZ = WorldGravityZ * s.GravityScales[s.ProjectileTemp->Class];
        TargetWorldPos = WeaponUtils::PredictProjectile(Core::g_CameraLocation, TargetWorldPos,
                                                        Info.Pawn->RootComponent->ComponentVelocity,
                                                        s.ProjectileTemp->GetDefaultSpeed(1.f), GravityZ);
    }

    SDK::FRotator AimRot = Math::FindLookAtRotation(Core::g_CameraLocation, TargetWorldPos);
    float         DistDeg = Math::GetDegreeDistance(Core::g_CameraRotation, AimRot);
    float         DistPix = TargetScreenPos.Dist({(float)Core::g_ScreenCenterX, (float)Core::g_ScreenCenterY});
    float         DistCombined = DistDeg + DistanceM * 0.25f;

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
        if ((s.Config.Selection == Config::ConfigData::TargetSelection::Distance && DistanceM > target.DistanceM) ||
            (s.Config.Selection == Config::ConfigData::TargetSelection::Degrees && DistDeg > target.DistDeg) ||
            (s.Config.Selection == Config::ConfigData::TargetSelection::Combined && DistCombined > target.DistCombined))
            return false;
    }

    // accept this target
    target.Pawn = Info.Pawn;
    target.ScreenPos = TargetScreenPos;
    target.DistDeg = DistDeg;
    target.DistanceM = DistanceM;
    target.DistCombined = DistCombined;
    target.InDeadZone = InDeadZone;
    target.Visible = Info.HeadVisible;

    SDK::FRotator RotationDelta(AimRot.Pitch - Core::g_CameraRotation.Pitch, AimRot.Yaw - Core::g_CameraRotation.Yaw,
                                0.f);
    RotationDelta.Normalize();
    RotationDelta = RotationDelta / s.EffectiveSmooth;
    RotationDelta.Normalize();
    target.RotationDelta = RotationDelta;
    return true;
}

void AcquireTarget() {
    auto& s = WeaponUtils::GetState();

    bool Found = false;
    if (target.Pawn) {
        for (auto& [_, Info] : Cache::Player::GetCachedPlayers()) {
            if (Info.DistanceM >= Config::g_Config.Visuals.Player.MaxDistance || Info.IsDBNO || Info.IsDead)
                continue;

            if (Info.Pawn == target.Pawn) {
                Found = EvaluateTarget(Info);
                break;
            }
        }
    }
    if (!Found)
        target.Reset();
    else if (s.IsTargeting && s.Config.StickyTarget)
        return;

    for (auto& [_, Info] : Cache::Player::GetCachedPlayers()) {
        if (Info.Pawn == SDK::GetLocalPawn() || Info.DistanceM >= Config::g_Config.Visuals.Player.MaxDistance ||
            Info.TeamIndex == Core::g_LocalTeamIndex || Info.IsDBNO || Info.IsDead)
            continue;
        EvaluateTarget(Info);
    }
}

// --- Public Tick Functions -----------------------------------------

void TickGameThread() {
    auto& s = WeaponUtils::GetState();
    AcquireTarget();

    if (s.Config.Enabled && s.CurrentAmmo != WeaponUtils::AmmoType::Unknown && target.Pawn && s.IsTargeting) {
        if (!s.Config.UseDeadzone || !target.InDeadZone) {
            SDK::APlayerController* Controller = SDK::GetLocalController();
            if (Controller) {
                Controller->AddPitchInput(target.RotationDelta.Pitch / Controller->InputPitchScale);
                Controller->AddYawInput(target.RotationDelta.Yaw / Controller->InputYawScale);
            }
        }
    }
}

void TickRenderThread() {
    auto& s = WeaponUtils::GetState();
    if (!s.Config.Enabled || s.CurrentAmmo == WeaponUtils::AmmoType::Unknown)
        return;

    s.IsTargeting = Input::IsKeyDown(Config::g_Config.Aimbot.AimbotKeybind);
    SDK::FVector2D Center =
        SDK::FVector2D(static_cast<float>(Core::g_ScreenCenterX), static_cast<float>(Core::g_ScreenCenterY));
    SDK::FLinearColor TargetColor =
        target.Visible ? Config::g_Config.Color.PrimaryColorVisible : Config::g_Config.Color.PrimaryColorHidden;

    if (s.Config.ShowFOV)
        Drawing::Circle(Center, s.Config.FOV * Core::g_PixelsPerDegree, 64, SDK::FLinearColor::White);

    if (s.Config.UseDeadzone && s.Config.ShowDeadzoneFOV)
        Drawing::Circle(Center, s.Config.DeadzoneFOV * Core::g_PixelsPerDegree, 64, SDK::FLinearColor::Red);

    if (Config::g_Config.Aimbot.ShowTargetLine && target.Pawn)
        Drawing::Line(Center, target.ScreenPos, TargetColor);
}

} // namespace Aimbot
} // namespace Features
