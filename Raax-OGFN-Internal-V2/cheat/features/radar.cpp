#include "radar.h"

#include <utils/log.h>
#include <utils/math.h>
#include <drawing/drawing.h>
#include <cheat/core.h>
#include <cheat/cache/playercache.h>
#include <config/config.h>

namespace Features {
namespace Radar {

// --- Radar Utility Functions ---------------------------------------

void DrawGuidelines(const SDK::FLinearColor& GuidelineColor, const SDK::FLinearColor& CrosshairColor,
                    const SDK::FVector2D& RadarPos, const SDK::FVector2D& RadarCenterPos, float Size) {
    constexpr int StepCount = 6;
    float         Step = (Size / StepCount);
    // Vertical lines
    for (int i = 0; i < StepCount; i++) {
        float GuidelineX = RadarPos.X + (Step * i);
        float GuidelineStartY = RadarPos.Y;
        float GuidelineEndY = GuidelineStartY + Size;

        Drawing::Line(SDK::FVector2D(GuidelineX, GuidelineStartY), SDK::FVector2D(GuidelineX, GuidelineEndY),
                      GuidelineColor);
    }

    // Horizontal lines
    for (int i = 0; i < StepCount; i++) {
        float GuidelineStartX = RadarPos.X;
        float GuidelineEndX = RadarPos.X + Size;
        float GuidelineY = RadarPos.Y + (Step * i);

        Drawing::Line(SDK::FVector2D(GuidelineStartX, GuidelineY), SDK::FVector2D(GuidelineEndX, GuidelineY),
                      GuidelineColor);
    }

    // Crosshair
    Drawing::Line(SDK::FVector2D(RadarCenterPos.X - Size / 2.f, RadarCenterPos.Y),
                  SDK::FVector2D(RadarCenterPos.X + Size / 2.f, RadarCenterPos.Y), CrosshairColor);
    Drawing::Line(SDK::FVector2D(RadarCenterPos.X, RadarCenterPos.Y - Size / 2.f),
                  SDK::FVector2D(RadarCenterPos.X, RadarCenterPos.Y + Size / 2.f), CrosshairColor);
}

void DrawPlayerIcons(const Config::ConfigData::VisualsConfig::RadarConfig& RadarConfig,
                     const SDK::FVector2D& RadarCenterPos, float CameraYaw, float Size) {
    float RadarRangeCm = RadarConfig.MaxDistance * 2 * 100.f;
    float PixelsPerCm = Size / RadarRangeCm;

    float CosCameraYaw = std::cos(CameraYaw - Math::DegreesToRadians(90.f));
    float SinCameraYaw = std::sin(CameraYaw - Math::DegreesToRadians(90.f));

    for (const auto& [_, Info] : Cache::Player::GetCachedPlayers()) {
        if (Info.Pawn == SDK::GetLocalPawn() || Info.TeamIndex == Core::g_LocalTeamIndex) {
            continue;
        }

        float RelX = Info.RootWorldLocation.X - Core::g_LocalPlayerPos.X;
        float RelY = Info.RootWorldLocation.Y - Core::g_LocalPlayerPos.Y;

        if (RadarConfig.RotateWithCamera) {
            float RotX = RelX * CosCameraYaw - RelY * SinCameraYaw;
            float RotY = RelX * SinCameraYaw + RelY * CosCameraYaw;
            RelX = RotX;
            RelY = RotY;
        }

        float Dist = std::sqrt(RelX * RelX + RelY * RelY);
        if (Dist > RadarRangeCm / 2.f)
            continue;

        SDK::FVector2D RadarIconPos;
        RadarIconPos.X = RadarCenterPos.X + RelX * PixelsPerCm;
        RadarIconPos.Y = RadarCenterPos.Y + RelY * PixelsPerCm;

        SDK::FLinearColor Color = Info.HeadVisible ? RadarConfig.ColorVisible : RadarConfig.ColorHidden;
        Drawing::Circle(RadarIconPos, 2.f, 4, Color, 2.f);
    }
}

void DrawFOVLines(const Config::ConfigData::VisualsConfig::RadarConfig& RadarConfig,
                  const SDK::FVector2D& RadarCenterPos, float CameraYaw, float Size) {
    if (RadarConfig.ShowCameraFOV) {
        float FOVLength = Size * 0.5f;

        float FOVAngleUp = RadarConfig.RotateWithCamera ? Math::DegreesToRadians(90.f) : CameraYaw;
        float LeftAngle = -(FOVAngleUp - Math::DegreesToRadians(Core::g_FOV * 0.5f));
        float RightAngle = -(FOVAngleUp + Math::DegreesToRadians(Core::g_FOV * 0.5f));

        SDK::FVector2D FOVLeftEnd = {RadarCenterPos.X + std::cos(LeftAngle) * FOVLength,
                                     RadarCenterPos.Y + std::sin(LeftAngle) * FOVLength};

        SDK::FVector2D FOVRightEnd = {RadarCenterPos.X + std::cos(RightAngle) * FOVLength,
                                      RadarCenterPos.Y + std::sin(RightAngle) * FOVLength};

        Drawing::Line(RadarCenterPos, FOVLeftEnd, SDK::FLinearColor::White, 1.5f);
        Drawing::Line(RadarCenterPos, FOVRightEnd, SDK::FLinearColor::White, 1.5f);
    }
}

// --- Public Tick Functions -----------------------------------------

void TickRenderThread() {
    const auto& RadarConfig = Config::g_Config.Visuals.Radar;
    if (!RadarConfig.Radar)
        return;

    float Size = std::max(Core::g_ScreenSizeX, Core::g_ScreenSizeY) * RadarConfig.Size / 100.f;
    float PosX = Core::g_ScreenSizeX * RadarConfig.PosX / 100.f;
    float PosY = Core::g_ScreenSizeY * RadarConfig.PosY / 100.f;

    SDK::FVector2D RadarSize = SDK::FVector2D(Size, Size);
    SDK::FVector2D RadarPos = SDK::FVector2D(PosX, PosY);
    SDK::FVector2D RadarCenterPos = SDK::FVector2D(RadarPos.X + Size / 2.f, RadarPos.Y + Size / 2.f);

    Drawing::RectFilled(RadarPos, RadarSize, SDK::FLinearColor::Black);

    constexpr SDK::FLinearColor SoftGray = SDK::FLinearColor(0.25f, 0.25f, 0.25f, 1.f);
    DrawGuidelines(SoftGray, SDK::FLinearColor::Gray, RadarPos, RadarCenterPos, Size);

    float CameraYaw = Math::DegreesToRadians(-Core::g_CameraRotation.Yaw);
    DrawPlayerIcons(RadarConfig, RadarCenterPos, CameraYaw, Size);

    DrawFOVLines(RadarConfig, RadarCenterPos, CameraYaw, Size);
}

} // namespace Radar
} // namespace Features
