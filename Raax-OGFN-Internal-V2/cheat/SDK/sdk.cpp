#include "sdk.h"
#include <utils/error.h>
#include <utils/memory.h>
#include <utils/log.h>

bool SetupGObjects()
{
    LOG(LOG_TRACE, "Searching for GObjects...");

    // Scan for the first signature, which is for chunked TUObjectArray.
    uintptr_t TUObjectArray = Memory::PatternScan<uint32_t>("48 8B 05 ? ? ? ? 48 8B 0C C8 48 8D 04 D1", 3, true);
    if (Memory::IsAddressInsideModule(TUObjectArray, Memory::GetImageBase()))
    {
        LOG(LOG_INFO, "Found GObjects (Chunked) offset: 0x%p", TUObjectArray - Memory::GetImageBase());
        SDK::UObject::Objects.Initialize(reinterpret_cast<void*>(TUObjectArray), true);
        return true;
    }

    // Scan for the second signature, which is for fixed TUObjectArray (older versions of UE4).
    TUObjectArray = Memory::PatternScan<uint32_t>("48 8B 05 ? ? ? ? 48 8D 14 C8 EB 02", 3, true);
    if (Memory::IsAddressInsideModule(TUObjectArray, Memory::GetImageBase()))
    {
        LOG(LOG_INFO, "Found GObjects (Fixed) offset: 0x%p", TUObjectArray - Memory::GetImageBase());
        SDK::UObject::Objects.Initialize(reinterpret_cast<void*>(TUObjectArray), false);
        return true;
    }

    LOG(LOG_ERROR, "Failed to find GObjects!");
    return false;
}

bool SetupFNameToString()
{
    uintptr_t StringRef = Memory::FindStringRef(L"AController::InitPlayerState: the PlayerStateClass of game mode %s is null, falling back to APlayerState.");
    if (StringRef)
    {
        uintptr_t FNameToString = Memory::PatternScanRange<int32_t>(StringRef, 0x50, "E8", true, 1, true);
        if (FNameToString)
        {
            SDK::FName::FNameToString = reinterpret_cast<decltype(SDK::FName::FNameToString)>(FNameToString);
            return true;
        }
    }
    
    LOG(LOG_ERROR, "Failed to find FNameToString!");
    return false;
}

bool SetupFNameConstructorW()
{
    uintptr_t StringRef = Memory::FindStringRef(L"CanvasObject");
    if (StringRef)
    {
        uintptr_t FNameConstructorW = Memory::PatternScanRange<int32_t>(StringRef, 0x50, "E8", false, 1, true);
        if (FNameConstructorW)
        {
            SDK::FName::FNameConstructorW = reinterpret_cast<decltype(SDK::FName::FNameConstructorW)>(FNameConstructorW);
            return true;
        }
    }

    LOG(LOG_ERROR, "Failed to find FNameConstructorW!");
    return false;
}


bool SDK::Init()
{
    return SetupGObjects() && SetupFNameToString() && SetupFNameConstructorW();
}
