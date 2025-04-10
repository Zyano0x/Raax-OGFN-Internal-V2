#include "gui.h"
#include "dx11/dx11.h"
#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <cheat/hooks.h>
#include <extern/imgui/imgui.h>
#include <extern/imgui/imgui_impl_win32.h>
#include <extern/imgui/imgui_impl_dx11.h>
#include <utils/log.h>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT __stdcall GUI::h_WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	std::lock_guard<std::recursive_mutex> lock(WndProcMutex);
	if (GUI::SetupImGui)
		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

	if (SetupImGui)
		return o_WndProc(hWnd, uMsg, wParam, lParam);
	return 0;
}


bool GUI::Init() {
	LOG(LOG_TRACE, "Setting up GUI...");
	return GUI::DX11::Init();
}

void GUI::Destroy() {
	LOG(LOG_TRACE, "Destroying GUI...");
	return GUI::DX11::Destroy();
}
