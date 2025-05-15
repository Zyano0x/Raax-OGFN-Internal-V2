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
    Chest,

    L_Shoulder,
    L_Elbow,
    L_Hand,
    R_Shoulder,
    R_Elbow,
    R_Hand,

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
    SDK::AFortPlayerState*       PlayerState = nullptr;
    SDK::USkeletalMeshComponent* Mesh = nullptr;
    SDK::AFortWeapon*            CurrentWeapon = nullptr;
    int32_t                      BoneIndicies[(int)BoneIdx::NUM];
    SDK::FVector                 RootWorldLocation;
    SDK::FVector                 BoneWorldPos[(int)BoneIdx::NUM];
    SDK::FVector2D               BoneScreenPos[(int)BoneIdx::NUM];
    SDK::FVector2D               BoundTopLeft, BoundBottomRight;
    SDK::FVector                 BoundCorners3D[8];
    SDK::FVector2D               BoundCorners2D[8];
    SDK::FVector2D               BoxTop, BoxMiddle, BoxBottom;
    std::string                  PlayerName;
    std::string                  Platform;
    std::string                  WeaponName;
    SDK::EFortItemTier           WeaponTier = SDK::EFortItemTier::No_Tier;
    int32_t                      AmmoCount = 0;
    int32_t                      BulletsPerClip = 0;
    float                        DistanceM = 0.f;
    bool                         HeadVisible = false;
    bool                         IsOnScreen = false;
    bool                         SeenThisFrame = false;
};

// --- Public Cache Functions ----------------------------------------

const std::unordered_map<void*, PlayerInfo>& GetCachedPlayers();
void                                         UpdateCache();

} // namespace Player
} // namespace Cache
