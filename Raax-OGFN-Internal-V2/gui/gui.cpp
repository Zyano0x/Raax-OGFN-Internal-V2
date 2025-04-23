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
	if (GUI::SetupImGui) {
		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
        if (GUI::WindowOpen) {
            // Just copied straight from ImGui. Replace later, cba now.
            switch (uMsg) {
            case WM_MOUSEMOVE:
            case WM_NCMOUSEMOVE:
                //case WM_MOUSELEAVE:
                //case WM_NCMOUSELEAVE:
                //case WM_DESTROY:
            case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
            case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
            case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
            case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
            case WM_LBUTTONUP:
            case WM_RBUTTONUP:
            case WM_MBUTTONUP:
            case WM_XBUTTONUP:
            case WM_MOUSEWHEEL:
            case WM_MOUSEHWHEEL:
            case WM_KEYDOWN:
            case WM_KEYUP:
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
                //case WM_SETFOCUS:
                //case WM_KILLFOCUS:
                //case WM_INPUTLANGCHANGE:
            case WM_CHAR:
                //case WM_SETCURSOR:
                //case WM_DEVICECHANGE:

            case WM_SETCURSOR:
            case WM_INPUT:
                return 0;
            default:
                break;
            }
        }

        return o_WndProc(hWnd, uMsg, wParam, lParam);
	}

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
