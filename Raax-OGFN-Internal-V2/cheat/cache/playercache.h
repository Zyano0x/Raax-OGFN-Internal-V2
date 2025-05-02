#pragma once

#include <unordered_map>
#include <cheat/sdk/sdk.h>

namespace Cache {
namespace Player {

// --- Cache Structures ----------------------------------------------

enum class BoneIdx : int {
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

    L_Thigh,
    L_Knee,
    L_Foot,
    R_Thigh,
    R_Knee,
    R_Foot,

    NUM
};

struct PlayerInfo {
    SDK::AFortPawn*              Pawn = nullptr;
    SDK::APlayerState*           PlayerState = nullptr;
    SDK::USkeletalMeshComponent* Mesh = nullptr;
    int32_t                      BoneIndicies[(int)BoneIdx::NUM];
    SDK::FVector                 RootWorldLocation;
    SDK::FVector                 BoneWorldPos[(int)BoneIdx::NUM];
    SDK::FVector2D               BoneScreenPos[(int)BoneIdx::NUM];
    SDK::FVector2D               BoundTopLeft, BoundBottomRight;
    SDK::FVector                 BoundCorners3D[8];
    SDK::FVector2D               BoundCorners2D[8];
    SDK::FVector2D               BoxTop, BoxMiddle, BoxBottom;
    std::string                  PlayerName;
    bool                         SeenThisFrame = false;
};

// --- Public Cache Functions ----------------------------------------

const std::unordered_map<void*, PlayerInfo>& GetCachedPlayers();
void                                         UpdateCache();

} // namespace Player
} // namespace Cache
