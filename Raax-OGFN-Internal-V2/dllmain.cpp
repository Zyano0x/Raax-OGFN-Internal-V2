#include <Windows.h>
#include <globals.h>
#include <cheat/core.h>
#include <cheat/sdk/sdk.h>
#include <utils/log.h>
#include <utils/memory.h>

DWORD Main(LPVOID) {
    LOG(LOG_INFO, "Core::Init() -> %d", Core::Init());
    return 0;
}

DWORD Unload(LPVOID hModule) {
    while (true) {
        if (GetAsyncKeyState(VK_F5) & 1) {
            LOG(LOG_INFO, "Unloading...");
            Core::Destroy();
            break;
        }

        Sleep(100);
    }

    Sleep(100);
    FreeLibraryAndExitThread(static_cast<HMODULE>(hModule), 0);
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
#if CFG_MAINTHREAD
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, Main, nullptr, 0, nullptr);
#else
        Main(nullptr);
#endif

#if CFG_UNLOADTHREAD
        CreateThread(nullptr, 0, Unload, hModule, 0, nullptr);
#endif
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
