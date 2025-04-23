#pragma once
#include <cheat/sdk/sdk.h>
#include <unordered_map>

namespace Cache::Player
{
    enum class BoneIdx : int
    {
        MIN = 0,
        Head = 0,
        Neck,
        Pelvis,

        L_Shoulder,
        L_Elbow,
        L_Hand,
        R_Shoulder,
        R_Elbow,
        R_Hand,

        Chest,

        // L_Hip,
        L_Thigh,
        L_Knee,
        L_Foot,
        //R_Hip,
        R_Thigh,
        R_Knee,
        R_Foot,

        NUM
    };
    struct PlayerInfo
    {
        SDK::AFortPawn* Pawn = nullptr;
        SDK::APlayerState* PlayerState = nullptr;
        SDK::USkeletalMeshComponent* Mesh = nullptr;

        std::string PlayerName;
        int32_t BoneIndicies[(int)BoneIdx::NUM];

        SDK::FVector RootLocation;
        SDK::FVector BoneWorldPos[(int)BoneIdx::NUM];
        SDK::FVector2D BoneScreenPos[(int)BoneIdx::NUM];

        SDK::FVector2D BoundTopLeft, BoundBottomRight;
        SDK::FVector BoundCorners3D[8];
        SDK::FVector2D BoundCorners2D[8];
        SDK::FVector2D BoxTop, BoxMiddle, BoxBottom;
        float FontSize = 12.f;

        bool WasSeenThisFrame = 0;
    };

    constexpr int NumBones = static_cast<int>(BoneIdx::NUM);

    const std::unordered_map<SDK::AFortPawn*, PlayerInfo>& GetCachedPlayers();
    void UpdateCache();
}
