#pragma once
#include <Windows.h>
#include <string>
#include <cheat/sdk/Basic.h>

namespace Config
{
    struct ConfigData {
        enum class BoxType : int {
            Full = 0,
            Cornered = 1,
            Full3D = 2,
        };
        enum class TracerPos : int {
            Top = 0,
            Middle = 1,
            Bottom = 2
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

static_assert(std::is_same_v<std::underlying_type_t<Config::ConfigData::BoxType>, int>, "BoxType needs to have underlying type of int for config loading and editing to be safe!");
static_assert(std::is_same_v<std::underlying_type_t<Config::ConfigData::TracerPos>, int>, "TracerPos needs to have underlying type of int for config loading and editing to be safe!");
