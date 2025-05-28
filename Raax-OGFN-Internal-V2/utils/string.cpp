#include "string.h"

#include <Windows.h>

std::wstring String::NarrowToWide(const std::string& String) {
    if (String.empty()) {
        return {};
    }

    const int Length = MultiByteToWideChar(CP_UTF8, 0, String.c_str(), -1, nullptr, 0);
    if (!Length) {
        return {};
    }

    std::wstring WideString(Length, 0);
    MultiByteToWideChar(CP_UTF8, 0, String.c_str(), -1, &WideString[0], Length);

    if (!WideString.empty() && WideString.back() == L'\0') {
        WideString.pop_back();
    }

    return WideString;
}
