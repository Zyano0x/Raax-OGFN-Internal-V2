#pragma once

#include "config_reflection.h"
#include <vector>

namespace Keybind {

// --- Keybind Structures --------------------------------------------

struct KeybindData {
    ConfigReflection::ConfigFieldView ReflectedBool = {};
    ImGuiKey Keybind = ImGuiKey_None;
};

// --- Public Tick Functions -----------------------------------------

void TickRenderThread();

// --- Global Variables ----------------------------------------------

extern std::vector<KeybindData> g_Keybinds;

} // namespace Keybind
