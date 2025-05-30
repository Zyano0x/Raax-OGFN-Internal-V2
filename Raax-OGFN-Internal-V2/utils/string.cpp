#include "string.h"
#include <Windows.h>

namespace String {

std::wstring NarrowToWide(const std::string& String) {
    if (String.empty())
        return L"";

    int Length = MultiByteToWideChar(CP_UTF8, 0, String.c_str(), -1, nullptr, 0);
    if (!Length)
        return L"";

    std::wstring WideString(Length, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, String.c_str(), -1, &WideString[0], Length);

    if (!WideString.empty() && WideString.back() == L'\0')
        WideString.pop_back();

    return WideString;
}

std::string WideToNarrow(const std::wstring& WideString) {
    if (WideString.empty())
        return "";

    int Length = WideCharToMultiByte(CP_UTF8, 0, WideString.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (!Length)
        return "";

    std::string NarrowString(Length, '\0');
    WideCharToMultiByte(CP_UTF8, 0, WideString.c_str(), -1, &NarrowString[0], Length, nullptr, nullptr);

    if (!NarrowString.empty() && NarrowString.back() == '\0')
        NarrowString.pop_back();

    return NarrowString;
}

} // namespace String
