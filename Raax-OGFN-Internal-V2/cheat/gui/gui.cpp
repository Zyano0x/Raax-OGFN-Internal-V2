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
	if (GUI::SetupImGui)
		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

	return CallWindowProc(o_WndProc, hWnd, uMsg, wParam, lParam);
}


bool GUI::Init() {
	return GUI::DX11::Init();
}

void GUI::Destroy() {
	return GUI::DX11::Destroy();
}
