#include <drawing/drawing.h>
#include <extern/imgui/imgui.h>
#include "fontdata.h"

ImDrawList* g_DrawList = nullptr;

ImFont* g_MainFontSmall = nullptr;
ImFont* g_MainFont = nullptr;

void Drawing::Init() {
    ImFontConfig Config;
    Config.FontDataOwnedByAtlas = false; // Ensure ImGui doesn't try to free our font data.
    g_MainFontSmall = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(&RawFontData, sizeof(RawFontData), 12.0f, &Config);
    g_MainFont = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(&RawFontData, sizeof(RawFontData), 48.0f, &Config);
    g_DrawList = ImGui::GetBackgroundDrawList();
}
void Drawing::Tick() {
    g_DrawList = ImGui::GetBackgroundDrawList();
}

void Drawing::Line(const SDK::FVector2D& ScreenPositionA, const SDK::FVector2D& ScreenPositionB,
    const SDK::FLinearColor& RenderColor, const float Thickness,
    const bool Outlined, const float OutlineThickness, const SDK::FLinearColor& OutlineColor)
{
    g_DrawList->AddLine(ImVec2(ScreenPositionA.X, ScreenPositionA.Y), ImVec2(ScreenPositionB.X, ScreenPositionB.Y), ImColor(RenderColor.R, RenderColor.G, RenderColor.B, RenderColor.A), Thickness);
}

void Drawing::Text(const char* RenderText, const SDK::FVector2D& ScreenPosition, const SDK::FLinearColor& RenderColor,
    const float FontSize, const bool CenteredX, const bool CenteredY,
    const bool Outlined, const float OutlineThickness, const SDK::FLinearColor& OutlineColor)
{
    ImVec2 TextPosition = ImVec2(ScreenPosition.X, ScreenPosition.Y);
    if (CenteredX || CenteredY) {
        ImVec2 TextSize = g_MainFont->CalcTextSizeA(FontSize, FLT_MAX, FLT_MAX, RenderText);

        if (CenteredX)
            TextPosition.x -= TextSize.x / 2.f;
        if (CenteredY)
            TextPosition.y -= TextSize.y / 2.f;
    }

    if (Outlined) {
        constexpr ImVec2 Offsets[] = {
            ImVec2(-1.f, 0),
            ImVec2(1.f, 0),
            ImVec2(0, -1.f),
            ImVec2(0, 1.f),
        };

        for (const ImVec2& Offset : Offsets) {
            ImVec2 OutlinePos = ImVec2(TextPosition.x + Offset.x, TextPosition.y + Offset.y);
            g_DrawList->AddText(g_MainFont, FontSize, OutlinePos, ImColor(OutlineColor.R, OutlineColor.G, OutlineColor.B, OutlineColor.A), RenderText);
        }
    }

    g_DrawList->AddText(g_MainFont, FontSize, TextPosition, ImColor(RenderColor.R, RenderColor.G, RenderColor.B, RenderColor.A), RenderText);
}
void Drawing::Text(const wchar_t* RenderText, const SDK::FVector2D& ScreenPosition, const SDK::FLinearColor& RenderColor,
    const float FontSize, const bool CenteredX, const bool CenteredY,
    const bool Outlined, const float OutlineThickness, const SDK::FLinearColor& OutlineColor)
{
    std::wstring wstr = RenderText;
    Text(std::string(wstr.begin(), wstr.end()).c_str(), ScreenPosition, RenderColor, FontSize, CenteredX, CenteredY, Outlined, OutlineThickness, OutlineColor);
}
