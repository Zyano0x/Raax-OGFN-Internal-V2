#pragma once
#include <Windows.h>
#include <mutex>

namespace GUI
{
    bool Init();
    void Destroy();

    using t_WndProc = LRESULT(*)(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    inline t_WndProc o_WndProc = nullptr;
    LRESULT __stdcall h_WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    inline std::recursive_mutex WndProcMutex;
    inline bool SetupImGui = false;
}
