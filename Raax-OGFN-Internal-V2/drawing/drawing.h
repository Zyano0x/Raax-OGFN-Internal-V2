#pragma once
#include <cheat/sdk/Basic.h>

namespace Drawing
{
    void Init();
    void Tick();

    // add outline soon
    void Line(const SDK::FVector2D& ScreenPositionA, const SDK::FVector2D& ScreenPositionB,
        const SDK::FLinearColor& RenderColor = SDK::FLinearColor::White, const float Thickness = 1.f,
        const bool Outlined = false, const float OutlineThickness = 1.f, const SDK::FLinearColor& OutlineColor = SDK::FLinearColor::Black);

    void Text(const char* RenderText, const SDK::FVector2D& ScreenPosition, const SDK::FLinearColor& RenderColor = SDK::FLinearColor::White,
        const float FontSize = 12.f, const bool CenteredX = true, const bool CenteredY = true,
        const bool Outlined = true, const float OutlineThickness = 1.f, const SDK::FLinearColor& OutlineColor = SDK::FLinearColor::Black);
    void Text(const wchar_t* RenderText, const SDK::FVector2D& ScreenPosition, const SDK::FLinearColor& RenderColor = SDK::FLinearColor::White,
        const float FontSize = 12.f, const bool CenteredX = true, const bool CenteredY = true,
        const bool Outlined = true, const float OutlineThickness = 1.f, const SDK::FLinearColor& OutlineColor = SDK::FLinearColor::Black);
}
