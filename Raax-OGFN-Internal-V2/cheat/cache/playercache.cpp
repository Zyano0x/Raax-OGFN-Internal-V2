#include "playercache.h"
#include <chrono>

#include <cheat/core.h>
#include <config/config.h>
#include <utils/math.h>

namespace Cache {
namespace Player {

// --- Cache Data ----------------------------------------------------

std::unordered_map<void*, PlayerInfo> CachedPlayers;

// --- Cache Utility Functions ---------------------------------------

SDK::FName GetBoneNameFromIdx(BoneIdx Idx) {
    static const std::unordered_map<BoneIdx, SDK::FName> BoneMap = {
        {BoneIdx::Head, "head"},
        {BoneIdx::Neck, "neck_01"},
        {BoneIdx::Pelvis, "pelvis"},

        {BoneIdx::L_Shoulder, "upperarm_l"},
        {BoneIdx::L_Elbow, "lowerarm_l"},
        {BoneIdx::L_Hand, "hand_l"},
        {BoneIdx::R_Shoulder, "upperarm_r"},
        {BoneIdx::R_Elbow, "lowerarm_r"},
        {BoneIdx::R_Hand, "hand_r"},

        {BoneIdx::L_Thigh, "thigh_l"},
        {BoneIdx::L_Knee, "calf_l"},
        {BoneIdx::L_Foot, "ik_foot_l"},
        {BoneIdx::R_Thigh, "thigh_r"},
        {BoneIdx::R_Knee, "calf_r"},
        {BoneIdx::R_Foot, "ik_foot_r"},
    };

    auto It = BoneMap.find(Idx);
    return (It != BoneMap.end()) ? It->second : SDK::FName();
}

void PopulateBones(PlayerInfo& Cache) {
    if (!Cache.Mesh)
        return;

    for (int i = 0; i < static_cast<int>(BoneIdx::NUM); i++) {
        if (i == static_cast<int>(BoneIdx::Chest))
            continue;

        int32_t Bone = Cache.BoneIndicies[i];
        Cache.BoneWorldPos[i] = Cache.Mesh->GetBoneLocation(Bone);
        Cache.BoneScreenPos[i] = SDK::Project(Cache.BoneWorldPos[i]);
    }

    SDK::FVector Chest = (Cache.BoneWorldPos[static_cast<int>(BoneIdx::L_Shoulder)] +
                          Cache.BoneWorldPos[static_cast<int>(BoneIdx::R_Shoulder)]) /
                         2.f;

    Cache.BoneWorldPos[static_cast<int>(BoneIdx::Chest)] = Chest;
    Cache.BoneScreenPos[static_cast<int>(BoneIdx::Chest)] = SDK::Project(Chest);
}

void PopulateDrawingInfo(PlayerInfo& Cache) {
    // Calculate the bounds for a 3D box and then project them to 2D
    SDK::FVector MostLeft = SDK::FVector(FLT_MAX, FLT_MAX, FLT_MAX);
    SDK::FVector MostRight = SDK::FVector(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    SDK::FVector MostTop = SDK::FVector(FLT_MAX, FLT_MAX, FLT_MAX);
    SDK::FVector MostBottom = SDK::FVector(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    for (int i = 0; i < static_cast<int>(BoneIdx::NUM); i++) {
        SDK::FVector BoneLocation = Cache.BoneWorldPos[i];

        MostLeft.X = std::min(MostLeft.X, BoneLocation.X);
        MostRight.X = std::max(MostRight.X, BoneLocation.X);

        MostTop.Y = std::min(MostTop.Y, BoneLocation.Y);
        MostBottom.Y = std::max(MostBottom.Y, BoneLocation.Y);

        MostTop.Z = std::min(MostTop.Z, BoneLocation.Z);
        MostBottom.Z = std::max(MostBottom.Z, BoneLocation.Z);
    }

    // Get the size of the box horizontally and vertically
    SDK::FVector BoxSizeHorizontal = MostRight - MostLeft;
    SDK::FVector BoxSizeVertical = MostBottom - MostTop;

    // Create the 8 corners of the box in 3D space, extending them by a 25% margin
    float HorizontalMargin = BoxSizeHorizontal.X * 0.2f;
    float VerticalMargin = BoxSizeVertical.Y * 0.2f;
    float DepthMargin = BoxSizeVertical.Z * 0.125f;

    Cache.BoundCorners3D[0] = SDK::FVector(MostLeft.X - HorizontalMargin, MostTop.Y - VerticalMargin,
                                           MostBottom.Z + DepthMargin); // Bottom Left Front
    Cache.BoundCorners3D[1] = SDK::FVector(MostRight.X + HorizontalMargin, MostTop.Y - VerticalMargin,
                                           MostBottom.Z + DepthMargin); // Bottom Right Front
    Cache.BoundCorners3D[2] = SDK::FVector(MostLeft.X - HorizontalMargin, MostBottom.Y + VerticalMargin,
                                           MostBottom.Z + DepthMargin); // Bottom Left Back
    Cache.BoundCorners3D[3] = SDK::FVector(MostRight.X + HorizontalMargin, MostBottom.Y + VerticalMargin,
                                           MostBottom.Z + DepthMargin); // Bottom Right Back
    Cache.BoundCorners3D[4] = SDK::FVector(MostLeft.X - HorizontalMargin, MostTop.Y - VerticalMargin,
                                           MostTop.Z - DepthMargin); // Top Left Front
    Cache.BoundCorners3D[5] = SDK::FVector(MostRight.X + HorizontalMargin, MostTop.Y - VerticalMargin,
                                           MostTop.Z - DepthMargin); // Top Right Front
    Cache.BoundCorners3D[6] = SDK::FVector(MostLeft.X - HorizontalMargin, MostBottom.Y + VerticalMargin,
                                           MostTop.Z - DepthMargin); // Top Left Back
    Cache.BoundCorners3D[7] = SDK::FVector(MostRight.X + HorizontalMargin, MostBottom.Y + VerticalMargin,
                                           MostTop.Z - DepthMargin); // Top Right Back

    for (int i = 0; i < 8; i++)
        Cache.BoundCorners2D[i] = Project(Cache.BoundCorners3D[i]);

    // Find the most top left and bottom right corners of the box
    Cache.BoundTopLeft = SDK::FVector2D(FLT_MAX, FLT_MAX);
    Cache.BoundBottomRight = SDK::FVector2D(-FLT_MAX, -FLT_MAX);

    for (int i = 0; i < 8; i++) {
        SDK::FVector2D& Corner = Cache.BoundCorners2D[i];

        Cache.BoundTopLeft.X = std::min(Cache.BoundTopLeft.X, Corner.X);
        Cache.BoundTopLeft.Y = std::min(Cache.BoundTopLeft.Y, Corner.Y);

        Cache.BoundBottomRight.X = std::max(Cache.BoundBottomRight.X, Corner.X);
        Cache.BoundBottomRight.Y = std::max(Cache.BoundBottomRight.Y, Corner.Y);
    }

    float BoxSizeX = Cache.BoundBottomRight.X - Cache.BoundTopLeft.X;
    float BoxSizeY = Cache.BoundBottomRight.Y - Cache.BoundTopLeft.Y;

    if (Config::g_Config.Visuals.Player.BoxType == Config::ConfigData::BoxType::Full3D) {
        Cache.BoxTop = SDK::Project(
            (Cache.BoundCorners3D[0] + Cache.BoundCorners3D[1] + Cache.BoundCorners3D[2] + Cache.BoundCorners3D[3]) /
            4.f);
        Cache.BoxMiddle = SDK::Project((Cache.BoundCorners3D[0] + Cache.BoundCorners3D[1] + Cache.BoundCorners3D[2] +
                                        Cache.BoundCorners3D[3] + Cache.BoundCorners3D[4] + Cache.BoundCorners3D[5] +
                                        Cache.BoundCorners3D[6] + Cache.BoundCorners3D[7]) /
                                       8.f);
        Cache.BoxBottom = SDK::Project(
            (Cache.BoundCorners3D[4] + Cache.BoundCorners3D[5] + Cache.BoundCorners3D[6] + Cache.BoundCorners3D[7]) /
            4.f);
    } else {
        Cache.BoxTop = SDK::FVector2D(Cache.BoundTopLeft.X + (BoxSizeX / 2.f), Cache.BoundTopLeft.Y);
        Cache.BoxMiddle =
            SDK::FVector2D(Cache.BoundTopLeft.X + (BoxSizeX / 2.f), Cache.BoundTopLeft.Y + (BoxSizeY / 2.f));
        Cache.BoxBottom = SDK::FVector2D(Cache.BoundTopLeft.X + (BoxSizeX / 2.f), Cache.BoundBottomRight.Y);
    }
}

void ForceRefreshIfNeeded() {
    // Force refresh cache every 5 seconds for player name changes, skin changes, etc
    // as these will invalidate some cached player info
    static std::chrono::time_point<std::chrono::high_resolution_clock> LastCheckTime;
    auto                                                               Now = std::chrono::steady_clock::now();
    auto Elapsed = std::chrono::duration_cast<std::chrono::seconds>(Now - LastCheckTime).count();
    if (Elapsed >= 5) {
        LastCheckTime = Now;
        CachedPlayers.clear();
    }
}

void SharedInfoUpdate(PlayerInfo& Info) {
    SDK::AFortWeapon* NewWeapon = Info.Pawn->CurrentWeapon();
    if (NewWeapon) {
        if (NewWeapon != Info.CurrentWeapon) {
            Info.WeaponName = NewWeapon->WeaponData()->DisplayName()->ToString();
            Info.WeaponTier = NewWeapon->WeaponData()->Tier();
            Info.BulletsPerClip = NewWeapon->GetBulletsPerClip();
        }
        Info.AmmoCount = NewWeapon->AmmoCount();
    } else {
        Info.WeaponName = "";
        Info.AmmoCount = 0;
        Info.BulletsPerClip = 0;
    }
    Info.CurrentWeapon = NewWeapon;

    Info.RootWorldLocation = Info.Pawn->RootComponent()->RelativeLocation();
    Info.DistanceM = Core::g_CameraLocation.Dist(Info.RootWorldLocation) / 100.f;
    Info.SeenThisFrame = true;

    PopulateBones(Info);
    PopulateDrawingInfo(Info);

    for (int i = 0; i < sizeof(Info.BoundCorners2D) / sizeof(*Info.BoundCorners2D); i++) {
        if (Math::IsOnScreen(Info.BoundCorners2D[i])) {
            Info.IsOnScreen = true;
            break;
        }
    }

    // Small optimisation. Checking for head visible with a line trace took 0.7% CPU time, adding this quick check
    // allows us avoid unnecassary line traces. This brought CPU time down to 0.2% for non visible targets on srceen.
    if (Info.IsOnScreen && !Info.Pawn->WasRecentlyRendered(0.1f)) {
        Info.HeadVisible = false;
    } else {
        Info.HeadVisible =
            SDK::IsPositionVisible(Info.BoneWorldPos[static_cast<int>(BoneIdx::Head)], Info.Pawn, SDK::GetLocalPawn());
    }
}

PlayerInfo CreateNewPlayerInfo(SDK::AFortPawn* Pawn) {
    PlayerInfo Info;
    Info.Pawn = Pawn;
    Info.PlayerState = SDK::Cast<SDK::AFortPlayerState, true>(Info.Pawn->PlayerState());
    Info.Mesh = Info.Pawn->Mesh();

    Info.PlayerName = Info.PlayerState->GetPlayerName().ToString();
    Info.Platform = Info.PlayerState->Platform().ToString();
    for (int i = 0; i < static_cast<int>(BoneIdx::NUM); i++) {
        if (i == static_cast<int>(BoneIdx::Chest))
            continue;

        int32_t Bone = Info.Mesh->GetBoneIndex(GetBoneNameFromIdx(static_cast<BoneIdx>(i)));
        Info.BoneIndicies[i] = Bone;
    }

    SharedInfoUpdate(Info);
    return Info;
}

void UpdateExistingPlayerInfo(PlayerInfo& Info, SDK::AFortPawn* Pawn) {
    Info.Pawn = Pawn;
    Info.PlayerState = SDK::Cast<SDK::AFortPlayerState, true>(Info.Pawn->PlayerState());
    Info.Mesh = Info.Pawn->Mesh();
    SharedInfoUpdate(Info);
}

void ResetPlayerSeenFlags() {
    for (auto& [_, Cache] : CachedPlayers) {
        Cache.SeenThisFrame = false;
    }
}

void RemoveUnseenPlayers() {
    std::erase_if(CachedPlayers, [](const auto& Cache) { return !Cache.second.SeenThisFrame; });
}

// --- Public Cache Functions ----------------------------------------

const std::unordered_map<void*, PlayerInfo>& GetCachedPlayers() {
    return CachedPlayers;
}

void UpdateCache() {
    ForceRefreshIfNeeded();
    ResetPlayerSeenFlags();

    static std::vector<SDK::AFortPawn*> PlayerList;
    SDK::GetAllActorsOfClass<SDK::AFortPawn>(PlayerList);
    for (const auto& Player : PlayerList) {
        auto It = CachedPlayers.find(Player);
        if (It == CachedPlayers.end()) {
            CachedPlayers[Player] = CreateNewPlayerInfo(Player);
        } else {
            UpdateExistingPlayerInfo(It->second, Player);
        }
    }

    RemoveUnseenPlayers();
}

} // namespace Player
} // namespace Cache
