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

    wchar_t* FText::Get() const {
        if (this && Data)
            return Data->Name;

        return nullptr;
    }
    std::string FText::ToString() const {
        wchar_t* Text = Get();
        if (Text) {
            std::wstring Temp(Text);
            return std::string(Temp.begin(), Temp.end());
        }

        return "";
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

    const FLinearColor FLinearColor::White = FLinearColor(1.f, 1.f, 1.f, 1.f);
    const FLinearColor FLinearColor::Gray = FLinearColor(0.5f, 0.5f, 0.5f, 1.f);
    const FLinearColor FLinearColor::Black = FLinearColor(0.f, 0.f, 0.f, 1.f);
    const FLinearColor FLinearColor::Red = FLinearColor(1.f, 0.f, 0.f, 1.f);
    const FLinearColor FLinearColor::Green = FLinearColor(0.f, 1.f, 0.f, 1.f);
    const FLinearColor FLinearColor::Blue = FLinearColor(0.f, 0.f, 1.f, 1.f);
}
