#include <Windows.h>
#include <globals.h>
#include <cheat/core.h>
#include <cheat/SDK/sdk.h>
#include <utils/log.h>

DWORD Main(LPVOID)
{
    Core::Init();

#if 0
    for (int i = 0; i < SDK::UObject::Objects.Num(); i++)
    {
        SDK::UObject* Obj = SDK::UObject::Objects.GetByIndex(i);
        if (!Obj)
            continue;

        LOG(LOG_INFO, "Found Object -> %s", Obj->GetFullName().c_str());
    }
#endif

#if 0
    SDK::FName test = "hi";
    LOG(LOG_INFO, "%s", test.ToString().c_str());
    test = "hi2";
    LOG(LOG_INFO, "%s", test.ToString().c_str());
#endif

    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
#if CFG_CREATETHREAD
        CreateThread(nullptr, 0, Main, nullptr, 0, nullptr);
#else
        Main(nullptr);
#endif
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
