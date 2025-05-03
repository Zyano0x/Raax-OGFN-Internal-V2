#pragma once

#include "config.h"
#include <tuple>
#include <string>

namespace ConfigReflection {

// --- Reflection Structs --------------------------------------------

template <typename T> struct TypeDescriptor {
    static constexpr std::string_view Name = "unknown";
};

template <> struct TypeDescriptor<bool> {
    static constexpr std::string_view Name = "bool";
};

template <> struct TypeDescriptor<float> {
    static constexpr std::string_view Name = "float";
};

template <> struct TypeDescriptor<int> {
    static constexpr std::string_view Name = "int";
};

template <> struct TypeDescriptor<SDK::FLinearColor> {
    static constexpr std::string_view Name = "FLinearColor";
};

template <typename Class, typename T> struct MemberDescriptor {
    std::string_view Name;
    T Class::*        Ptr;
    TypeDescriptor<T> Type;
};

struct ConfigFieldView {
    void*            Ptr;
    std::string_view Name;
    std::string_view Type;
    std::string      FullPath;

    template <typename T> T&   As() const { return *reinterpret_cast<T*>(Ptr); }
    template <typename T> bool Is() const { return Type == TypeDescriptor<T>::Name; }

    template <typename tParent, typename T>
    static void BuildPath(const tParent* Parent, const MemberDescriptor<tParent, T>& Member, std::string& CurrentPath) {
        if constexpr (requires { ConfigReflection::DescribeMembers<tParent>(); }) {
            CurrentPath += std::string(Member.Name) + ".";
            if constexpr (requires { ConfigReflection::DescribeMembers<T>(); }) {
                auto Nembers = ConfigReflection::DescribeMembers<T>();
                std::apply(
                    [&](auto&&... m) {
                    (BuildPath(static_cast<const T*>(&(Parent->*(Member.Ptr)), m, CurrentPath), ...);
                    },
                    Nembers);
            }
        } else {
            CurrentPath += std::string(Member.Name);
        }
    }
};

// --- Member Describers ---------------------------------------------

template <typename T> constexpr auto DescribeMembers();

template <> constexpr auto DescribeMembers<Config::ConfigData::ColorConfig>() {
    using T = Config::ConfigData::ColorConfig;
    return std::make_tuple(
        MemberDescriptor<T, SDK::FLinearColor>{"PrimaryColorVisible", &T::PrimaryColorVisible, {}},
        MemberDescriptor<T, SDK::FLinearColor>{"PrimaryColorHidden", &T::PrimaryColorHidden, {}},
        MemberDescriptor<T, SDK::FLinearColor>{"SecondaryColorVisible", &T::SecondaryColorVisible, {}},
        MemberDescriptor<T, SDK::FLinearColor>{"SecondaryColorHidden", &T::SecondaryColorHidden, {}});
}

template <> constexpr auto DescribeMembers<Config::ConfigData::VisualsConfig::PlayerConfig>() {
    using T = Config::ConfigData::VisualsConfig::PlayerConfig;
    return std::make_tuple(MemberDescriptor<T, bool>{"Box", &T::Box, {}},
                           MemberDescriptor<T, Config::ConfigData::BoxType>{"BoxType", &T::BoxType, {}},
                           MemberDescriptor<T, float>{"BoxThickness", &T::BoxThickness, {}},
                           MemberDescriptor<T, bool>{"FilledBox", &T::FilledBox, {}},
                           MemberDescriptor<T, SDK::FLinearColor>{"FilledBoxColor", &T::FilledBoxColor, {}},
                           MemberDescriptor<T, bool>{"Skeleton", &T::Skeleton, {}},
                           MemberDescriptor<T, float>{"SkeletonThickness", &T::SkeletonThickness, {}},
                           MemberDescriptor<T, bool>{"Tracer", &T::Tracer, {}},
                           MemberDescriptor<T, float>{"TracerThickness", &T::TracerThickness, {}},
                           MemberDescriptor<T, Config::ConfigData::TracerPos>{"TracerStart", &T::TracerStart, {}},
                           MemberDescriptor<T, Config::ConfigData::TracerPos>{"TracerEnd", &T::TracerEnd, {}},
                           MemberDescriptor<T, bool>{"Platform", &T::Platform, {}},
                           MemberDescriptor<T, bool>{"Name", &T::Name, {}},
                           MemberDescriptor<T, bool>{"CurrentWeapon", &T::CurrentWeapon, {}},
                           MemberDescriptor<T, bool>{"Distance", &T::Distance, {}});
}

template <> constexpr auto DescribeMembers<Config::ConfigData::VisualsConfig::LootConfig>() {
    using T = Config::ConfigData::VisualsConfig::LootConfig;
    return std::make_tuple(MemberDescriptor<T, bool>{"LootText", &T::LootText, {}},
                           MemberDescriptor<T, float>{"LootMaxDistance", &T::LootMaxDistance, {}},
                           MemberDescriptor<T, Config::ConfigData::Tier>{"MinLootTier", &T::MinLootTier, {}},
                           MemberDescriptor<T, bool>{"ChestText", &T::ChestText, {}},
                           MemberDescriptor<T, float>{"ChestMaxDistance", &T::ChestMaxDistance, {}},
                           MemberDescriptor<T, bool>{"AmmoBoxText", &T::AmmoBoxText, {}},
                           MemberDescriptor<T, float>{"AmmoBoxMaxDistance", &T::AmmoBoxMaxDistance, {}});
}

template <> constexpr auto DescribeMembers<Config::ConfigData::VisualsConfig>() {
    using T = Config::ConfigData::VisualsConfig;
    return std::make_tuple(
        MemberDescriptor<T, Config::ConfigData::VisualsConfig::PlayerConfig>{"Player", &T::Player, {}},
        MemberDescriptor<T, Config::ConfigData::VisualsConfig::LootConfig>{"Loot", &T::Loot, {}});
}

template <> constexpr auto DescribeMembers<Config::ConfigData::AimbotConfig::AimbotAmmoConfig>() {
    using T = Config::ConfigData::AimbotConfig::AimbotAmmoConfig;
    return std::make_tuple(MemberDescriptor<T, bool>{"Enabled", &T::Enabled, {}},
                           MemberDescriptor<T, float>{"Smoothness", &T::Smoothness, {}},
                           MemberDescriptor<T, float>{"MaxDistance", &T::MaxDistance, {}},
                           MemberDescriptor<T, bool>{"VisibleCheck", &T::VisibleCheck, {}},
                           MemberDescriptor<T, bool>{"StickyTarget", &T::StickyTarget, {}},
                           MemberDescriptor<T, bool>{"ShowFOV", &T::ShowFOV, {}},
                           MemberDescriptor<T, float>{"FOV", &T::FOV, {}},
                           MemberDescriptor<T, bool>{"UseDeadzone", &T::UseDeadzone, {}},
                           MemberDescriptor<T, bool>{"ShowDeadzoneFOV", &T::ShowDeadzoneFOV, {}},
                           MemberDescriptor<T, float>{"DeadzoneFOV", &T::DeadzoneFOV, {}},
                           MemberDescriptor<T, Config::ConfigData::TargetSelection>{"Selection", &T::Selection, {}},
                           MemberDescriptor<T, Config::ConfigData::TargetBone>{"Bone", &T::Bone, {}},
                           MemberDescriptor<T, float>{"RandomBoneRefreshRate", &T::RandomBoneRefreshRate, {}});
}

template <> constexpr auto DescribeMembers<Config::ConfigData::AimbotConfig>() {
    using T = Config::ConfigData::AimbotConfig;
    return std::make_tuple(MemberDescriptor<T, T::AimbotAmmoConfig>{"ShellsAimbot", &T::ShellsAimbot, {}},
                           MemberDescriptor<T, T::AimbotAmmoConfig>{"LightAimbot", &T::LightAimbot, {}},
                           MemberDescriptor<T, T::AimbotAmmoConfig>{"MediumAimbot", &T::MediumAimbot, {}},
                           MemberDescriptor<T, T::AimbotAmmoConfig>{"HeavyAimbot", &T::HeavyAimbot, {}},
                           MemberDescriptor<T, T::AimbotAmmoConfig>{"OtherAimbot", &T::OtherAimbot, {}},
                           MemberDescriptor<T, T::AimbotAmmoConfig>{"AllAimbot", &T::AllAimbot, {}},
                           MemberDescriptor<T, bool>{"SplitAimbotByAmmo", &T::SplitAimbotByAmmo, {}},
                           MemberDescriptor<T, bool>{"BulletPrediction", &T::BulletPrediction, {}},
                           MemberDescriptor<T, int>{"AimbotKeybind", &T::AimbotKeybind, {}});
}

template <> constexpr auto DescribeMembers<Config::ConfigData>() {
    using T = Config::ConfigData;
    return std::make_tuple(MemberDescriptor<T, Config::ConfigData::VisualsConfig>{"Visuals", &T::Visuals, {}},
                           MemberDescriptor<T, Config::ConfigData::AimbotConfig>{"Aimbot", &T::Aimbot, {}},
                           MemberDescriptor<T, Config::ConfigData::ColorConfig>{"Color", &T::Color, {}});
}

} // namespace ConfigReflection
