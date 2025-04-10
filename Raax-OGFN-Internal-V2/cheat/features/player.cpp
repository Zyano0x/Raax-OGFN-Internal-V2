#include "player.h"
#include <cheat/sdk/Engine.h>
#include <drawing/drawing.h>

SDK::FName GetBoneNameFromIdx(Features::Player::BoneIdx Bone) {
    static SDK::FName Head = SDK::FName("head");
    static SDK::FName Neck = SDK::FName("neck_02");
    static SDK::FName Pelvis = SDK::FName("pelvis");

    static SDK::FName L_Shoulder = SDK::FName("upperarm_l");
    static SDK::FName L_Elbow = SDK::FName("lowerarm_l");
    static SDK::FName L_Hand = SDK::FName("hand_l");
    static SDK::FName R_Shoulder = SDK::FName("upperarm_r");
    static SDK::FName R_Elbow = SDK::FName("lowerarm_r");
    static SDK::FName R_Hand = SDK::FName("hand_r");

    static SDK::FName L_Thigh = SDK::FName("thigh_l");
    static SDK::FName L_Knee = SDK::FName("calf_l");
    static SDK::FName L_Foot = SDK::FName("ik_foot_l");
    static SDK::FName R_Thigh = SDK::FName("thigh_r");
    static SDK::FName R_Knee = SDK::FName("calf_r");
    static SDK::FName R_Foot = SDK::FName("ik_foot_r");

    switch (Bone)
    {
    case Features::Player::BoneIdx::Head: return Head;
    case Features::Player::BoneIdx::Neck: return Neck;
    case Features::Player::BoneIdx::Pelvis: return Pelvis;

    case Features::Player::BoneIdx::L_Shoulder: return L_Shoulder;
    case Features::Player::BoneIdx::L_Elbow: return L_Elbow;
    case Features::Player::BoneIdx::L_Hand: return L_Hand;

    case Features::Player::BoneIdx::R_Shoulder: return R_Shoulder;
    case Features::Player::BoneIdx::R_Elbow: return R_Elbow;
    case Features::Player::BoneIdx::R_Hand: return R_Hand;

    case Features::Player::BoneIdx::L_Thigh: return L_Thigh;
    case Features::Player::BoneIdx::L_Knee: return L_Knee;
    case Features::Player::BoneIdx::L_Foot: return L_Foot;

    case Features::Player::BoneIdx::R_Thigh: return R_Thigh;
    case Features::Player::BoneIdx::R_Knee: return R_Knee;
    case Features::Player::BoneIdx::R_Foot: return R_Foot;

    default: return SDK::FName();
    }
}

void Features::Player::PopulateBones(PlayerCache& Cache) {
    if (!Cache.Mesh)
        return;

    for (int i = 0; i < (int)BoneIdx::NUM; i++) {
        if (i == (int)BoneIdx::Chest)
            continue;

        int32_t Bone = Cache.Mesh->GetBoneIndex(GetBoneNameFromIdx((BoneIdx)i));
        Cache.BonePos[i] = Cache.Mesh->GetBoneLocation(Bone);
        Cache.BonePos2D[i] = SDK::Project(Cache.BonePos[i]);
    }

    SDK::FVector Chest = (Cache.BonePos[(int)BoneIdx::L_Shoulder] + Cache.BonePos[(int)BoneIdx::R_Shoulder]) / 2.f;
    Cache.BonePos[(int)BoneIdx::Chest] = Chest;
    Cache.BonePos2D[(int)BoneIdx::Chest] = SDK::Project(Chest);
}

void Features::Player::UpdateCache() {
    Players.clear();

    const std::vector<SDK::AFortPawn*> Pawns = SDK::GetAllActorsOfClass<SDK::AFortPawn>();
    for (const auto& Pawn : Pawns) {
        PlayerCache Cache;
        Cache.Pawn = Pawn;
        Cache.Mesh = Pawn->Mesh();
        PopulateBones(Cache);

        Players.push_back(Cache);
    }
}


void Features::Player::Tick() {
    UpdateCache();

    for (const auto& Player : Players) {
        for (const auto& Pair : BonePairs) {
            Drawing::Line(Player.BonePos2D[(int)Pair.first], Player.BonePos2D[(int)Pair.second]);
        }
    }
}
