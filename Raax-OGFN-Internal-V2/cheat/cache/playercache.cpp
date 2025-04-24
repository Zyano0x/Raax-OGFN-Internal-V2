#include "playercache.h"
#include <config/config.h>
#include <chrono>

std::unordered_map<void*, Cache::Player::PlayerInfo> g_CachedPlayers;

const std::unordered_map<void*, Cache::Player::PlayerInfo>& Cache::Player::GetCachedPlayers() {
    return g_CachedPlayers;
}


SDK::FName GetBoneNameFromIdx(Cache::Player::BoneIdx Idx) {
    static const std::unordered_map<Cache::Player::BoneIdx, SDK::FName> BoneMap = {
        {Cache::Player::BoneIdx::Head, "head"},
        {Cache::Player::BoneIdx::Neck, "neck_02"},
        {Cache::Player::BoneIdx::Pelvis, "pelvis"},

        {Cache::Player::BoneIdx::L_Shoulder, "upperarm_l"},
        {Cache::Player::BoneIdx::L_Elbow, "lowerarm_l"},
        {Cache::Player::BoneIdx::L_Hand, "hand_l"},
        {Cache::Player::BoneIdx::R_Shoulder, "upperarm_r"},
        {Cache::Player::BoneIdx::R_Elbow, "lowerarm_r"},
        {Cache::Player::BoneIdx::R_Hand, "hand_r"},

        {Cache::Player::BoneIdx::L_Thigh, "thigh_l"},
        {Cache::Player::BoneIdx::L_Knee, "calf_l"},
        {Cache::Player::BoneIdx::L_Foot, "ik_foot_l"},
        {Cache::Player::BoneIdx::R_Thigh, "thigh_r"},
        {Cache::Player::BoneIdx::R_Knee, "calf_r"},
        {Cache::Player::BoneIdx::R_Foot, "ik_foot_r"},
    };

    auto it = BoneMap.find(Idx);
    return (it != BoneMap.end()) ? it->second : SDK::FName();
}

void PopulateBones(Cache::Player::PlayerInfo& Cache) {
    if (!Cache.Mesh)
        return;

    for (int i = 0; i < Cache::Player::NumBones; i++) {
        if (i == static_cast<int>(Cache::Player::BoneIdx::Chest))
            continue;

        int32_t Bone = Cache.BoneIndicies[i];
        Cache.BoneWorldPos[i] = Cache.Mesh->GetBoneLocation(Bone);
        Cache.BoneScreenPos[i] = SDK::Project(Cache.BoneWorldPos[i]);
    }

    SDK::FVector Chest =
        (Cache.BoneWorldPos[static_cast<int>(Cache::Player::BoneIdx::L_Shoulder)] +
        Cache.BoneWorldPos[static_cast<int>(Cache::Player::BoneIdx::R_Shoulder)])
        / 2.f;

    Cache.BoneWorldPos[static_cast<int>(Cache::Player::BoneIdx::Chest)] = Chest;
    Cache.BoneScreenPos[static_cast<int>(Cache::Player::BoneIdx::Chest)] = SDK::Project(Chest);
}

void PopulateDrawingInfo(Cache::Player::PlayerInfo& Cache) {
    // Calculate the bounds for a 3D box and then project them to 2D
    SDK::FVector MostLeft = SDK::FVector(FLT_MAX, FLT_MAX, FLT_MAX);
    SDK::FVector MostRight = SDK::FVector(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    SDK::FVector MostTop = SDK::FVector(FLT_MAX, FLT_MAX, FLT_MAX);
    SDK::FVector MostBottom = SDK::FVector(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    for (int i = 0; i < Cache::Player::NumBones; i++) {
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

    Cache.BoundCorners3D[0] = SDK::FVector(MostLeft.X - HorizontalMargin,  MostTop.Y - VerticalMargin,    MostBottom.Z + DepthMargin);     // Bottom Left Front
    Cache.BoundCorners3D[1] = SDK::FVector(MostRight.X + HorizontalMargin, MostTop.Y - VerticalMargin,    MostBottom.Z + DepthMargin);     // Bottom Right Front
    Cache.BoundCorners3D[2] = SDK::FVector(MostLeft.X - HorizontalMargin,  MostBottom.Y + VerticalMargin, MostBottom.Z + DepthMargin);     // Bottom Left Back
    Cache.BoundCorners3D[3] = SDK::FVector(MostRight.X + HorizontalMargin, MostBottom.Y + VerticalMargin, MostBottom.Z + DepthMargin);     // Bottom Right Back
    Cache.BoundCorners3D[4] = SDK::FVector(MostLeft.X - HorizontalMargin,  MostTop.Y - VerticalMargin,    MostTop.Z - DepthMargin);       // Top Left Front
    Cache.BoundCorners3D[5] = SDK::FVector(MostRight.X + HorizontalMargin, MostTop.Y - VerticalMargin,    MostTop.Z - DepthMargin);       // Top Right Front
    Cache.BoundCorners3D[6] = SDK::FVector(MostLeft.X - HorizontalMargin,  MostBottom.Y + VerticalMargin, MostTop.Z - DepthMargin);       // Top Left Back
    Cache.BoundCorners3D[7] = SDK::FVector(MostRight.X + HorizontalMargin, MostBottom.Y + VerticalMargin, MostTop.Z - DepthMargin);       // Top Right Back

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

    float BoxSizeX =  Cache.BoundBottomRight.X -  Cache.BoundTopLeft.X;
    float BoxSizeY =  Cache.BoundBottomRight.Y -  Cache.BoundTopLeft.Y;

    if (Config::g_Config.Visuals.Player.BoxType == Config::ConfigData::BoxType::Full3D) {
        Cache.BoxTop = SDK::Project((Cache.BoundCorners3D[0] + Cache.BoundCorners3D[1] + Cache.BoundCorners3D[2] + Cache.BoundCorners3D[3]) / 4.f);
        Cache.BoxMiddle = SDK::Project((Cache.BoundCorners3D[0] + Cache.BoundCorners3D[1] + Cache.BoundCorners3D[2] + Cache.BoundCorners3D[3] +
            Cache.BoundCorners3D[4] + Cache.BoundCorners3D[5] + Cache.BoundCorners3D[6] + Cache.BoundCorners3D[7]) / 8.f);
        Cache.BoxBottom = SDK::Project((Cache.BoundCorners3D[4] + Cache.BoundCorners3D[5] + Cache.BoundCorners3D[6] + Cache.BoundCorners3D[7]) / 4.f);
    }
    else {
        Cache.BoxTop = SDK::FVector2D(Cache.BoundTopLeft.X + (BoxSizeX / 2.f), Cache.BoundTopLeft.Y);
        Cache.BoxMiddle = SDK::FVector2D(Cache.BoundTopLeft.X + (BoxSizeX / 2.f), Cache.BoundTopLeft.Y + (BoxSizeY / 2.f));
        Cache.BoxBottom = SDK::FVector2D(Cache.BoundTopLeft.X + (BoxSizeX / 2.f), Cache.BoundBottomRight.Y);
    }
    Cache.FontSize = 12.f;
}


void ForceRefreshIfNeeded() {
    // Force refresh cache every 5 seconds for player name changes, skin changes, etc
    // as these will invalidate some cached player info
    static auto CacheTime = std::chrono::steady_clock::now();
    auto CurrentTime = std::chrono::steady_clock::now();
    auto Elapsed = std::chrono::duration_cast<std::chrono::seconds>(CurrentTime - CacheTime).count();
    if (Elapsed >= 5) {
        CacheTime = CurrentTime;
        g_CachedPlayers.clear();
    }
}

void ResetPlayerSeenFlags() {
    for (auto& [Pawn, Cache] : g_CachedPlayers) {
        Cache.WasSeenThisFrame = false;
    }
}

Cache::Player::PlayerInfo CreateNewPlayerInfo(SDK::AFortPawn* Pawn) {
    Cache::Player::PlayerInfo Info;
    Info.Pawn = Pawn;
    Info.PlayerState = Pawn->PlayerState();
    Info.Mesh = Pawn->Mesh();
    Info.PlayerName = Info.PlayerState->GetPlayerName().ToString();
    for (int i = 0; i < Cache::Player::NumBones; i++) {
        if (i == static_cast<int>(Cache::Player::BoneIdx::Chest))
            continue;

        int32_t Bone = Info.Mesh->GetBoneIndex(GetBoneNameFromIdx(static_cast<Cache::Player::BoneIdx>(i)));
        Info.BoneIndicies[i] = Bone;
    }
    Info.RootLocation = Pawn->RootComponent()->RelativeLocation();
    Info.WasSeenThisFrame = true;

    PopulateBones(Info);
    PopulateDrawingInfo(Info);
    return Info;
}

void UpdateExistingPlayerInfo(Cache::Player::PlayerInfo& Info, SDK::AFortPawn* Pawn) {
    Info.Pawn = Pawn;
    Info.PlayerState = Pawn->PlayerState();
    Info.Mesh = Pawn->Mesh();
    Info.RootLocation = Pawn->RootComponent()->RelativeLocation();
    Info.WasSeenThisFrame = true;

    PopulateBones(Info);
    PopulateDrawingInfo(Info);
}

void RemoveUnseenPlayers() {
    for (auto it = g_CachedPlayers.begin(); it != g_CachedPlayers.end(); ) {
        if (!it->second.WasSeenThisFrame)
            it = g_CachedPlayers.erase(it);
        else
            ++it;
    }
}


void Cache::Player::UpdateCache() {
    ForceRefreshIfNeeded();

    ResetPlayerSeenFlags();
    static std::vector<SDK::AFortPawn*> Players;
    SDK::GetAllActorsOfClass<SDK::AFortPawn>(Players);
    for (const auto& Pawn : Players) {
        auto it = g_CachedPlayers.find(Pawn);
        if (it == g_CachedPlayers.end())
            g_CachedPlayers[Pawn] = CreateNewPlayerInfo(Pawn);
        else
            UpdateExistingPlayerInfo(it->second, Pawn);
    }
    RemoveUnseenPlayers();
}
