#include "raaxgui_window.h"
#include "raaxgui_globals.h"
#include "raaxgui_elements.h"

namespace RaaxGUI {
namespace Impl {

RaaxWindow::RaaxWindow(const std::string& Name, const Vec2& Pos, const Vec2& Size)
    : RaaxElement(), m_Name(Name), m_Pos(Pos), m_Size(Size) {}

void RaaxWindow::Render() {
    if (!m_Open)
        return;

    // If we get rendered then we are in a new frame and can reset the current element position.
    m_CurrentElementPos = Vec2();

    DrawFilledRect(m_Pos, m_Size, g_Style.WindowCol);

    for (const auto& [_, Elem] : m_Elements) {
        Elem->Render();
    }
}

void RaaxWindow::AddCheckbox(int Id, const char* Name, bool* pValue) {
    if (!m_Elements.contains(Id)) {
        m_Elements[Id] = std::make_unique<RaaxCheckbox>(this, Name, pValue);
    }
}
void RaaxWindow::AddSliderInt(int Id, const char* Name, int* pValue, int Min, int Max) {
    if (!m_Elements.contains(Id)) {
        m_Elements[Id] = std::make_unique<RaaxSlider<int>>(this, Name, pValue, Min, Max);
    }
}
void RaaxWindow::AddSliderFloat(int Id, const char* Name, float* pValue, float Min, float Max) {
    if (!m_Elements.contains(Id)) {
        m_Elements[Id] = std::make_unique<RaaxSlider<float>>(this, Name, pValue, Min, Max);
    }
}

Vec2 RaaxWindow::GetNextElementPos(const Vec2& ElementSize) {
    m_CurrentElementPos.Y += g_Style.ElementSpacing.Y;

    Vec2 Result = m_CurrentElementPos + m_Pos;
    m_CurrentElementPos.Y += ElementSize.Y;
    return Result;
}

RaaxElement* RaaxWindow::GetCollidingElement(const Vec2& Pos) {
    for (const auto& [_, Elem] : m_Elements) {
        if (Elem->CollidesWith(Pos)) {
            return Elem.get();
        }
    }
    return CollidesWith(Pos) ? this : nullptr;
}

} // namespace Impl
} // namespace RaaxGUI
