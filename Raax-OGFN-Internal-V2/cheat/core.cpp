#include "core.h"
#include <algorithm>

#include <cheat/autorevertfeature.h>
#include <cheat/sdk/sdk.h>
#include <cheat/hooks.h>
#include <cheat/drawtransition.h>
#include <cheat/tick/playertick.h>
#include <cheat/tick/containertick.h>
#include <cheat/tick/pickuptick.h>
#include <cheat/features/weaponutils.h>
#include <cheat/features/radar.h>
#include <cheat/features/aimbot.h>
#include <cheat/features/triggerbot.h>
#include <cheat/features/exploits.h>
#include <drawing/drawing.h>
#include <config/keybind.h>
#include <globals.h>
#include <gui/mainwindow.h>
#include <gui/gui.h>
#include <utils/log.h>
#include <utils/math.h>
#include <filesystem>

namespace Core {

// --- Utility Functions ---------------------------------------------

SDK::UWorld* GetNewWorld() {
    SDK::UEngine* Engine = SDK::GetEngine();
    if (!Engine)
        return nullptr;

    SDK::UGameViewportClient* GameViewport = Engine->GameViewport;
    if (!GameViewport)
        return nullptr;

    return GameViewport->World;
}

SDK::ULocalPlayer* GetNewLocalPlayer() {
    SDK::UEngine* Engine = SDK::GetEngine();
    if (!Engine)
        return nullptr;

    SDK::UGameViewportClient* GameViewport = Engine->GameViewport;
    if (!GameViewport)
        return nullptr;

    SDK::UGameInstance* GameInstance = GameViewport->GameInstance;
    if (!GameInstance)
        return nullptr;

    SDK::TArray<SDK::ULocalPlayer*>& LocalPlayers = GameInstance->LocalPlayers;
    if (!LocalPlayers.IsValid())
        return nullptr;

    return LocalPlayers.GetByIndex(0);
}

SDK::APlayerController* GetNewPlayerController() {
    if (!g_LocalPlayer)
        return nullptr;

    return g_LocalPlayer->PlayerController;
}

SDK::APlayerCameraManager* GetNewPlayerCameraManager() {
    if (!g_LocalPlayerController)
        return nullptr;

    return g_LocalPlayerController->PlayerCameraManager;
}

SDK::APawn* GetNewPawn() {
    if (!g_LocalPlayerController)
        return nullptr;

    return g_LocalPlayerController->AcknowledgedPawn;
}

float GetNewCameraFOV() {
    if (!g_LocalPlayerCameraManager)
        return 0.f;

    return g_LocalPlayerCameraManager->GetFOVAngle();
}
SDK::FVector GetNewCameraLocation() {
    if (!g_LocalPlayerCameraManager)
        return {};

    return g_LocalPlayerCameraManager->GetCameraLocation();
}
SDK::FRotator GetNewCameraRotation() {
    if (!g_LocalPlayerCameraManager)
        return {};

    return g_LocalPlayerCameraManager->GetCameraRotation();
}

void UpdateGlobalVariables() {
    g_World = GetNewWorld();
    g_LocalPlayer = GetNewLocalPlayer();
    g_LocalPlayerController = GetNewPlayerController();
    g_LocalPlayerCameraManager = GetNewPlayerCameraManager();
    g_LocalPawn = GetNewPawn();

    SDK::UCanvas* Canvas = SDK::GetCanvas();
    if (Canvas) {
        g_ScreenSizeX = SDK::GetCanvas()->SizeX;
        g_ScreenSizeY = SDK::GetCanvas()->SizeY;
        g_ScreenCenterX = static_cast<int32_t>(static_cast<float>(g_ScreenSizeX) / 2.f);
        g_ScreenCenterY = static_cast<int32_t>(static_cast<float>(g_ScreenSizeY) / 2.f);
    }

    g_CameraLocation = GetNewCameraLocation();
    g_CameraRotation = GetNewCameraRotation();
    g_FOV = GetNewCameraFOV();

    g_PixelsPerDegree = g_ScreenSizeX / Math::RadiansToDegrees(
                                            (2.f * tan(0.5f * Math::DegreesToRadians(std::clamp(g_FOV, 0.f, 120.f)))));

    if (g_LocalPawn) {
        SDK::USceneComponent* RootComponent = g_LocalPawn->RootComponent;
        if (RootComponent) {
            g_LocalPlayerPos = RootComponent->RelativeLocation;
        }

        SDK::AFortPlayerStateAthena* PlayerState = SDK::Cast<SDK::AFortPlayerStateAthena>(g_LocalPawn->PlayerState);
        if (PlayerState) {
            g_LocalTeamIndex = PlayerState->TeamIndex;
        }
    }
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
    AutoRevertFeature::Destroy();
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

    AutoRevertFeature::Tick();

    Tick::Container::TickGameThread();
    Tick::Pickup::TickGameThread();
    Tick::Player::TickGameThread();

    Features::WeaponUtils::TickGameThread();

    Features::Aimbot::TickGameThread();
    Features::TriggerBot::TickGameThread();

    Features::Exploits::TickGameThread();

    // For engine builds, render and game thread is merged since rendering happens at DrawTransition ticks, which is
    // when TickGameThread is triggered
#ifdef _ENGINE
    TickRenderThread();
#endif
}

void TickRenderThread() {
#ifdef _ENGINE
    static bool Init = false;
    if (!Init) {
        Drawing::Init();
        Init = true;
    }
#endif
    Drawing::Tick();

    Tick::Container::TickRenderThread();
    Tick::Pickup::TickRenderThread();
    Tick::Player::TickRenderThread();

    Features::Radar::TickRenderThread();
    Features::Aimbot::TickRenderThread();
    Features::TriggerBot::TickRenderThread();

    Keybind::Tick();

    GUI::MainWindow::Tick();
}

// --- Global Variables ----------------------------------------------

SDK::UWorld*               g_World = nullptr;
SDK::ULocalPlayer*         g_LocalPlayer = nullptr;
SDK::APlayerController*    g_LocalPlayerController = nullptr;
SDK::APlayerCameraManager* g_LocalPlayerCameraManager = nullptr;
SDK::APawn*                g_LocalPawn = nullptr;
uint8_t                    g_LocalTeamIndex = 0;

int32_t g_ScreenSizeX = 0;
int32_t g_ScreenSizeY = 0;
int32_t g_ScreenCenterX = 0;
int32_t g_ScreenCenterY = 0;
float   g_PixelsPerDegree = 0.f;

SDK::FVector  g_CameraLocation = SDK::FVector();
SDK::FRotator g_CameraRotation = SDK::FRotator();
float         g_FOV = 0.f;

SDK::FVector g_LocalPlayerPos = SDK::FVector();

std::mutex g_GameRenderThreadLock;

} // namespace Core
