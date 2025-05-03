#include "keybind.h"

namespace Keybind {

void TickRenderThread() {
    for (const auto& Keybind : g_Keybinds) {
        if (ImGui::IsKeyPressed(Keybind.Keybind, false) && Keybind.ReflectedBool.Is<bool>()) {
            Keybind.ReflectedBool.As<bool>() = !Keybind.ReflectedBool.As<bool>();
        }
    }
}

std::vector<KeybindData> g_Keybinds;

} // namespace Keybind
