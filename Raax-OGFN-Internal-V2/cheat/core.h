#pragma once

#include <mutex>
#include <cheat/sdk/Basic.h>

namespace Core {

// --- Initialization ------------------------------------------------

bool Init();
void Destroy();

// --- Public Tick Functions -----------------------------------------

void TickGameThread();
void TickRenderThread();

// --- Global Variables ----------------------------------------------

extern float         g_PixelsPerDegree;
extern int32_t       g_ScreenSizeX;
extern int32_t       g_ScreenSizeY;
extern float         g_FOV;
extern SDK::FVector  g_CameraLocation;
extern SDK::FRotator g_CameraRotation;
extern std::mutex    g_GameRenderThreadLock;

} // namespace Core
