#include "drawing.h"
#include "fontdata.h"
#include <vector>

#include <extern/imgui/imgui.h>

namespace Drawing {

// --- Drawing Data --------------------------------------------------

struct BatchedLine {
    SDK::FVector2D    A, B;
    SDK::FLinearColor Color;
    float             Thickness;
    bool              Outlined;
    float             OutlineThickness;
    SDK::FLinearColor OutlineColor;
};

ImDrawList*              g_DrawList = nullptr;
ImFont*                  g_MainFontSmall = nullptr;
ImFont*                  g_MainFont = nullptr;
bool                     g_BatchedLines = false;
std::vector<BatchedLine> g_BatchedLinesList;

// --- Initialization & Tick Functions -------------------------------

void Init() {
    ImFontConfig Config;
    Config.FontDataOwnedByAtlas = false; // Ensure ImGui doesn't try to free our font data.
    g_MainFontSmall = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(&RawFontData, sizeof(RawFontData), 12.0f, &Config);
    g_MainFont = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(&RawFontData, sizeof(RawFontData), 48.0f, &Config);
    g_DrawList = ImGui::GetBackgroundDrawList();
}
void Tick() {
    g_DrawList = ImGui::GetBackgroundDrawList();
}

// --- Drawing Utility Functions -------------------------------------

void BeginBatchedLines() {
    g_BatchedLines = true;
}
void EndBatchedLines() {
    for (const auto& Line : g_BatchedLinesList) {
        if (Line.Outlined)
            g_DrawList->AddLine(
                ImVec2(Line.A.X, Line.A.Y), ImVec2(Line.B.X, Line.B.Y),
                ImColor(Line.OutlineColor.R, Line.OutlineColor.G, Line.OutlineColor.B, Line.OutlineColor.A),
                Line.OutlineThickness + Line.Thickness);
    }

    for (const auto& Line : g_BatchedLinesList) {
        g_DrawList->AddLine(ImVec2(Line.A.X, Line.A.Y), ImVec2(Line.B.X, Line.B.Y),
                            ImColor(Line.Color.R, Line.Color.G, Line.Color.B, Line.Color.A), Line.Thickness);
    }

    g_BatchedLinesList.clear();
    g_BatchedLines = false;
}

// --- Drawing Functions ---------------------------------------------

void Line(const SDK::FVector2D& ScreenPositionA, const SDK::FVector2D& ScreenPositionB,
          const SDK::FLinearColor& RenderColor, const float Thickness, const bool Outlined,
          const float OutlineThickness, const SDK::FLinearColor& OutlineColor) {
    if (ScreenPositionA.X != -1.f && ScreenPositionA.Y != -1.f && ScreenPositionB.X != -1.f &&
        ScreenPositionB.Y != -1.f) {
        if (g_BatchedLines)
            g_BatchedLinesList.push_back(
                {ScreenPositionA, ScreenPositionB, RenderColor, Thickness, Outlined, OutlineThickness, OutlineColor});
        else {
            if (Outlined)
                g_DrawList->AddLine(ImVec2(ScreenPositionA.X, ScreenPositionA.Y),
                                    ImVec2(ScreenPositionB.X, ScreenPositionB.Y),
                                    ImColor(OutlineColor.R, OutlineColor.G, OutlineColor.B, OutlineColor.A),
                                    OutlineThickness + Thickness);

            g_DrawList->AddLine(ImVec2(ScreenPositionA.X, ScreenPositionA.Y),
                                ImVec2(ScreenPositionB.X, ScreenPositionB.Y),
                                ImColor(RenderColor.R, RenderColor.G, RenderColor.B, RenderColor.A), Thickness);
        }
    }
}

void Text(const char* RenderText, const SDK::FVector2D& ScreenPosition, const SDK::FLinearColor& RenderColor,
          const float FontSize, const bool CenteredX, const bool CenteredY, const bool Outlined,
          const float OutlineThickness, const SDK::FLinearColor& OutlineColor) {
    if (ScreenPosition.X != -1.f && ScreenPosition.Y != -1.f) {
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
                g_DrawList->AddText(g_MainFont, FontSize, OutlinePos,
                                    ImColor(OutlineColor.R, OutlineColor.G, OutlineColor.B, OutlineColor.A),
                                    RenderText);
            }
        }

        g_DrawList->AddText(g_MainFont, FontSize, TextPosition,
                            ImColor(RenderColor.R, RenderColor.G, RenderColor.B, RenderColor.A), RenderText);
    }
}
void Text(const wchar_t* RenderText, const SDK::FVector2D& ScreenPosition, const SDK::FLinearColor& RenderColor,
          const float FontSize, const bool CenteredX, const bool CenteredY, const bool Outlined,
          const float OutlineThickness, const SDK::FLinearColor& OutlineColor) {
    std::wstring wstr = RenderText;
    Text(std::string(wstr.begin(), wstr.end()).c_str(), ScreenPosition, RenderColor, FontSize, CenteredX, CenteredY,
         Outlined, OutlineThickness, OutlineColor);
}

void RectFilled(const SDK::FVector2D& ScreenPosition, const SDK::FVector2D& ScreenSize,
                const SDK::FLinearColor& RenderColor) {
    if (ScreenPosition.X != -1.f && ScreenPosition.Y != -1.f) {
        ImGui::GetBackgroundDrawList()->AddRectFilled(
            ImVec2(ScreenPosition.X, ScreenPosition.Y),
            ImVec2(ScreenPosition.X + ScreenSize.X, ScreenPosition.Y + ScreenSize.Y),
            ImColor(RenderColor.R, RenderColor.G, RenderColor.B, RenderColor.A), 0, 0);
    }
}

void Rect(const SDK::FVector2D& ScreenPosition, const SDK::FVector2D& ScreenSize, const SDK::FLinearColor& RenderColor,
          float Thickness, bool Outlined, float OutlineThickness, const SDK::FLinearColor& OutlineColor) {
    if (ScreenPosition.X != -1.f && ScreenPosition.Y != -1.f) {
        if (Outlined) {
            ImU32 OutlineColor = ImColor(0.f, 0.f, 0.f, RenderColor.A);
            ImGui::GetBackgroundDrawList()->AddRect(
                ImVec2(ScreenPosition.X, ScreenPosition.Y),
                ImVec2(ScreenPosition.X + ScreenSize.X, ScreenPosition.Y + ScreenSize.Y), OutlineColor, 0, 0,
                Thickness + 1.f);
        }
        ImGui::GetBackgroundDrawList()->AddRect(
            ImVec2(ScreenPosition.X, ScreenPosition.Y),
            ImVec2(ScreenPosition.X + ScreenSize.X, ScreenPosition.Y + ScreenSize.Y),
            ImColor(RenderColor.R, RenderColor.G, RenderColor.B, RenderColor.A), 0, 0, Thickness);
    }
}

void CorneredRect(const SDK::FVector2D& ScreenPosition, const SDK::FVector2D& ScreenSize,
                  const SDK::FLinearColor& RenderColor, float Thickness, bool Outlined, float OutlineThickness,
                  const SDK::FLinearColor& OutlineColor) {
    if (ScreenPosition.X != -1.f && ScreenPosition.Y != -1.f) {
        ImVec2 a(ScreenPosition.X, ScreenPosition.Y);                               // Top-left
        ImVec2 b(ScreenPosition.X + ScreenSize.X, ScreenPosition.Y);                // Top-right
        ImVec2 c(ScreenPosition.X + ScreenSize.X, ScreenPosition.Y + ScreenSize.Y); // Bottom-right
        ImVec2 d(ScreenPosition.X, ScreenPosition.Y + ScreenSize.Y);                // Bottom-left

        const float LineW = ScreenSize.X / 4.f;
        const float LineH = ScreenSize.Y / 4.f;

        auto DrawCorner = [&](const ImVec2& Corner, const ImVec2& Start, const ImVec2& End, ImU32 ColorU32,
                              float LineThickness) {
            g_DrawList->PathLineTo(Corner);
            g_DrawList->PathLineTo(Start);
            g_DrawList->PathLineTo(End);
            g_DrawList->PathStroke(ColorU32, 0, LineThickness);
        };

        auto DrawCorneredBox = [&](ImU32 ColorU32, float LineThickness) {
            DrawCorner(ImVec2(a.x + LineW, a.y), a, ImVec2(a.x, a.y + LineH), ColorU32, LineThickness);
            DrawCorner(ImVec2(b.x - LineW, b.y), b, ImVec2(b.x, b.y + LineH), ColorU32, LineThickness);
            DrawCorner(ImVec2(c.x, c.y - LineH), c, ImVec2(c.x - LineW, c.y), ColorU32, LineThickness);
            DrawCorner(ImVec2(d.x + LineW, d.y), d, ImVec2(d.x, d.y - LineH), ColorU32, LineThickness);
        };

        if (Outlined)
            DrawCorneredBox(ImColor(OutlineColor.R, OutlineColor.G, OutlineColor.B, OutlineColor.A),
                            Thickness + OutlineThickness);

        DrawCorneredBox(ImColor(RenderColor.R, RenderColor.G, RenderColor.B, RenderColor.A), Thickness);
    }
}
void Rect3D(const SDK::FVector2D (&BoxCorners)[8], const SDK::FLinearColor& RenderColor, float Thickness, bool Outlined,
            float OutlineThickness, const SDK::FLinearColor& OutlineColor) {
    if (BoxCorners[0].X == -1.f && BoxCorners[0].Y == -1.f)
        return;

    BeginBatchedLines();

    // Bottom
    Line(BoxCorners[0], BoxCorners[1], RenderColor, Thickness, Outlined, OutlineThickness, OutlineColor);
    Line(BoxCorners[1], BoxCorners[3], RenderColor, Thickness, Outlined, OutlineThickness, OutlineColor);
    Line(BoxCorners[3], BoxCorners[2], RenderColor, Thickness, Outlined, OutlineThickness, OutlineColor);
    Line(BoxCorners[2], BoxCorners[0], RenderColor, Thickness, Outlined, OutlineThickness, OutlineColor);

    // Top
    Line(BoxCorners[4], BoxCorners[5], RenderColor, Thickness, Outlined, OutlineThickness, OutlineColor);
    Line(BoxCorners[5], BoxCorners[7], RenderColor, Thickness, Outlined, OutlineThickness, OutlineColor);
    Line(BoxCorners[7], BoxCorners[6], RenderColor, Thickness, Outlined, OutlineThickness, OutlineColor);
    Line(BoxCorners[6], BoxCorners[4], RenderColor, Thickness, Outlined, OutlineThickness, OutlineColor);

    // Sides
    Line(BoxCorners[0], BoxCorners[4], RenderColor, Thickness, Outlined, OutlineThickness, OutlineColor);
    Line(BoxCorners[1], BoxCorners[5], RenderColor, Thickness, Outlined, OutlineThickness, OutlineColor);
    Line(BoxCorners[2], BoxCorners[6], RenderColor, Thickness, Outlined, OutlineThickness, OutlineColor);
    Line(BoxCorners[3], BoxCorners[7], RenderColor, Thickness, Outlined, OutlineThickness, OutlineColor);

    EndBatchedLines();
}

void Circle(SDK::FVector2D ScreenPosition, float Radius, int32_t Segments, const SDK::FLinearColor& RenderColor,
            float Thickness, bool Outlined, float OutlineThickness, const SDK::FLinearColor& OutlineColor) {
    if (Outlined)
        g_DrawList->AddCircle(ImVec2(ScreenPosition.X, ScreenPosition.Y), Radius,
                              ImColor(OutlineColor.R, OutlineColor.G, OutlineColor.B, OutlineColor.A), Segments,
                              Thickness + OutlineThickness);
    g_DrawList->AddCircle(ImVec2(ScreenPosition.X, ScreenPosition.Y), Radius,
                          ImColor(RenderColor.R, RenderColor.G, RenderColor.B, RenderColor.A), Segments, Thickness);
}

} // namespace Drawing
