#include "dx11.h"
#include "../gui.h"
#include <Windows.h>
#include <dxgi.h>
#include <d3d11.h>
#include <cheat/hooks.h>
#include <extern/imgui/imgui.h>
#include <extern/imgui/imgui_impl_win32.h>
#include <extern/imgui/imgui_impl_dx11.h>
#include <drawing/drawing.h>
#include <cheat/core.h>

#pragma comment(lib, "d3d11.lib")

// One of the messiest files in the project, I hate this file.
// It works so I don't care tho.

inline IDXGISwapChain* g_SwapChain;
inline ID3D11Device* g_Device;
inline ID3D11DeviceContext* g_DeviceContext;
inline ID3D11RenderTargetView* g_RenderTargetView;
inline HWND g_Window = nullptr;
inline void* g_PresentFunc = nullptr;
inline void* g_ResizeBuffersFunc = nullptr;
inline void* g_WndProc = nullptr;
inline bool g_PendingShutdown = false;
inline bool g_Hooked = false;

typedef HRESULT(__stdcall* t_Present) (IDXGISwapChain* SwapChain, UINT SyncInterval, UINT Flags);
inline t_Present o_Present = nullptr;
HRESULT __stdcall h_Present(IDXGISwapChain* SwapChain, UINT SyncInterval, UINT Flags) {
	std::lock_guard<std::recursive_mutex> lock(GUI::WndProcMutex);
	std::lock_guard<std::mutex> lock2(Core::GameRenderThreadLock);

	if (!GUI::SetupImGui) {
		GUI::SetupImGui = true;

		if (SUCCEEDED(SwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&g_Device))) {
			g_Device->GetImmediateContext(&g_DeviceContext);

			DXGI_SWAP_CHAIN_DESC Desc = {};
			SwapChain->GetDesc(&Desc);
			g_Window = Desc.OutputWindow;

			ID3D11Texture2D* BackBuffer = nullptr;
			SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&BackBuffer);
			g_Device->CreateRenderTargetView(BackBuffer, NULL, &g_RenderTargetView);
			BackBuffer->Release();

			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;

			Drawing::Init();

			ImGui_ImplWin32_Init(g_Window);
			ImGui_ImplDX11_Init(g_Device, g_DeviceContext);

			g_WndProc = (void*)GetWindowLongPtrW(g_Window, GWLP_WNDPROC);
			Hooks::CreateHook(g_WndProc, &GUI::h_WndProc, (void**)&GUI::o_WndProc);
			Hooks::EnableHook(g_WndProc);
		}
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	Drawing::Tick();

	Core::TickRenderThread();
	GUI::TickMainWindow();

	ImGui::EndFrame();

	ImGui::Render();

	g_DeviceContext->OMSetRenderTargets(1, &g_RenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (g_PendingShutdown) {
		GUI::SetupImGui = false;

		Hooks::RemoveHook(g_PresentFunc);
		Hooks::RemoveHook(g_ResizeBuffersFunc);
		Hooks::RemoveHook(g_WndProc);

		ImGui_ImplDX11_CustomShutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		g_DeviceContext = nullptr;
		g_Device = nullptr;
		g_SwapChain = nullptr;

		g_PendingShutdown = false;
		return 0;
	}

	return o_Present(SwapChain, SyncInterval, Flags);
}

using t_ResizeBuffers = HRESULT(*)(IDXGISwapChain* pThis, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
inline t_ResizeBuffers o_ResizeBuffers = nullptr;
HRESULT __stdcall h_ResizeBuffers(IDXGISwapChain* pThis, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) {
	if (g_RenderTargetView) {
		g_DeviceContext->OMSetRenderTargets(0, 0, 0);
		g_RenderTargetView->Release();
	}

	HRESULT Result = o_ResizeBuffers(pThis, BufferCount, Width, Height, NewFormat, SwapChainFlags);

	ID3D11Texture2D* Buffer = nullptr;
	pThis->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&Buffer);

	g_Device->CreateRenderTargetView(Buffer, NULL, &g_RenderTargetView);
	Buffer->Release();

	g_DeviceContext->OMSetRenderTargets(1, &g_RenderTargetView, NULL);

	D3D11_VIEWPORT vp = {};
	vp.Width = Width;
	vp.Height = Height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_DeviceContext->RSSetViewports(1, &vp);

	return Result;
}


bool GUI::DX11::Init() {
	HMODULE D3D11 = GetModuleHandleA("d3d11.dll");
	if (!D3D11)
		return false;

	D3D_FEATURE_LEVEL FeatureLevel = D3D_FEATURE_LEVEL_11_0;
	const D3D_FEATURE_LEVEL FeatureLevels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_11_1 };

	DXGI_RATIONAL RefreshRate = {};
	RefreshRate.Numerator = 60;
	RefreshRate.Denominator = 1;

	DXGI_MODE_DESC BufferDesc = {};
	BufferDesc.Width = 100;
	BufferDesc.Height = 100;
	BufferDesc.RefreshRate = RefreshRate;
	BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	DXGI_SAMPLE_DESC SampleDesc = {};
	SampleDesc.Count = 1;
	SampleDesc.Quality = 0;

	DXGI_SWAP_CHAIN_DESC SwapChainDesc = {};
	SwapChainDesc.BufferDesc = BufferDesc;
	SwapChainDesc.SampleDesc = SampleDesc;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.BufferCount = 1;
	SwapChainDesc.OutputWindow = GetForegroundWindow();
	SwapChainDesc.Windowed = 1;
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ID3D11Device* Device = nullptr;
	ID3D11DeviceContext* DeviceContext = nullptr;
	IDXGISwapChain* SwapChain = nullptr;

	HRESULT Result = ((HRESULT(*)(IDXGIAdapter*, D3D_DRIVER_TYPE, HMODULE, UINT, const D3D_FEATURE_LEVEL*, UINT, UINT, const DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**, ID3D11Device**, D3D_FEATURE_LEVEL*, ID3D11DeviceContext**))D3D11CreateDeviceAndSwapChain)(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, FeatureLevels, 1, D3D11_SDK_VERSION, &SwapChainDesc, &SwapChain, &Device, &FeatureLevel, &DeviceContext);
	if (FAILED(Result))
		return false;

	void** VFT = *(void***)SwapChain;
	g_PresentFunc = VFT[8];
	g_ResizeBuffersFunc = VFT[13];

	SwapChain->Release();
	Device->Release();
	DeviceContext->Release();

	g_Hooked = Hooks::CreateHook(g_PresentFunc, h_Present, reinterpret_cast<void**>(&o_Present)) && Hooks::EnableHook(g_PresentFunc) &&
		Hooks::CreateHook(g_ResizeBuffersFunc, h_ResizeBuffers, reinterpret_cast<void**>(&o_ResizeBuffers)) && Hooks::EnableHook(g_ResizeBuffersFunc);
	return g_Hooked;
}

void GUI::DX11::Destroy() {
	// Spin until h_Present has finished...
	if (g_Hooked) {
		g_PendingShutdown = true;
		while (g_PendingShutdown) {
			Sleep(100);
		}
	}
}
