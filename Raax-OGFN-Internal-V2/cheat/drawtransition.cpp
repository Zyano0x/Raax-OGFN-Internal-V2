#include "drawtransition.h"
#include <cheat/sdk/sdk.h>
#include <cheat/hooks.h>
#include <cheat/features/playerfeatures.h>
#include <cheat/core.h>

inline void* g_DrawTransitionFunc = nullptr;

typedef void(__stdcall* t_DrawTransition) (SDK::UGameViewportClient* _this, SDK::UCanvas* Canvas);
inline t_DrawTransition o_DrawTransition = nullptr;
void h_DrawTransition(SDK::UGameViewportClient* _this, SDK::UCanvas* Canvas) {
    std::lock_guard<std::mutex> lock2(Core::GameRenderThreadLock);

    Core::TickGameThread();

    o_DrawTransition(_this, Canvas);
}


bool DrawTransition::Init() {
    g_DrawTransitionFunc = SDK::GetEngine()->GameViewport()->VTable[SDK::UGameViewportClient::DrawTransition_Idx];
    return Hooks::CreateHook(g_DrawTransitionFunc, &h_DrawTransition, reinterpret_cast<void**>(&o_DrawTransition)) && Hooks::EnableHook(g_DrawTransitionFunc);
}

void DrawTransition::Destroy() {
    Hooks::RemoveHook(g_DrawTransitionFunc);
}
