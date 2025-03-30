#include <Windows.h>
#include <globals.h>
#include <cheat/core.h>
#include <cheat/sdk/sdk.h>
#include <utils/log.h>

DWORD Main(LPVOID) {
    LOG(LOG_INFO, "Core::Init() -> %d", Core::Init());

#if 0
    for (int i = 0; i < SDK::UObject::Objects.Num(); i++) {
        SDK::UObject* Obj = SDK::UObject::Objects.GetByIndex(i);
        if (!Obj || !Obj->Class || !Obj->IsDefaultObject() || !Obj->Class->Children())
            continue;

        LOG(LOG_INFO, "Found Object -> %s", Obj->GetFullName().c_str());
        for (SDK::UField* Child = Obj->Class->Children(); Child; Child = Child->Next()) {
            if (Child->HasTypeFlag(SDK::EClassCastFlags::Property))
                LOG(LOG_INFO, "     Prop  -> 0x%X - %s", ((SDK::UProperty*)Child)->Offset(), Child->GetName().c_str());
            else
                LOG(LOG_INFO, "     Child -> %s", Child->GetName().c_str());
        }
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

DWORD Unload(LPVOID hModule) {
    while (true) {
        if (GetAsyncKeyState(VK_F5) & 1) {
            LOG(LOG_INFO, "Unloading...");
            Core::Destroy();
            break;
        }

        Sleep(100);
    }

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
