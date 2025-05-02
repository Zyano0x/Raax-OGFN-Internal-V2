#include "config.h"
#include <sstream>
#include <charconv>

#include <utils/error.h>

namespace Config {

// --- Serialization Utility Functions -------------------------------

bool TryParseBool(const std::string& Str, bool& Out) {
    int  Temp = 0;
    auto Result = std::from_chars(Str.data(), Str.data() + Str.size(), Temp);
    Out = Temp == 1;
    return Result.ec == std::errc();
}

bool TryParseInt(const std::string& Str, int& Out) {
    auto Result = std::from_chars(Str.data(), Str.data() + Str.size(), Out);
    return Result.ec == std::errc();
}

bool TryParseFloat(const std::string& Str, float& Out) {
    auto Result = std::from_chars(Str.data(), Str.data() + Str.size(), Out);
    return Result.ec == std::errc();
}

bool TryParseColor(const std::string& Str, SDK::FLinearColor& OutColor) {
    std::stringstream ss(Str);
    std::string       Item;
    int               Colors[4];
    int               i = 0;

    while (std::getline(ss, Item, '/') && i < 4) {
        if (!TryParseInt(Item, Colors[i]))
            return false;
        i++;
    }

    if (i != 4)
        return false;

    OutColor = SDK::FLinearColor{Colors[0] / 255.f, Colors[1] / 255.f, Colors[2] / 255.f, Colors[3] / 255.f};
    return true;
}

template <typename T> bool TryParseEnum(const std::string& Str, T& Out) {
    int IntValue;
    if (TryParseInt(Str, IntValue)) {
        if (IntValue >= static_cast<int>(T::MIN) && IntValue <= static_cast<int>(T::MAX)) {
            Out = static_cast<T>(IntValue);
            return true;
        }
    }
    return false;
}

// --- Config Serialization Functions --------------------------------

std::string ConfigData::SerializeConfig(bool FullConfig) {
    std::ostringstream out;

    ConfigData DefaultConfig = {};

    // Visuals - Player
    if (FullConfig || DefaultConfig.Visuals.Player.Box != Visuals.Player.Box)
        out << "Visuals.Player.Box=" << Visuals.Player.Box << ",";
    if (FullConfig || DefaultConfig.Visuals.Player.BoxType != Visuals.Player.BoxType)
        out << "Visuals.Player.BoxType=" << static_cast<int>(Visuals.Player.BoxType) << ",";
    if (FullConfig || DefaultConfig.Visuals.Player.BoxThickness != Visuals.Player.BoxThickness)
        out << "Visuals.Player.BoxThickness=" << Visuals.Player.BoxThickness << ",";

    if (FullConfig || DefaultConfig.Visuals.Player.Skeleton != Visuals.Player.Skeleton)
        out << "Visuals.Player.Skeleton=" << Visuals.Player.Skeleton << ",";
    if (FullConfig || DefaultConfig.Visuals.Player.SkeletonThickness != Visuals.Player.SkeletonThickness)
        out << "Visuals.Player.SkeletonThickness=" << Visuals.Player.SkeletonThickness << ",";

    if (FullConfig || DefaultConfig.Visuals.Player.Tracer != Visuals.Player.Tracer)
        out << "Visuals.Player.Tracer=" << Visuals.Player.Tracer << ",";
    if (FullConfig || DefaultConfig.Visuals.Player.TracerThickness != Visuals.Player.TracerThickness)
        out << "Visuals.Player.TracerThickness=" << Visuals.Player.TracerThickness << ",";
    if (FullConfig || DefaultConfig.Visuals.Player.TracerStart != Visuals.Player.TracerStart)
        out << "Visuals.Player.TracerStart=" << static_cast<int>(Visuals.Player.TracerStart) << ",";
    if (FullConfig || DefaultConfig.Visuals.Player.TracerEnd != Visuals.Player.TracerEnd)
        out << "Visuals.Player.TracerEnd=" << static_cast<int>(Visuals.Player.TracerEnd) << ",";

    if (FullConfig || DefaultConfig.Visuals.Player.Name != Visuals.Player.Name)
        out << "Visuals.Player.Name=" << Visuals.Player.Name << ",";

    // Visuals - Loot
    if (FullConfig || DefaultConfig.Visuals.Loot.LootText != Visuals.Loot.LootText)
        out << "Visuals.Loot.LootText=" << Visuals.Loot.LootText << ",";
    if (FullConfig || DefaultConfig.Visuals.Loot.LootMaxDistance != Visuals.Loot.LootMaxDistance)
        out << "Visuals.Loot.LootMaxDistance=" << Visuals.Loot.LootMaxDistance << ",";
    if (FullConfig || DefaultConfig.Visuals.Loot.MinLootTier != Visuals.Loot.MinLootTier)
        out << "Visuals.Loot.MinLootTier=" << static_cast<int>(Visuals.Loot.MinLootTier) << ",";

    if (FullConfig || DefaultConfig.Visuals.Loot.ChestText != Visuals.Loot.ChestText)
        out << "Visuals.Loot.ChestText=" << Visuals.Loot.ChestText << ",";
    if (FullConfig || DefaultConfig.Visuals.Loot.ChestMaxDistance != Visuals.Loot.ChestMaxDistance)
        out << "Visuals.Loot.ChestMaxDistance=" << Visuals.Loot.ChestMaxDistance << ",";

    if (FullConfig || DefaultConfig.Visuals.Loot.AmmoBoxText != Visuals.Loot.AmmoBoxText)
        out << "Visuals.Loot.AmmoBoxText=" << Visuals.Loot.AmmoBoxText << ",";
    if (FullConfig || DefaultConfig.Visuals.Loot.AmmoBoxMaxDistance != Visuals.Loot.AmmoBoxMaxDistance)
        out << "Visuals.Loot.AmmoBoxMaxDistance=" << Visuals.Loot.AmmoBoxMaxDistance << ",";

    // Aimbot
    auto SerializeAimbotConfig = [&](const std::string& Prefix, ConfigData::AimbotConfig& DefaultAimConfig,
                                     ConfigData::AimbotConfig& AimConfig) {
        if (FullConfig || DefaultAimConfig.Enabled != AimConfig.Enabled)
            out << Prefix << "Enabled=" << AimConfig.Enabled << ",";
        if (FullConfig || DefaultAimConfig.Smoothness != AimConfig.Smoothness)
            out << Prefix << "Smoothness=" << AimConfig.Smoothness << ",";
        if (FullConfig || DefaultAimConfig.MaxDistance != AimConfig.MaxDistance)
            out << Prefix << "MaxDistance=" << AimConfig.MaxDistance << ",";

        if (FullConfig || DefaultAimConfig.VisibleCheck != AimConfig.VisibleCheck)
            out << Prefix << "VisibleCheck=" << AimConfig.VisibleCheck << ",";
        if (FullConfig || DefaultAimConfig.StickyTarget != AimConfig.StickyTarget)
            out << Prefix << "StickyTarget=" << AimConfig.StickyTarget << ",";

        if (FullConfig || DefaultAimConfig.ShowFOV != AimConfig.ShowFOV)
            out << Prefix << "ShowFOV=" << AimConfig.ShowFOV << ",";
        if (FullConfig || DefaultAimConfig.FOV != AimConfig.FOV)
            out << Prefix << "FOV=" << AimConfig.FOV << ",";

        if (FullConfig || DefaultAimConfig.UseDeadzone != AimConfig.UseDeadzone)
            out << Prefix << "UseDeadzone=" << AimConfig.UseDeadzone << ",";
        if (FullConfig || DefaultAimConfig.ShowDeadzoneFOV != AimConfig.ShowDeadzoneFOV)
            out << Prefix << "ShowDeadzoneFOV=" << AimConfig.ShowDeadzoneFOV << ",";
        if (FullConfig || DefaultAimConfig.DeadzoneFOV != AimConfig.DeadzoneFOV)
            out << Prefix << "DeadzoneFOV=" << AimConfig.DeadzoneFOV << ",";

        if (FullConfig || DefaultAimConfig.Selection != AimConfig.Selection)
            out << Prefix << "Selection=" << static_cast<int>(AimConfig.Selection) << ",";
        if (FullConfig || DefaultAimConfig.Bone != AimConfig.Bone)
            out << Prefix << "Bone=" << static_cast<int>(AimConfig.Bone) << ",";

        if (FullConfig || DefaultAimConfig.RandomBoneRefreshRate != AimConfig.RandomBoneRefreshRate)
            out << Prefix << "RandomBoneRefreshRate=" << AimConfig.RandomBoneRefreshRate << ",";
    };
    SerializeAimbotConfig("ShellsAimbot.", DefaultConfig.ShellsAimbot, ShellsAimbot);
    SerializeAimbotConfig("LightAimbot.", DefaultConfig.LightAimbot, LightAimbot);
    SerializeAimbotConfig("MediumAimbot.", DefaultConfig.MediumAimbot, MediumAimbot);
    SerializeAimbotConfig("HeavyAimbot.", DefaultConfig.HeavyAimbot, HeavyAimbot);
    SerializeAimbotConfig("OtherAimbot.", DefaultConfig.OtherAimbot, OtherAimbot);

    SerializeAimbotConfig("AllAimbot.", DefaultConfig.AllAimbot, AllAimbot);

    if (FullConfig || DefaultConfig.SplitAimbotByAmmo != SplitAimbotByAmmo)
        out << "SplitAimbotByAmmo=" << SplitAimbotByAmmo << ",";
    if (FullConfig || DefaultConfig.AimbotKeybind != AimbotKeybind)
        out << "AimbotKeybind=" << AimbotKeybind << ",";

    // Color
    if (FullConfig || DefaultConfig.Color.PrimaryColorVisible != Color.PrimaryColorVisible)
        out << "Color.PrimaryColorVisible=" << Color.PrimaryColorVisible.ToStr().c_str() << ",";
    if (FullConfig || DefaultConfig.Color.PrimaryColorHidden != Color.PrimaryColorHidden)
        out << "Color.PrimaryColorHidden=" << Color.PrimaryColorHidden.ToStr().c_str() << ",";
    if (FullConfig || DefaultConfig.Color.SecondaryColorVisible != Color.SecondaryColorVisible)
        out << "Color.SecondaryColorVisible=" << Color.SecondaryColorVisible.ToStr().c_str() << ",";
    if (FullConfig || DefaultConfig.Color.SecondaryColorHidden != Color.SecondaryColorHidden)
        out << "Color.SecondaryColorHidden=" << Color.SecondaryColorHidden.ToStr().c_str() << ",";

    std::string Result = out.str();
    if (!Result.empty())
        Result.pop_back();

    return Result;
}

bool ConfigData::DeserializeConfig(const std::string& Data, ConfigData& MergeConfig) {
    std::istringstream in(Data);
    std::string        Token;

    bool       Success = false;
    ConfigData NewConfig = MergeConfig;
    while (std::getline(in, Token, ',')) {
        size_t EqPos = Token.find('=');
        if (EqPos == std::string::npos)
            continue;

        std::string Key = Token.substr(0, EqPos);
        std::string Value = Token.substr(EqPos + 1);

        int ConfigVarsAdded = 0;
        ConfigVarsAdded += (Key == "Visuals.Player.Box" && TryParseBool(Value, NewConfig.Visuals.Player.Box));
        ConfigVarsAdded += (Key == "Visuals.Player.BoxType" && TryParseEnum(Value, NewConfig.Visuals.Player.BoxType));
        ConfigVarsAdded +=
            (Key == "Visuals.Player.BoxThickness" && TryParseFloat(Value, NewConfig.Visuals.Player.BoxThickness));
        ConfigVarsAdded += (Key == "Visuals.Player.Skeleton" && TryParseBool(Value, NewConfig.Visuals.Player.Skeleton));
        ConfigVarsAdded += (Key == "Visuals.Player.SkeletonThickness" &&
                            TryParseFloat(Value, NewConfig.Visuals.Player.SkeletonThickness));

        ConfigVarsAdded += (Key == "Visuals.Player.Tracer" && TryParseBool(Value, NewConfig.Visuals.Player.Tracer));
        ConfigVarsAdded +=
            (Key == "Visuals.Player.TracerThickness" && TryParseFloat(Value, NewConfig.Visuals.Player.TracerThickness));
        ConfigVarsAdded +=
            (Key == "Visuals.Player.TracerStart" && TryParseEnum(Value, NewConfig.Visuals.Player.TracerStart));
        ConfigVarsAdded +=
            (Key == "Visuals.Player.TracerEnd" && TryParseEnum(Value, NewConfig.Visuals.Player.TracerEnd));

        ConfigVarsAdded += (Key == "Visuals.Player.Name" && TryParseBool(Value, NewConfig.Visuals.Player.Name));

        ConfigVarsAdded += (Key == "Visuals.Loot.LootText" && TryParseBool(Value, NewConfig.Visuals.Loot.LootText));
        ConfigVarsAdded +=
            (Key == "Visuals.Loot.LootMaxDistance" && TryParseFloat(Value, NewConfig.Visuals.Loot.LootMaxDistance));
        ConfigVarsAdded +=
            (Key == "Visuals.Loot.MinLootTier" && TryParseEnum(Value, NewConfig.Visuals.Loot.MinLootTier));

        ConfigVarsAdded += (Key == "Visuals.Loot.ChestText" && TryParseBool(Value, NewConfig.Visuals.Loot.ChestText));
        ConfigVarsAdded +=
            (Key == "Visuals.Loot.ChestMaxDistance" && TryParseFloat(Value, NewConfig.Visuals.Loot.ChestMaxDistance));

        ConfigVarsAdded +=
            (Key == "Visuals.Loot.AmmoBoxText" && TryParseBool(Value, NewConfig.Visuals.Loot.AmmoBoxText));
        ConfigVarsAdded += (Key == "Visuals.Loot.AmmoBoxMaxDistance" &&
                            TryParseFloat(Value, NewConfig.Visuals.Loot.AmmoBoxMaxDistance));

        auto DeserializeAimbotConfig = [&](const std::string& Prefix, ConfigData::AimbotConfig& AimConfig) {
            ConfigVarsAdded += (Key == Prefix + "Enabled" && TryParseBool(Value, AimConfig.Enabled));
            ConfigVarsAdded += (Key == Prefix + "Smoothness" && TryParseFloat(Value, AimConfig.Smoothness));
            ConfigVarsAdded += (Key == Prefix + "MaxDistance" && TryParseFloat(Value, AimConfig.MaxDistance));

            ConfigVarsAdded += (Key == Prefix + "VisibleCheck" && TryParseBool(Value, AimConfig.VisibleCheck));
            ConfigVarsAdded += (Key == Prefix + "StickyTarget" && TryParseBool(Value, AimConfig.StickyTarget));

            ConfigVarsAdded += (Key == Prefix + "ShowFOV" && TryParseBool(Value, AimConfig.ShowFOV));
            ConfigVarsAdded += (Key == Prefix + "FOV" && TryParseFloat(Value, AimConfig.FOV));

            ConfigVarsAdded += (Key == Prefix + "UseDeadzone" && TryParseBool(Value, AimConfig.UseDeadzone));
            ConfigVarsAdded += (Key == Prefix + "ShowDeadzoneFOV" && TryParseBool(Value, AimConfig.ShowDeadzoneFOV));
            ConfigVarsAdded += (Key == Prefix + "DeadzoneFOV" && TryParseFloat(Value, AimConfig.DeadzoneFOV));

            ConfigVarsAdded += (Key == Prefix + "Selection" && TryParseEnum(Value, AimConfig.Selection));
            ConfigVarsAdded += (Key == Prefix + "Bone" && TryParseEnum(Value, AimConfig.Bone));

            ConfigVarsAdded +=
                (Key == Prefix + "RandomBoneRefreshRate" && TryParseFloat(Value, AimConfig.RandomBoneRefreshRate));
        };
        DeserializeAimbotConfig("ShellsAimbot.", NewConfig.ShellsAimbot);
        DeserializeAimbotConfig("LightAimbot.", NewConfig.LightAimbot);
        DeserializeAimbotConfig("MediumAimbot.", NewConfig.MediumAimbot);
        DeserializeAimbotConfig("HeavyAimbot.", NewConfig.HeavyAimbot);
        DeserializeAimbotConfig("OtherAimbot.", NewConfig.OtherAimbot);

        DeserializeAimbotConfig("AllAimbot.", NewConfig.AllAimbot);

        ConfigVarsAdded += (Key == "SplitAimbotByAmmo" && TryParseBool(Value, NewConfig.SplitAimbotByAmmo));
        ConfigVarsAdded += (Key == "AimbotKeybind" && TryParseInt(Value, NewConfig.AimbotKeybind));

        ConfigVarsAdded +=
            (Key == "Color.PrimaryColorVisible" && TryParseColor(Value, NewConfig.Color.PrimaryColorVisible));
        ConfigVarsAdded +=
            (Key == "Color.PrimaryColorHidden" && TryParseColor(Value, NewConfig.Color.PrimaryColorHidden));
        ConfigVarsAdded +=
            (Key == "Color.SecondaryColorVisible" && TryParseColor(Value, NewConfig.Color.SecondaryColorVisible));
        ConfigVarsAdded +=
            (Key == "Color.SecondaryColorHidden" && TryParseColor(Value, NewConfig.Color.SecondaryColorHidden));

        if (!ConfigVarsAdded) {
            Success = false;
            break;
        }
        Success = true;
    }

    if (Success)
        *this = NewConfig;
    return Success;
}

} // namespace Config
