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

        inline int32_t Num() {
            return NumElements;
        }

        inline int32_t Max() {
            return MaxElements;
        }

        inline int32_t GetSlack() {
            return MaxElements - NumElements;
        }

        inline ElementType* GetData() {
            return Data;
        }

    public:
        inline ElementType& operator[](int32_t Index) {
            return Data[Index];
        }
        inline const ElementType& operator[](int32_t Index) const {
            return Data[Index];
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
