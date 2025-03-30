#pragma once
#include <cstdint>
#include <string>

namespace SDK
{
    template<typename ElementType>
    class TArray
    {
    protected:
        ElementType* Data;
        int32_t NumElements;
        int32_t MaxElements;

    public:
        TArray() : Data(nullptr), NumElements(0), MaxElements(0) {};

    public:
        inline bool IsValid() {
            return Data != nullptr;
        }
    };

    class FString : public TArray<wchar_t>
    {
    public:
        inline std::wstring ToWString() {
            return IsValid() ? Data : L"";
        }

        inline std::string ToString() {
            if (IsValid()) {
                std::wstring WData = ToWString();
                return std::string(WData.begin(), WData.end());
            }
            return "";
        }
    };
}
