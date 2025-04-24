#pragma once
#include <string>
#include <cheat/sdk/Basic.h>

namespace Config
{
    struct ConfigData {
        enum class BoxType : int {
            MIN = 0,
            Full = 0,
            Cornered = 1,
            Full3D = 2,
            MAX = 2
        };
        enum class TracerPos : int {
            MIN = 0,
            Top = 0,
            Middle = 1,
            Bottom = 2,
            MAX = 2
        };
        enum class Tier : int {
            MIN = 1,
            Common = 1,
            Uncommon = 2,
            Rare = 3,
            Epic = 4,
            Legendary = 5,
            Mythic = 6,
            MAX = 6
        };

        struct Visuals {
            struct PlayerConfig {
                bool Box = true;
                BoxType BoxType = BoxType::Cornered;
                float BoxThickness = 2.f;

                bool Skeleton = true;
                float SkeletonThickness = 1.f;

                bool Tracer = true;
                float TracerThickness = 1.f;
                TracerPos TracerStart = TracerPos::Bottom;
                TracerPos TracerEnd = TracerPos::Bottom;

                bool Name = true;
            } Player;

            struct LootConfig {
                bool LootText = false;
                Tier MinLootTier = Tier::Epic;

                bool ChestText = false;
                bool AmmoBoxText = false;
            } Loot;
        } Visuals;

        struct AimbotConfig {
            bool Enabled = true;
            float Smoothness = 5.f;
            //int Keybind = VK_RBUTTON;
        } Aimbot;

        struct ColorConfig {
            SDK::FLinearColor PrimaryColorVisible = SDK::FLinearColor::White;
            SDK::FLinearColor PrimaryColorHidden = SDK::FLinearColor::Gray;
            SDK::FLinearColor SecondaryColorVisible = SDK::FLinearColor::White;
            SDK::FLinearColor SecondaryColorHidden = SDK::FLinearColor::Gray;
        } Color;

        std::string SerializeConfig();
        bool DeserializeConfig(const std::string& Data, ConfigData& MergeConfig);
    };

    inline ConfigData g_Config;
}

static_assert(std::is_same_v<std::underlying_type_t<Config::ConfigData::BoxType>, int>);
static_assert(std::is_same_v<std::underlying_type_t<Config::ConfigData::TracerPos>, int>);
static_assert(std::is_same_v<std::underlying_type_t<Config::ConfigData::Tier>, int>);
