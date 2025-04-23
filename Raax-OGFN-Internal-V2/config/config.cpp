#include <config/config.h>
#include <utils/error.h>
#include <sstream>
#include <charconv>

bool TryParseBool(const std::string& Str, bool& Out) {
    int Temp = 0;
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

bool TryParseBoxType(const std::string& Str, Config::ConfigData::BoxType& OutBoxType) {
    int IntValue;
    if (TryParseInt(Str, IntValue)) {
        if (IntValue >= static_cast<int>(Config::ConfigData::BoxType::Full) &&
            IntValue <= static_cast<int>(Config::ConfigData::BoxType::Full3D)) {
            OutBoxType = static_cast<Config::ConfigData::BoxType>(IntValue);
            return true;
        }
    }
    return false;
}

bool TryParseTracerPos(const std::string& Str, Config::ConfigData::TracerPos& OutBoxType) {
    int IntValue;
    if (TryParseInt(Str, IntValue)) {
        if (IntValue >= static_cast<int>(Config::ConfigData::TracerPos::Top) &&
            IntValue <= static_cast<int>(Config::ConfigData::TracerPos::Bottom)) {
            OutBoxType = static_cast<Config::ConfigData::TracerPos>(IntValue);
            return true;
        }
    }
    return false;
}

bool TryParseColor(const std::string& Str, SDK::FLinearColor& OutColor) {
    std::stringstream ss(Str);
    std::string Item;
    int Colors[4];
    int i = 0;

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



std::string Config::ConfigData::SerializeConfig() {
    std::ostringstream out;

    ConfigData DefaultConfig = {};

    // Visuals - Player
    if (DefaultConfig.Visuals.Player.Box != Visuals.Player.Box)
        out << "Visuals.Player.Box=" << Visuals.Player.Box << ",";
    if (DefaultConfig.Visuals.Player.BoxType != Visuals.Player.BoxType)
        out << "Visuals.Player.BoxType=" << static_cast<int>(Visuals.Player.BoxType) << ",";
    if (DefaultConfig.Visuals.Player.BoxThickness != Visuals.Player.BoxThickness)
        out << "Visuals.Player.BoxThickness=" << Visuals.Player.BoxThickness << ",";

    if (DefaultConfig.Visuals.Player.Skeleton != Visuals.Player.Skeleton)
        out << "Visuals.Player.Skeleton=" << Visuals.Player.Skeleton << ",";
    if (DefaultConfig.Visuals.Player.SkeletonThickness != Visuals.Player.SkeletonThickness)
        out << "Visuals.Player.SkeletonThickness=" << Visuals.Player.SkeletonThickness << ",";

    if (DefaultConfig.Visuals.Player.Tracer != Visuals.Player.Tracer)
        out << "Visuals.Player.Tracer=" << Visuals.Player.Tracer << ",";
    if (DefaultConfig.Visuals.Player.TracerThickness != Visuals.Player.TracerThickness)
        out << "Visuals.Player.TracerThickness=" << Visuals.Player.TracerThickness << ",";
    if (DefaultConfig.Visuals.Player.TracerStart != Visuals.Player.TracerStart)
        out << "Visuals.Player.TracerStart=" <<  static_cast<int>(Visuals.Player.TracerStart) << ",";
    if (DefaultConfig.Visuals.Player.TracerEnd != Visuals.Player.TracerEnd)
        out << "Visuals.Player.TracerEnd=" <<  static_cast<int>(Visuals.Player.TracerEnd) << ",";

    if (DefaultConfig.Visuals.Player.Name != Visuals.Player.Name)
        out << "Visuals.Player.Name=" << Visuals.Player.Name << ",";

    // Visuals - Loot
    if (DefaultConfig.Visuals.Loot.LootText != Visuals.Loot.LootText)
        out << "Visuals.Loot.LootText=" << Visuals.Loot.LootText << ",";
    if (DefaultConfig.Visuals.Loot.ChestText != Visuals.Loot.ChestText)
        out << "Visuals.Loot.ChestText=" << Visuals.Loot.ChestText << ",";
    if (DefaultConfig.Visuals.Loot.AmmoBoxText != Visuals.Loot.AmmoBoxText)
        out << "Visuals.Loot.AmmoBoxText=" << Visuals.Loot.AmmoBoxText << ",";

    // Aimbot
    if (DefaultConfig.Aimbot.Enabled != Aimbot.Enabled)
        out << "Aimbot.Enabled=" << Aimbot.Enabled << ",";
    if (DefaultConfig.Aimbot.Smoothness != Aimbot.Smoothness)
        out << "Aimbot.Smoothness=" << Aimbot.Smoothness << ",";

    // Color
    if (DefaultConfig.Color.PrimaryColorVisible != Color.PrimaryColorVisible)
        out << "Color.PrimaryColorVisible=" << Color.PrimaryColorVisible.ToStr().c_str() << ",";
    if (DefaultConfig.Color.PrimaryColorHidden != Color.PrimaryColorHidden)
        out << "Color.PrimaryColorHidden=" << Color.PrimaryColorHidden.ToStr().c_str() << ",";
    if (DefaultConfig.Color.SecondaryColorVisible != Color.SecondaryColorVisible)
        out << "Color.SecondaryColorVisible=" << Color.SecondaryColorVisible.ToStr().c_str() << ",";
    if (DefaultConfig.Color.SecondaryColorHidden != Color.SecondaryColorHidden)
        out << "Color.SecondaryColorHidden=" << Color.SecondaryColorHidden.ToStr().c_str() << ",";


    std::string Result = out.str();
    if (!Result.empty())
        Result.pop_back();

    return Result;
}

bool Config::ConfigData::DeserializeConfig(const std::string& Data, ConfigData& MergeConfig) {
    std::istringstream in(Data);
    std::string Token;

    bool Success = false;
    ConfigData NewConfig = MergeConfig;
    while (std::getline(in, Token, ',')) {
        size_t EqPos = Token.find('=');
        if (EqPos == std::string::npos)
            continue;

        std::string Key = Token.substr(0, EqPos);
        std::string Value = Token.substr(EqPos + 1);
 
        int ConfigVarsAdded = 0;
        ConfigVarsAdded += (Key == "Visuals.Player.Box" && TryParseBool(Value, NewConfig.Visuals.Player.Box));
        ConfigVarsAdded += (Key == "Visuals.Player.BoxType" && TryParseBoxType(Value, NewConfig.Visuals.Player.BoxType));
        ConfigVarsAdded += (Key == "Visuals.Player.BoxThickness" && TryParseFloat(Value, NewConfig.Visuals.Player.BoxThickness));
        ConfigVarsAdded += (Key == "Visuals.Player.Skeleton" && TryParseBool(Value, NewConfig.Visuals.Player.Skeleton));
        ConfigVarsAdded += (Key == "Visuals.Player.SkeletonThickness" && TryParseFloat(Value, NewConfig.Visuals.Player.SkeletonThickness));
        
        ConfigVarsAdded += (Key == "Visuals.Player.Tracer" && TryParseBool(Value, NewConfig.Visuals.Player.Tracer));
        ConfigVarsAdded += (Key == "Visuals.Player.TracerThickness" && TryParseFloat(Value, NewConfig.Visuals.Player.TracerThickness));
        ConfigVarsAdded += (Key == "Visuals.Player.TracerStart" && TryParseTracerPos(Value, NewConfig.Visuals.Player.TracerStart));
        ConfigVarsAdded += (Key == "Visuals.Player.TracerEnd" && TryParseTracerPos(Value, NewConfig.Visuals.Player.TracerEnd));

        ConfigVarsAdded += (Key == "Visuals.Player.Name" && TryParseBool(Value, NewConfig.Visuals.Player.Name));

        ConfigVarsAdded += (Key == "Visuals.Loot.LootText" && TryParseBool(Value, NewConfig.Visuals.Loot.LootText));
        ConfigVarsAdded += (Key == "Visuals.Loot.ChestText" && TryParseBool(Value, NewConfig.Visuals.Loot.ChestText));
        ConfigVarsAdded += (Key == "Visuals.Loot.AmmoBoxText" && TryParseBool(Value, NewConfig.Visuals.Loot.AmmoBoxText));

        ConfigVarsAdded += (Key == "Aimbot.Enabled" && TryParseBool(Value, NewConfig.Aimbot.Enabled));
        ConfigVarsAdded += (Key == "Aimbot.Smoothness" && TryParseFloat(Value, NewConfig.Aimbot.Smoothness));

        ConfigVarsAdded += (Key == "Color.PrimaryColorVisible" && TryParseColor(Value, NewConfig.Color.PrimaryColorVisible));
        ConfigVarsAdded += (Key == "Color.PrimaryColorHidden" && TryParseColor(Value, NewConfig.Color.PrimaryColorHidden));
        ConfigVarsAdded += (Key == "Color.SecondaryColorVisible" && TryParseColor(Value, NewConfig.Color.SecondaryColorVisible));
        ConfigVarsAdded += (Key == "Color.SecondaryColorHidden" && TryParseColor(Value, NewConfig.Color.SecondaryColorHidden));

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
