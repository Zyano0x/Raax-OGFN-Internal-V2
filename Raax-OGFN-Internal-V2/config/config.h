#pragma once

#include "keybind.h"
#include <string>
#include <format>

#include <cheat/sdk/Basic.h>
#include <extern/imgui/imgui.h>

namespace Config {

// --- Config Structs & Data -----------------------------------------

struct ConfigData {
    enum class BoxType : int { MIN = 0, Full = 0, Cornered = 1, Full3D = 2, MAX = 2 };
    enum class TracerPos : int { MIN = 0, Top = 0, Middle = 1, Bottom = 2, MAX = 2 };
    enum class Tier : int { MIN = 0, Common = 0, Uncommon = 1, Rare = 2, Epic = 3, Legendary = 4, Mythic = 5, MAX = 5 };
    enum class TargetSelection : int { MIN = 0, Distance = 0, Degrees = 1, Combined = 2, MAX = 2 };
    enum class TargetBone : int { MIN = 0, Head = 0, Neck = 1, Chest = 2, Pelvis = 3, Random = 4, MAX = 4 };

    struct VisualsConfig {
        struct PlayerConfig {
            float MaxDistance = 300.f;

            bool    Box = true;
            BoxType BoxType = BoxType::Cornered;
            float   BoxThickness = 2.f;

            bool              FilledBox = true;
            SDK::FLinearColor FilledBoxColor = SDK::FLinearColor(0.f, 0.f, 0.f, 0.25f);

            bool  Skeleton = true;
            float SkeletonThickness = 1.f;

            bool      Tracer = true;
            float     TracerThickness = 1.f;
            TracerPos TracerStart = TracerPos::Bottom;
            TracerPos TracerEnd = TracerPos::Bottom;

            bool Platform = true;
            bool Name = true;
            bool CurrentWeapon = true;
            bool Distance = true;

            bool  OSI = false;
            bool  OSIMatchFOV = true;
            float OSIFOV = 10.f;
            float OSISize = 8.f;
        } Player;

        struct RadarConfig {
            bool              Radar = true;
            bool              RotateWithCamera = true;
            bool              ShowCameraFOV = true;
            float             MaxDistance = 150.f;
            float             PosX = 1.f;
            float             PosY = 1.f;
            float             Size = 10.f;
            SDK::FLinearColor ColorVisible = SDK::FLinearColor(1.f, 0.f, 0.f, 1.f);
            SDK::FLinearColor ColorHidden = SDK::FLinearColor(0.43f, 0.f, 0.f, 1.f);
        } Radar;

        struct LootConfig {
            bool  LootText = false;
            float LootMaxDistance = 300.f;
            Tier  MinLootTier = Tier::Epic;

            bool  ChestText = false;
            float ChestMaxDistance = 300.f;

            bool  AmmoBoxText = false;
            float AmmoBoxMaxDistance = 300.f;
        } Loot;
    } Visuals;

    struct AimbotConfig {
        struct AimbotAmmoConfig {
            bool  Enabled = true;
            bool  UseInTriggerBot = true;
            float Smoothness = 5.f;
            float MaxDistance = 300.f;

            bool VisibleCheck = true;
            bool StickyTarget = true;

            bool  ShowFOV = true;
            float FOV = 10.f;

            bool  UseDeadzone = true;
            bool  ShowDeadzoneFOV = true;
            float DeadzoneFOV = 2.f;

            TargetSelection Selection = TargetSelection::Combined;
            TargetBone      Bone = TargetBone::Head;

            float RandomBoneRefreshRate = 1.f;
        };

        AimbotAmmoConfig ShellsAimbot = {.Enabled = true,
                                         .Smoothness = 1.5f,
                                         .MaxDistance = 25.f,

                                         .VisibleCheck = true,
                                         .StickyTarget = true,

                                         .ShowFOV = true,
                                         .FOV = 7.5f,

                                         .UseDeadzone = true,
                                         .ShowDeadzoneFOV = true,
                                         .DeadzoneFOV = 0.5f,

                                         .Selection = TargetSelection::Combined,
                                         .Bone = TargetBone::Head,

                                         .RandomBoneRefreshRate = 1.f};
        AimbotAmmoConfig LightAimbot = {.Enabled = true,
                                        .Smoothness = 3.f,
                                        .MaxDistance = 75.f,

                                        .VisibleCheck = true,
                                        .StickyTarget = true,

                                        .ShowFOV = true,
                                        .FOV = 8.5f,

                                        .UseDeadzone = true,
                                        .ShowDeadzoneFOV = true,
                                        .DeadzoneFOV = 0.6f,

                                        .Selection = TargetSelection::Combined,
                                        .Bone = TargetBone::Neck,

                                        .RandomBoneRefreshRate = 1.f};
        AimbotAmmoConfig MediumAimbot = {.Enabled = true,
                                         .Smoothness = 7.5f,
                                         .MaxDistance = 300.f,

                                         .VisibleCheck = true,
                                         .StickyTarget = true,

                                         .ShowFOV = true,
                                         .FOV = 10.f,

                                         .UseDeadzone = true,
                                         .ShowDeadzoneFOV = true,
                                         .DeadzoneFOV = 1.f,

                                         .Selection = TargetSelection::Combined,
                                         .Bone = TargetBone::Random,

                                         .RandomBoneRefreshRate = 1.f};
        AimbotAmmoConfig HeavyAimbot = {.Enabled = true,
                                        .Smoothness = 3.f,
                                        .MaxDistance = 300.f,

                                        .VisibleCheck = true,
                                        .StickyTarget = true,

                                        .ShowFOV = true,
                                        .FOV = 10.f,

                                        .UseDeadzone = false,
                                        .ShowDeadzoneFOV = true,
                                        .DeadzoneFOV = 2.f,

                                        .Selection = TargetSelection::Combined,
                                        .Bone = TargetBone::Head,

                                        .RandomBoneRefreshRate = 1.f};
        AimbotAmmoConfig OtherAimbot = {};

        AimbotAmmoConfig AllAimbot = {};

        bool SplitAimbotByAmmo = true;
        bool BulletPrediction = true;
        int  AimbotKeybind = ImGuiKey_MouseRight;
    } Aimbot;

    struct TriggerBotConfig {
        bool  Enabled = false;
        bool  UseKeybind = false;
        int   Keybind = ImGuiKey_G;
        bool  ShowFOV = true;
        float FOV = 4.f;
        float MaxDistance = 25.f;
        float FireDelayS = 0.25f;
    } TriggerBot;

    struct KeybindConfig {
        std::vector<Keybind::KeybindData> Keybinds;
        std::string                       KeybindData;
    } Keybinds;

    struct ColorConfig {
        SDK::FLinearColor PrimaryColorVisible = SDK::FLinearColor(1.f, 0.f, 0.f, 1.f);
        SDK::FLinearColor PrimaryColorHidden = SDK::FLinearColor(0.43f, 0.f, 0.f, 1.f);
        SDK::FLinearColor SecondaryColorVisible = SDK::FLinearColor(0.f, 0.47f, 1.f, 1.f);
        SDK::FLinearColor SecondaryColorHidden = SDK::FLinearColor(0.f, 0.17f, 0.34f, 1.f);
    } Color;

    int MenuKeybind = ImGuiKey_Insert;

    std::string SerializeConfig(bool FullConfig);
    bool        DeserializeConfig(const std::string& Data);
};

inline ConfigData g_Config;

} // namespace Config

static_assert(std::is_same_v<std::underlying_type_t<Config::ConfigData::BoxType>, int>);
static_assert(std::is_same_v<std::underlying_type_t<Config::ConfigData::TracerPos>, int>);
static_assert(std::is_same_v<std::underlying_type_t<Config::ConfigData::Tier>, int>);
static_assert(std::is_same_v<std::underlying_type_t<Config::ConfigData::TargetSelection>, int>);
static_assert(std::is_same_v<std::underlying_type_t<Config::ConfigData::TargetBone>, int>);
