#include "raaxgui_window.h"
#include "raaxgui_globals.h"
#include "raaxgui_elements.h"

namespace RaaxGUI {
namespace Impl {

RaaxWindow::RaaxWindow(const std::string& Name, const Vec2& Pos, const Vec2& Size)
    : RaaxElement(), m_IsFirstElement(false), m_Name(Name), m_Pos(Pos), m_Size(Size) {}

void RaaxWindow::Render() {
    if (!m_Open)
        return;

    // If we get rendered then we are in a new frame and can reset the current element position.
    m_CurrentElementPos = g_Style.ElementSpacing;
    m_IsFirstElement = true;

    DrawFilledRect(m_Pos, m_Size, g_Style.WindowCol);

    RemoveUnseenElements();
    for (auto& [_, Elem] : m_Elements) {
        Elem.first = false;
        Elem.second->Render();
    }
}

void RaaxWindow::RemoveUnseenElements() {
    std::erase_if(m_Elements, [](const auto& Cache) { return !Cache.second.first; });
}

RaaxElement* RaaxWindow::AddButton(int Id, const char* Name) {
    if (!m_Elements.contains(Id))
        m_Elements[Id] = std::make_pair(true, std::make_unique<RaaxButton>(this, Name));

    auto& It = m_Elements[Id];
    auto  Elem = static_cast<RaaxButton*>(It.second.get());
    It.first = true;
    return Elem;
}
RaaxElement* RaaxWindow::AddCheckbox(int Id, const char* Name, bool* pValue) {
    if (!m_Elements.contains(Id))
        m_Elements[Id] = std::make_pair(true, std::make_unique<RaaxCheckbox>(this, Name, pValue));

    auto& It = m_Elements[Id];
    auto  Elem = static_cast<RaaxCheckbox*>(It.second.get());
    It.first = true;
    Elem->Update(pValue);
    return Elem;
}
RaaxElement* RaaxWindow::AddSliderInt(int Id, const char* Name, int* pValue, int Min, int Max) {
    if (!m_Elements.contains(Id))
        m_Elements[Id] = std::make_pair(true, std::make_unique<RaaxSlider<int>>(this, Name, pValue, Min, Max));

    auto& It = m_Elements[Id];
    auto  Elem = static_cast<RaaxSlider<int>*>(It.second.get());
    It.first = true;
    Elem->Update(pValue, Min, Max);
    return Elem;
}
RaaxElement* RaaxWindow::AddSliderFloat(int Id, const char* Name, float* pValue, float Min, float Max) {
    if (!m_Elements.contains(Id))
        m_Elements[Id] = std::make_pair(true, std::make_unique<RaaxSlider<float>>(this, Name, pValue, Min, Max));

    auto& It = m_Elements[Id];
    auto  Elem = static_cast<RaaxSlider<float>*>(It.second.get());
    It.first = true;
    Elem->Update(pValue, Min, Max);
    return Elem;
}

Vec2 RaaxWindow::GetNextElementPos(const Vec2& ElementSize, bool SameLine) {
    if (SameLine) {
        m_CurrentElementPos.X += g_Style.ElementSpacing.X + ElementSize.X;
    } else {
        m_CurrentElementPos.X = g_Style.ElementSpacing.X;
        if (!m_IsFirstElement)
            m_CurrentElementPos.Y += g_Style.ElementSpacing.Y + ElementSize.Y;
    }

    m_IsFirstElement = false;
    return m_CurrentElementPos + m_Pos;
}

RaaxElement* RaaxWindow::GetCollidingElement(const Vec2& Pos) {
    for (const auto& [_, Elem] : m_Elements) {
        if (Elem.second->CollidesWith(Pos)) {
            return Elem.second.get();
        }
    }
    return CollidesWith(Pos) ? this : nullptr;
}

} // namespace Impl
} // namespace RaaxGUI
