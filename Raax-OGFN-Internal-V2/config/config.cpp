#include "config.h"
#include "config_reflection.h"
#include <sstream>
#include <charconv>

#include <utils/error.h>
#include <utils/log.h>

namespace Config {

// --- Serialization Utility Functions -------------------------------

bool TryParseBool(const std::string& Str, bool& Out) {
    bool StrIsTrue = Str == "true";
    bool StrIsFalse = Str == "false";
    if (!StrIsTrue && !StrIsFalse)
        return false;

    Out = StrIsTrue;
    return true;
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

template <typename T> bool ShouldSerializeValue(const T& Value, const T& MergeValue) {
    if constexpr (requires { ConfigReflection::DescribeMembers<T>(); }) {
        const auto Members = ConfigReflection::DescribeMembers<T>();
        bool       AnyDifference = false;

        std::apply(
            [&](const auto&... Member) {
                auto CompareMember = [&](const auto& m) {
                    if (ShouldSerializeValue(Value.*(m.Ptr), MergeValue.*(m.Ptr))) {
                        AnyDifference = true;
                    }
                };
                (CompareMember(Member), ...);
            },
            Members);

        return AnyDifference;
    } else {
        return Value != MergeValue;
    }
}

template <typename T> std::string SerializeValue(const T& Value, const T& MergeValue, bool IgnoreMergeConfig) {
    if constexpr (requires { ConfigReflection::DescribeMembers<T>(); }) {
        if (!IgnoreMergeConfig && !ShouldSerializeValue(Value, MergeValue)) {
            return "";
        }

        std::string Result = "{";
        const auto  Members = ConfigReflection::DescribeMembers<T>();
        bool        FirstMember = true;

        std::apply(
            [&](const auto&... Member) {
                auto ProcessMember = [&](const auto& m) {
                    std::string MemberValue = SerializeValue(Value.*(m.Ptr), MergeValue.*(m.Ptr), IgnoreMergeConfig);
                    if (MemberValue.empty()) {
                        return;
                    }

                    if (!FirstMember) {
                        Result += ",";
                    }
                    FirstMember = false;

                    Result += "\"" + std::string(m.Name) + "\":" + MemberValue;
                };
                (ProcessMember(Member), ...);
            },
            Members);

        if (Result.size() == 1) {
            return "";
        }

        Result += "}";
        return Result;
    } else if constexpr (std::is_enum_v<T>) {
        if (IgnoreMergeConfig || (Value != MergeValue)) {
            return std::to_string(static_cast<int>(Value));
        }
        return "";
    } else if constexpr (std::is_same_v<T, bool>) {
        if (IgnoreMergeConfig || (Value != MergeValue)) {
            return Value ? "true" : "false";
        }
        return "";
    } else if constexpr (std::is_arithmetic_v<T>) {
        if (IgnoreMergeConfig || (Value != MergeValue)) {
            return std::to_string(Value);
        }
        return "";
    } else if constexpr (std::is_same_v<T, std::string>) {
        if (IgnoreMergeConfig || (Value != MergeValue)) {
            return Value;
        }
        return "";
    } else if constexpr (std::is_same_v<T, SDK::FLinearColor>) {
        if (IgnoreMergeConfig || (Value != MergeValue)) {
            return std::format("{}/{}/{}/{}", (int)(Value.R * 255.f), (int)(Value.G * 255.f), (int)(Value.B * 255.f),
                               (int)(Value.A * 255.f));
        }
        return "";
    } else {
        static_assert(false, "Unsupported type for serialization!");
    }
}

std::string ConfigData::SerializeConfig(bool FullConfig) {
    ConfigData DefaultConfig = {};
    return SerializeValue(*this, DefaultConfig, FullConfig);
}

template <typename T> bool DeserializeValue(T& Output, const std::string& Data) {
    if constexpr (requires { ConfigReflection::DescribeMembers<T>(); }) {
        size_t Start = Data.find('{');
        size_t End = Data.rfind('}');

        if (Start == std::string::npos || End == std::string::npos) {
            return false;
        }

        std::string Content = Data.substr(Start + 1, End - Start - 1);
        const auto  Members = ConfigReflection::DescribeMembers<T>();
        bool        AnySuccess = false;

        std::apply(
            [&](const auto&... Member) {
                auto ProcessMember = [&](const auto& m) {
                    std::string SearchStr = "\"" + std::string(m.Name) + "\":";
                    size_t      Pos = Content.find(SearchStr);
                    if (Pos == std::string::npos) {
                        return;
                    }

                    size_t ValueStart = Pos + SearchStr.length();
                    size_t ValueEnd = ValueStart;
                    int    BraceCount = 0;
                    bool   InString = false;
                    while (ValueEnd < Content.length()) {
                        char c = Content[ValueEnd];
                        if (c == '"' && (ValueEnd == 0 || Content[ValueEnd - 1] != '\\')) {
                            InString = !InString;
                        } else if (!InString) {
                            if (c == '{')
                                BraceCount++;
                            else if (c == '}')
                                BraceCount--;
                            else if (c == ',' && BraceCount == 0)
                                break;
                        }

                        ValueEnd++;
                    }

                    std::string ValueStr = Content.substr(ValueStart, ValueEnd - ValueStart);
                    if (!ValueStr.empty()) {
                        using MemberType = std::remove_reference_t<decltype(Output.*(m.Ptr))>;
                        if (DeserializeValue<MemberType>(Output.*(m.Ptr), ValueStr)) {
                            AnySuccess = true;
                        }
                    }
                };
                (ProcessMember(Member), ...);
            },
            Members);

        return AnySuccess;
    } else if constexpr (std::is_enum_v<T>) {
        return TryParseEnum<T>(Data, Output);
    } else if constexpr (std::is_same_v<T, bool>) {
        return TryParseBool(Data, Output);
    } else if constexpr (std::is_same_v<T, int>) {
        return TryParseInt(Data, Output);
    } else if constexpr (std::is_same_v<T, float>) {
        return TryParseFloat(Data, Output);
    } else if constexpr (std::is_same_v<T, SDK::FLinearColor>) {
        return TryParseColor(Data, Output);
    } else {
        static_assert(false, "Unsupported type for deserialization!");
    }
}

bool ConfigData::DeserializeConfig(const std::string& Data) {
    return DeserializeValue<ConfigData>(*this, Data);
}

} // namespace Config
