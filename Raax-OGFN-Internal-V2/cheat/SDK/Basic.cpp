#include "Basic.h"
#include "Containers.h"

namespace SDK
{
    FName::FName(const wchar_t* Name)
        : ComparisonIndex(0), Number(0)
    {
        FNameConstructorW(this, Name, FNAME_Add);
    }
    FName::FName(const char* Name)
        : ComparisonIndex(0), Number(0)
    {
        std::string NameStr = Name;
        FNameConstructorW(this, std::wstring(NameStr.begin(), NameStr.end()).c_str(), FNAME_Add);
    }

    FString FName::ToFString() const {
        FString Out;
        FNameToString(this, &Out);
        return Out;
    }
    std::wstring FName::ToWString() const {
        FString FStr = ToFString();
        return FStr.ToWString();
    }
    std::string FName::ToString() const {
        FString FStr = ToFString();
        return FStr.ToString();
    }

    bool FName::operator==(const FName& Other) const {
        return ComparisonIndex == Other.ComparisonIndex;
    }
    bool FName::operator!=(const FName& Other) const {
        return !operator==(Other);
    }

    void FName::operator=(const FName& Other) {
        ComparisonIndex = Other.ComparisonIndex;
        Number = Other.Number;
    }
    void FName::operator=(const wchar_t* Name) {
        FName New(Name);
        ComparisonIndex = New.ComparisonIndex;
        Number = New.Number;
    }
    void FName::operator=(const char* Name) {
        FName New(Name);
        ComparisonIndex = New.ComparisonIndex;
        Number = New.Number;
    }
}
