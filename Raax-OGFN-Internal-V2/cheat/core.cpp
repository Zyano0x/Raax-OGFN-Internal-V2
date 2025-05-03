#include "core.h"
#include <algorithm>

#include <cheat/sdk/sdk.h>
#include <cheat/hooks.h>
#include <cheat/drawtransition.h>
#include <cheat/tick/playertick.h>
#include <cheat/tick/containertick.h>
#include <cheat/tick/pickuptick.h>
#include <cheat/features/aimbot.h>
#include <config/keybind.h>
#include <globals.h>
#include <gui/gui.h>
#include <utils/log.h>
#include <utils/math.h>

namespace Core {

// --- Utility Functions ---------------------------------------------

void UpdateGlobalVariables() {
    g_ScreenSizeX = SDK::GetCanvas()->SizeX();
    g_ScreenSizeY = SDK::GetCanvas()->SizeY();
    g_FOV = SDK::GetLocalController()->PlayerCameraManager()->GetFOVAngle();
    g_PixelsPerDegree =
        g_ScreenSizeX / Math::RadiansToDegrees((2.f * tan(0.5f * Math::DegreesToRadians(std::clamp(g_FOV, 0.f, 120.f)))));
    g_CameraLocation = SDK::GetCameraLocation();
    g_CameraRotation = SDK::GetCameraRotation();
}

// --- Initialization ------------------------------------------------

bool Init() {
    LOG(LOG_TRACE, "Setting up core...");
#if CFG_USELOGGING
    Log::InitLogger();
#endif
#if CFG_SHOWCONSOLE
    Log::CreateConsole();
#endif
    return Hooks::Init() && SDK::Init() && GUI::Init() && DrawTransition::Init();
}

void Destroy() {
    GUI::Destroy();
    DrawTransition::Destroy();
    Hooks::Destroy();

#if CFG_SHOWCONSOLE
    Log::DestroyConsole();
#endif
}

// --- Public Tick Functions -----------------------------------------

void TickGameThread() {
    UpdateGlobalVariables();

    Tick::Container::TickGameThread();
    Tick::Pickup::TickGameThread();
    Tick::Player::TickGameThread();

    Features::Aimbot::TickGameThread();
}

void TickRenderThread() {
    Tick::Container::TickRenderThread();
    Tick::Pickup::TickRenderThread();
    Tick::Player::TickRenderThread();

    Features::Aimbot::TickRenderThread();

    Keybind::Tick();
}

// --- Global Variables ----------------------------------------------

float         g_PixelsPerDegree = 0.f;
int32_t       g_ScreenSizeX = 0;
int32_t       g_ScreenSizeY = 0;
float         g_FOV = 0.f;
SDK::FVector  g_CameraLocation = SDK::FVector();
SDK::FRotator g_CameraRotation = SDK::FRotator();
std::mutex    g_GameRenderThreadLock;

} // namespace Core
