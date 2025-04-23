#pragma once
#include <cheat/sdk/Basic.h>

namespace Drawing
{
    void Init();
    void Tick();

    void BeginBatchedLines();
    void EndBatchedLines();

    // add outline soon
    void Line(const SDK::FVector2D& ScreenPositionA, const SDK::FVector2D& ScreenPositionB,
        const SDK::FLinearColor& RenderColor = SDK::FLinearColor::White, float Thickness = 1.f,
        bool Outlined = true, float OutlineThickness = 1.f, const SDK::FLinearColor& OutlineColor = SDK::FLinearColor::Black);

    void Text(const char* RenderText, const SDK::FVector2D& ScreenPosition, const SDK::FLinearColor& RenderColor = SDK::FLinearColor::White,
        float FontSize = 12.f, bool CenteredX = true, bool CenteredY = true,
        bool Outlined = true, float OutlineThickness = 1.f, const SDK::FLinearColor& OutlineColor = SDK::FLinearColor::Black);
    void Text(const wchar_t* RenderText, const SDK::FVector2D& ScreenPosition, const SDK::FLinearColor& RenderColor = SDK::FLinearColor::White,
        const float FontSize = 12.f, bool CenteredX = true, bool CenteredY = true,
        bool Outlined = true, float OutlineThickness = 1.f, const SDK::FLinearColor& OutlineColor = SDK::FLinearColor::Black);

    void Rect(const SDK::FVector2D& ScreenPosition, const SDK::FVector2D& ScreenSize, const SDK::FLinearColor& RenderColor = SDK::FLinearColor::White,
        float Thickness = 1.f, bool Outlined = true, float OutlineThickness = 1.f, const SDK::FLinearColor& OutlineColor = SDK::FLinearColor::Black);
    void CorneredRect(const SDK::FVector2D& ScreenPosition, const SDK::FVector2D& ScreenSize, const SDK::FLinearColor& RenderColor = SDK::FLinearColor::White,
        float Thickness = 1.f, bool Outlined = true, float OutlineThickness = 1.f, const SDK::FLinearColor& OutlineColor = SDK::FLinearColor::Black);
    void Rect3D(const SDK::FVector2D(&BoxCorners)[8], const SDK::FLinearColor& RenderColor = SDK::FLinearColor::White,
        float Thickness = 1.f, bool Outlined = true, float OutlineThickness = 1.f, const SDK::FLinearColor& OutlineColor = SDK::FLinearColor::Black);
}
