#pragma once
#include <cstdint>
#include <string>

namespace SDK
{
    enum EFindName
    {
        FNAME_Find,
        FNAME_Add
    };

    class FName
    {
    public:
        static inline void(*FNameToString)(const FName*, class FString*) = nullptr;
        static inline void(*FNameConstructorW)(const FName*, const wchar_t*, EFindName) = nullptr;

    public:
        int32_t ComparisonIndex;
        int32_t Number;

    public:
        FName() : ComparisonIndex(0), Number(0) {}
        FName(const wchar_t* Name);
        FName(const char* Name);

    public:
        class FString ToFString() const;
        std::wstring ToWString() const;
        std::string ToString() const;

        bool operator==(const FName& Other) const;
        bool operator!=(const FName& Other) const;

        void operator=(const FName& Other);
        void operator=(const wchar_t* Name);
        void operator=(const char* Name);
    };
}
