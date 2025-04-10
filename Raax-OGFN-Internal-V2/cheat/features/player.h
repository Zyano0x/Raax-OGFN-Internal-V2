#pragma once
#include <cheat/sdk/sdk.h>
#include <array>
#include <vector>

namespace Features::Player
{
    enum class BoneIdx : int
    {
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
    struct PlayerCache
    {
        SDK::AFortPawn* Pawn = nullptr;
        SDK::USkeletalMeshComponent* Mesh = nullptr;
        SDK::FVector BonePos[(int)BoneIdx::NUM];
        SDK::FVector2D BonePos2D[(int)BoneIdx::NUM];
    };

    void PopulateBones(PlayerCache& Cache);
    void UpdateCache();

    void Tick();

    constexpr std::array<std::pair<BoneIdx, BoneIdx>, 100> BonePairs =
    {
        // Spine & Head
        std::pair(BoneIdx::Head, BoneIdx::Neck),
        std::pair(BoneIdx::Neck, BoneIdx::Chest),
        std::pair(BoneIdx::Chest, BoneIdx::Pelvis),

        // Left Arm
        std::pair(BoneIdx::Chest, BoneIdx::L_Shoulder),
        std::pair(BoneIdx::L_Shoulder, BoneIdx::L_Elbow),
        std::pair(BoneIdx::L_Elbow, BoneIdx::L_Hand),

        // Right Arm
        std::pair(BoneIdx::Chest, BoneIdx::R_Shoulder),
        std::pair(BoneIdx::R_Shoulder, BoneIdx::R_Elbow),
        std::pair(BoneIdx::R_Elbow, BoneIdx::R_Hand),

        // Left Leg
        //std::pair(BoneIdx::Pelvis, BoneIdx::L_Hip),
        std::pair(BoneIdx::Pelvis, BoneIdx::L_Thigh),
        //std::pair(BoneIdx::L_Hip, BoneIdx::L_Thigh),
        std::pair(BoneIdx::L_Thigh, BoneIdx::L_Knee),
        std::pair(BoneIdx::L_Knee, BoneIdx::L_Foot),

        // Right Leg
        //std::pair(BoneIdx::Pelvis, BoneIdx::R_Hip),
        std::pair(BoneIdx::Pelvis, BoneIdx::R_Thigh),
        //std::pair(BoneIdx::R_Hip, BoneIdx::R_Thigh),
        std::pair(BoneIdx::R_Thigh, BoneIdx::R_Knee),
        std::pair(BoneIdx::R_Knee, BoneIdx::R_Foot)
    };
    inline std::vector<PlayerCache> Players;
}
