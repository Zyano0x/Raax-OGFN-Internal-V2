#pragma once

#include <Windows.h>
#include <mutex>

#include <extern/imgui/imgui.h>

namespace GUI {

// --- Initialization ------------------------------------------------

bool Init();
void Destroy();

// --- ImGui Utility Functions ---------------------------------------

void Keybind(const char* Str, bool& WaitingForKeybind, ImGuiKey& OutKeybind);

// --- WndProc Hook --------------------------------------------------

using t_WndProc = LRESULT (*)(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
inline t_WndProc o_WndProc = nullptr;
LRESULT __stdcall h_WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// --- Global Variables ----------------------------------------------

extern std::recursive_mutex g_WndProcMutex;
extern bool                 g_SetupImGui;

} // namespace GUI
