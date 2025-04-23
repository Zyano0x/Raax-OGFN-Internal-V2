#pragma once
#include <mutex>

namespace Core
{
    bool Init();
    void Destroy();

    void TickGameThread();
    void TickRenderThread();

    inline float ScreenSizeX = 0.f;
    inline float ScreenSizeY = 0.f;
    inline std::mutex GameRenderThreadLock; // Prevents game-thread stuff from running when render-thread stuff is running. Untesed, maybe dead lock???
}
