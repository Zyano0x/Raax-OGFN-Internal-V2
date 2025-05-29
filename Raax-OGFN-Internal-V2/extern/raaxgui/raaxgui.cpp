#include "raaxgui.h"
#include "raaxgui_impl.h"

#include <unordered_map>
#include <memory>
#include <string>

namespace RaaxGUI {
namespace Impl {

// --- Input Data ----------------------------------------------------

struct InputData {
    Impl::Vec2 MousePos;
    bool       Clicking = false;

    void Update() { MousePos = GetMousePos(); }
} g_Input;

struct RaaxStyle {
    Color WindowCol = Color(0.1f, 0.1f, 0.1f, 1.f);

    Color CheckboxCol = Color(0.2f, 0.2f, 0.2f, 1.f);
    Color CheckboxTextCol = Color(1.f, 1.f, 1.f, 1.f);
    Color CheckboxBoxCol = Color(0.2f, 0.8f, 0.2f, 1.f);
    float CheckboxSize = 20.f;
    float CheckboxBoxSize = 10.f;
    float CheckboxTextFontSize = 10.f;
} g_Style;

// --- Classes -------------------------------------------------------

class RaaxElement {
  protected:
    RaaxElement(RaaxElement* parent = nullptr) : m_Parent(parent) {}

  protected:
    RaaxElement* m_Parent;

  public:
    virtual void Render() = 0;
    virtual bool CollidesWith(const Vec2& Pos) = 0;
    virtual void BeginClick(const Vec2& Pos) = 0;
    virtual void TickClick(const Vec2& Pos) = 0;
    virtual void EndClick() = 0;
    virtual Vec2 GetPos() = 0;
};

class RaaxCheckbox final : public RaaxElement {
  public:
    RaaxCheckbox(RaaxElement* Window, const char* Name, bool* pValue, const Vec2& Pos)
        : RaaxElement(Window), m_Name(Name), m_pValue(pValue), m_Clicked(false), m_Pos(Pos),
          m_Size(Vec2(g_Style.CheckboxSize, g_Style.CheckboxSize)) {}

  public:
    void Render() override {
        Vec2 InWindowPos = m_Parent->GetPos() + m_Pos;
        DrawFilledRect(InWindowPos, m_Size, g_Style.CheckboxCol);

        Vec2 BoxPos =
            Vec2(InWindowPos.X + g_Style.CheckboxBoxSize / 2.f, InWindowPos.Y + g_Style.CheckboxBoxSize / 2.f);
        Vec2 BoxSize = Vec2(g_Style.CheckboxBoxSize, g_Style.CheckboxBoxSize);
        if (*m_pValue) {
            DrawFilledRect(BoxPos, BoxSize, g_Style.CheckboxBoxCol);
        }

        Vec2 TextPos = Vec2(BoxPos.X + m_Size.X, BoxPos.Y);
        DrawText(m_Name.c_str(), TextPos, g_Style.CheckboxTextCol, g_Style.CheckboxTextFontSize, false, false, true);
    }
    bool CollidesWith(const Vec2& Pos) override {
        Vec2 InWindowPos = m_Parent->GetPos() + m_Pos;
        return Pos.X >= InWindowPos.X && Pos.Y >= InWindowPos.Y && Pos.X < InWindowPos.X + m_Size.X &&
               Pos.Y < InWindowPos.Y + m_Size.Y;
    }
    void BeginClick(const Vec2& Pos) override { m_Clicked = true; }
    void TickClick(const Vec2& Pos) override {}
    void EndClick() override {
        m_Clicked = false;
        *m_pValue = !*m_pValue;
    }
    Vec2 GetPos() override { return m_Pos; }

  private:
    bool*       m_pValue;
    bool        m_Clicked;
    std::string m_Name;
    Vec2        m_Pos;
    Vec2        m_Size;
};

class RaaxWindow final : public RaaxElement {
  public:
    RaaxWindow(const std::string& Name, const Vec2& Pos, const Vec2& Size)
        : RaaxElement(), m_Name(Name), m_Pos(Pos), m_Size(Size) {}

  public:
    void Render() override {
        if (!m_Open)
            return;

        DrawFilledRect(m_Pos, m_Size, g_Style.WindowCol);

        for (const auto& [_, Elem] : m_Elements) {
            Elem->Render();
        }
    }

    void AddCheckbox(int Id, const char* Name, bool* pValue) {
        if (!m_Elements.contains(Id)) {
            m_Elements[Id] = std::make_unique<RaaxCheckbox>(this, Name, pValue, Vec2(0.f, 0.f));
        }
    }

  public:
    void SetOpenState(bool Value) { m_Open = Value; }
    void SetSize(const Vec2& Size) { m_Size = Size; }

  public:
    bool CollidesWith(const Vec2& Pos) override {
        return Pos.X >= m_Pos.X && Pos.Y >= m_Pos.Y && Pos.X < m_Pos.X + m_Size.X && Pos.Y < m_Pos.Y + m_Size.Y;
    }
    void BeginClick(const Vec2& Pos) override { m_ClickPos = Pos; }
    void TickClick(const Vec2& Pos) override {
        m_Pos.X += Pos.X - m_ClickPos.X;
        m_Pos.Y += Pos.Y - m_ClickPos.Y;
        m_ClickPos = Pos;
    }
    void EndClick() override {}
    Vec2 GetPos() override { return m_Pos; }

    RaaxElement* GetCollidingElement(const Vec2& Pos) {
        for (const auto& [_, Elem] : m_Elements) {
            if (Elem->CollidesWith(Pos)) {
                return Elem.get();
            }
        }
        return CollidesWith(Pos) ? this : nullptr;
    }

  private:
    Vec2 m_ClickPos;

    std::string m_Name;
    Vec2        m_Pos;
    Vec2        m_Size;
    bool        m_Open = false;

    std::unordered_map<int, std::unique_ptr<RaaxElement>> m_Elements;

  public:
    bool SeenThisFrame = false;
};

// --- Global Data ---------------------------------------------------

bool g_PendingNextWindowSize = false;
Vec2 g_NextWindowSize = Vec2();

bool                                                 g_InFrame = false;
std::unordered_map<int, std::unique_ptr<RaaxWindow>> g_Windows;
RaaxElement*                                         g_LastClickedElem = nullptr;
RaaxElement*                                         g_ClickedElem = nullptr;

RaaxWindow* g_CurrentWindow = nullptr;

RaaxElement* TracePos(const Vec2& Pos) {
    for (const auto& [_, Window] : g_Windows) {
        if (RaaxElement* Elem = Window->GetCollidingElement(Pos)) {
            return Elem;
        }
    }
    return nullptr;
}

void HandleInput() {
    if (WasKeyJustPressed(KeyID::LeftMouseButton)) {
        g_Input.Clicking = true;

        RaaxElement* Elem = TracePos(g_Input.MousePos);
        if (Elem) {
            g_ClickedElem = Elem;
            g_ClickedElem->BeginClick(g_Input.MousePos);
        }
    } else if (g_Input.Clicking) {
        if (WasKeyJustReleased(KeyID::LeftMouseButton)) {
            if (g_ClickedElem) {
                g_ClickedElem->EndClick();
            }

            g_Input.Clicking = false;
            g_LastClickedElem = g_ClickedElem;
            g_ClickedElem = nullptr;
        } else if (g_ClickedElem) {
            g_ClickedElem->TickClick(g_Input.MousePos);
        }
    }
}

// --- Static Data ---------------------------------------------------

constexpr Vec2 g_DefaultWindowPos = Vec2(100.f, 100.f);
constexpr Vec2 g_DefaultWindowSize = Vec2(250.f, 250.f);

// --- Hashing Functions ---------------------------------------------

int HashString(const wchar_t* String) {
    int Hash = 0;

    for (int i = 0; i < wcslen(String); i++) {
        Hash += String[i];
    }

    return Hash;
}
int HashString(const char* String) {
    int Hash = 0;

    for (int i = 0; i < strlen(String); i++) {
        Hash += String[i];
    }

    return Hash;
}

// --- Private GUI Functions -----------------------------------------

void ResetWindowSeenFlags() {
    for (const auto& [_, Window] : g_Windows) {
        Window->SeenThisFrame = false;
    }
}

void RemoveUnseenWindows() {
    for (auto It = g_Windows.begin(); It != g_Windows.end();) {
        RaaxWindow* Window = It->second.get();
        if (!Window->SeenThisFrame) {
            It = g_Windows.erase(It);
        } else {
            ++It;
        }
    }
}

RaaxWindow* FindOrCreateWindow(int Id, const char* Name) {
    if (!g_Windows.contains(Id)) {
        g_Windows[Id] = std::make_unique<RaaxWindow>(Name, g_DefaultWindowPos, g_DefaultWindowSize);
    }
    return g_Windows.at(Id).get();
}

} // namespace Impl

// --- Public GUI Functions ------------------------------------------

void SetNextWindowSize(const Impl::Vec2& Size) {
    Impl::g_PendingNextWindowSize = true;
    Impl::g_NextWindowSize = Size;
}

bool Begin(const char* Name, bool* pOpen) {
    int               Id = Impl::HashString(Name);
    Impl::RaaxWindow* Window = Impl::FindOrCreateWindow(Id, Name);

    bool Open = pOpen ? *pOpen : true;
    Window->SetOpenState(Open);
    Window->SeenThisFrame = true;

    if (Impl::g_PendingNextWindowSize) {
        Window->SetSize(Impl::g_NextWindowSize);
        Impl::g_PendingNextWindowSize = false;
    }

    Impl::g_CurrentWindow = Window;
    return Open;
}

void End() {
    Impl::g_CurrentWindow = nullptr;
}

void Checkbox(const char* Name, bool* pValue) {
    int Id = Impl::HashString(Name);
    if (!Impl::g_CurrentWindow)
        Impl::ThrowError("Attempted to create a checkbox whilst not in a window!");

    Impl::g_CurrentWindow->AddCheckbox(Id, Name, pValue);
}

// --- Public Tick Functions -----------------------------------------

void NewFrame() {
    if (Impl::g_InFrame)
        Impl::ThrowError("Attempted to create new frame during a frame! Missing EndFrame()?");
    Impl::g_InFrame = true;

    Impl::g_Input.Update();
    Impl::HandleInput();

    Impl::ResetWindowSeenFlags();
}
void EndFrame() {
    if (!Impl::g_InFrame)
        Impl::ThrowError("Attempted to end a frame whilst not in a frame! Missing NewFrame()?");
    Impl::g_InFrame = false;

    if (Impl::g_CurrentWindow)
        Impl::ThrowError("Attempted to end a frame whilst in a window! Missing End()?");

    Impl::RemoveUnseenWindows();

    for (const auto& [_, Window] : Impl::g_Windows) {
        Window->Render();
    }
}

} // namespace RaaxGUI
