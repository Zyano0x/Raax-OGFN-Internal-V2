#include "playerfeatures.h"
#include <cheat/cache/playercache.h>
#include <cheat/sdk/sdk.h>
#include <drawing/drawing.h>
#include <config/config.h>
#include <array>
#include <cheat/core.h>

constexpr std::array<std::pair<Cache::Player::BoneIdx, Cache::Player::BoneIdx>, 100> BoneConnections = {
    // Spine & Head
    std::pair(Cache::Player::BoneIdx::Head, Cache::Player::BoneIdx::Neck),
    std::pair(Cache::Player::BoneIdx::Neck, Cache::Player::BoneIdx::Chest),
    std::pair(Cache::Player::BoneIdx::Chest, Cache::Player::BoneIdx::Pelvis),

    // Left Arm
    std::pair(Cache::Player::BoneIdx::Chest, Cache::Player::BoneIdx::L_Shoulder),
    std::pair(Cache::Player::BoneIdx::L_Shoulder, Cache::Player::BoneIdx::L_Elbow),
    std::pair(Cache::Player::BoneIdx::L_Elbow, Cache::Player::BoneIdx::L_Hand),

    // Right Arm
    std::pair(Cache::Player::BoneIdx::Chest, Cache::Player::BoneIdx::R_Shoulder),
    std::pair(Cache::Player::BoneIdx::R_Shoulder, Cache::Player::BoneIdx::R_Elbow),
    std::pair(Cache::Player::BoneIdx::R_Elbow, Cache::Player::BoneIdx::R_Hand),

    // Left Leg
    //std::pair(Cache::Player::BoneIdx::Pelvis, Cache::Player::BoneIdx::L_Hip),
    std::pair(Cache::Player::BoneIdx::Pelvis, Cache::Player::BoneIdx::L_Thigh),
    //std::pair(Cache::Player::BoneIdx::L_Hip, Cache::Player::BoneIdx::L_Thigh),
    std::pair(Cache::Player::BoneIdx::L_Thigh, Cache::Player::BoneIdx::L_Knee),
    std::pair(Cache::Player::BoneIdx::L_Knee, Cache::Player::BoneIdx::L_Foot),

    // Right Leg
    //std::pair(Cache::Player::BoneIdx::Pelvis, Cache::Player::BoneIdx::R_Hip),
    std::pair(Cache::Player::BoneIdx::Pelvis, Cache::Player::BoneIdx::R_Thigh),
    //std::pair(Cache::Player::BoneIdx::R_Hip, Cache::Player::BoneIdx::R_Thigh),
    std::pair(Cache::Player::BoneIdx::R_Thigh, Cache::Player::BoneIdx::R_Knee),
    std::pair(Cache::Player::BoneIdx::R_Knee, Cache::Player::BoneIdx::R_Foot)
};


void Features::Player::TickGameThread() {
    Cache::Player::UpdateCache();
}

void Features::Player::TickRenderThread() {
    for (const auto& [Pawn, Info] : Cache::Player::GetCachedPlayers()) {
        if (Info.Pawn == SDK::GetLocalPawn())
            continue;

        const auto& PlayerConfig = Config::g_Config.Visuals.Player;
        const auto& ColorConfig = Config::g_Config.Color;

        SDK::FLinearColor PrimaryColor = ColorConfig.PrimaryColorVisible;
        SDK::FLinearColor SecondaryColor = ColorConfig.SecondaryColorVisible;

        if (PlayerConfig.Box) {
            switch (PlayerConfig.BoxType) {
            case Config::ConfigData::BoxType::Full:
                Drawing::Rect(Info.BoundTopLeft, (Info.BoundBottomRight - Info.BoundTopLeft), PrimaryColor, PlayerConfig.BoxThickness);
                break;
            case Config::ConfigData::BoxType::Cornered:
                Drawing::CorneredRect(Info.BoundTopLeft, (Info.BoundBottomRight - Info.BoundTopLeft), PrimaryColor, PlayerConfig.BoxThickness);
                break;
            case Config::ConfigData::BoxType::Full3D:
                Drawing::Rect3D(Info.BoundCorners2D, PrimaryColor, PlayerConfig.BoxThickness);
                break;
            }
        }

        if (PlayerConfig.Skeleton) {
            Drawing::BeginBatchedLines();
            for (const auto& Pair : BoneConnections) {
                Drawing::Line(Info.BoneScreenPos[static_cast<int>(Pair.first)], Info.BoneScreenPos[static_cast<int>(Pair.second)], SecondaryColor, PlayerConfig.SkeletonThickness);
            }
            Drawing::EndBatchedLines();
        }

        if (PlayerConfig.Tracer) {
            SDK::FVector2D TracerStart, TracerEnd;

            switch (PlayerConfig.TracerStart) {
            case Config::ConfigData::TracerPos::Bottom: TracerStart = SDK::FVector2D(Core::ScreenSizeX / 2.f, Core::ScreenSizeY); break;
            case Config::ConfigData::TracerPos::Middle: TracerStart = SDK::FVector2D(Core::ScreenSizeX / 2.f, Core::ScreenSizeY / 2.f); break;
            case Config::ConfigData::TracerPos::Top: TracerStart = SDK::FVector2D(Core::ScreenSizeX / 2.f, 0.f); break;
            }
            switch (PlayerConfig.TracerEnd) {
            case Config::ConfigData::TracerPos::Bottom: TracerEnd = Info.BoxBottom; break;
            case Config::ConfigData::TracerPos::Middle: TracerEnd = Info.BoxMiddle; break;
            case Config::ConfigData::TracerPos::Top: TracerEnd = Info.BoxTop; break;
            }

            Drawing::Line(TracerStart, TracerEnd, PrimaryColor, PlayerConfig.TracerThickness);
        }

        if (PlayerConfig.Name) {
            Drawing::Text(Info.PlayerName.c_str(), SDK::FVector2D(Info.BoxTop.X, (Info.BoxTop.Y - Info.FontSize) - 2.f), PrimaryColor, Info.FontSize, true, false);
        }
    }
}
