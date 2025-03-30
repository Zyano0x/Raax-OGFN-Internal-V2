#include "sdk.h"
#include <utils/error.h>
#include <utils/memory.h>
#include <utils/log.h>

// Unreal-Engine general offsets...

bool SetupGObjects() {
    LOG(LOG_TRACE, "Searching for GObjects...");

    static const std::vector<std::pair<const std::string, const std::string>> Patterns = {
        { "Chunked", "48 8B 05 ? ? ? ? 48 8B 0C C8 48 8D 04 D1" },
        { "Fixed", "48 8B 05 ? ? ? ? 48 8D 14 C8 EB 02" }
    };

    for (const auto& [Type, Pattern] : Patterns) {
        uintptr_t Address = Memory::PatternScan<uint32_t>(Pattern.c_str(), 3, true);
        if (Memory::IsAddressInsideImage(Address)) {
            LOG(LOG_INFO, "Found GObjects (%s) offset: 0x%p", Type.c_str(), Address - Memory::GetImageBase());
            SDK::UObject::Objects.Initialize(reinterpret_cast<void*>(Address), Type == "Chunked");
            return true;
        }
    }

    LOG(LOG_ERROR, "Failed to find GObjects!");
    return false;
}
bool SetupFNameToString() {
    uintptr_t StringRef = Memory::FindStringRef(L"AController::InitPlayerState: the PlayerStateClass of game mode %s is null, falling back to APlayerState.");
    if (StringRef) {
        uintptr_t FNameToString = Memory::PatternScanRange<int32_t>(StringRef, 0x50, "E8", true, 1, true);
        if (FNameToString) {
            SDK::FName::FNameToString = reinterpret_cast<decltype(SDK::FName::FNameToString)>(FNameToString);
            LOG(LOG_INFO, "Found FNameToString offset: 0x%p", reinterpret_cast<uintptr_t>(SDK::FName::FNameToString) - Memory::GetImageBase());
            return true;
        }
    }
    
    LOG(LOG_ERROR, "Failed to find FNameToString!");
    return false;
}
bool SetupFNameConstructorW() {
    uintptr_t StringRef = Memory::FindStringRef(L"CanvasObject");
    if (StringRef) {
        uintptr_t FNameConstructorW = Memory::PatternScanRange<int32_t>(StringRef, 0x50, "E8", false, 1, true);
        if (FNameConstructorW) {
            SDK::FName::FNameConstructorW = reinterpret_cast<decltype(SDK::FName::FNameConstructorW)>(FNameConstructorW);
            LOG(LOG_INFO, "Found FNameConstructorW offset: 0x%p", reinterpret_cast<uintptr_t>(SDK::FName::FNameConstructorW) - Memory::GetImageBase());
            return true;
        }
    }

    LOG(LOG_ERROR, "Failed to find FNameConstructorW!");
    return false;
}

bool SetupUnrealGeneralOffsets() {
    return SetupGObjects() &&
        SetupFNameToString() &&
        SetupFNameConstructorW();
}


// Unreal-Engine struct/class offsets...

bool Setup_UProperty_Offset_Internal() {
    SDK::UProperty::Offset_Internal_Offset = 0x44;
    LOG(LOG_INFO, "Using hardcoded UProperty::Offset_Internal offset: 0x%X", SDK::UProperty::Offset_Internal_Offset);
    return true;
}
bool Setup_UClass_ClassCastFlags() {
    std::vector<std::pair<void*, SDK::EClassCastFlags>> Pairs = {
        { SDK::UObject::FindObjectFast("Actor"), SDK::EClassCastFlags::Actor },
        { SDK::UObject::FindObjectFast("Class"), SDK::EClassCastFlags::Field | SDK::EClassCastFlags::Struct | SDK::EClassCastFlags::Class }
    };

    SDK::UClass::ClassCastFlags_Offset = Memory::FindMatchingValueOffset(Pairs);
    if (SDK::UClass::ClassCastFlags_Offset) {
        LOG(LOG_INFO, "Found UClass::ClassCastFlags offset: 0x%X", SDK::UClass::ClassCastFlags_Offset);
        return true;
    }

    LOG(LOG_ERROR, "Failed to find UClass::ClassCastFlags offset!");
    return false;
}
bool Setup_UClass_ClassDefaultObject() {
    std::vector<std::pair<void*, void*>> Pairs = {
        { SDK::UObject::FindObjectFast("Object"), SDK::UObject::FindObjectFast("Default__Object") },
        { SDK::UObject::FindObjectFast("Field"), SDK::UObject::FindObjectFast("Default__Field") }
    };

    SDK::UClass::ClassDefaultObject_Offset = Memory::FindMatchingValueOffset(Pairs);
    if (SDK::UClass::ClassDefaultObject_Offset) {
        LOG(LOG_INFO, "Found UClass::ClassDefaultObject offset: 0x%X", SDK::UClass::ClassDefaultObject_Offset);
        return true;
    }

    LOG(LOG_ERROR, "Failed to find UClass::ClassDefaultObject offset!");
    return false;
}
bool Setup_UStruct_SuperStruct() {
    std::vector<std::pair<void*, void*>> Pairs = {
        { SDK::UObject::FindObjectFast("Struct"), SDK::UObject::FindObjectFast("Field") },
        { SDK::UObject::FindObjectFast("Class"), SDK::UObject::FindObjectFast("Struct") }
    };

    if (!Pairs[0].first)
        Pairs[0].first = Pairs[1].second = SDK::UObject::FindObjectFast("struct");

    SDK::UStruct::SuperStruct_Offset = Memory::FindMatchingValueOffset(Pairs);
    if (SDK::UStruct::SuperStruct_Offset) {
        LOG(LOG_INFO, "Found UStruct::SuperStruct offset: 0x%X", SDK::UStruct::SuperStruct_Offset);
        return true;
    }

    LOG(LOG_ERROR, "Failed to find UStruct::SuperStruct offset!");
    return false;
}
bool Setup_UStruct_Children() {
    std::vector<std::pair<void*, void*>> Pairs = {
        { SDK::UObject::FindObjectFast("PlayerController"), SDK::UObject::FindObjectFastWithOuter("WasInputKeyJustReleased", "PlayerController") },
        { SDK::UObject::FindObjectFast("Controller"), SDK::UObject::FindObjectFastWithOuter("UnPossess", "Controller") }
    };

    SDK::UStruct::Children_Offset = Memory::FindMatchingValueOffset(Pairs);
    if (!SDK::UStruct::Children_Offset) {
        Pairs = {
            { SDK::UObject::FindObjectFast("Vector"), SDK::UObject::FindObjectFastWithOuter("X", "Vector") },
            { SDK::UObject::FindObjectFast("Vector4"), SDK::UObject::FindObjectFastWithOuter("X", "Vector4") },
            { SDK::UObject::FindObjectFast("Vector2D"), SDK::UObject::FindObjectFastWithOuter("X", "Vector2D") },
            { SDK::UObject::FindObjectFast("Guid"), SDK::UObject::FindObjectFastWithOuter("A", "Guid") }
        };

        SDK::UStruct::Children_Offset = Memory::FindMatchingValueOffset(Pairs);
    }

    if (SDK::UStruct::Children_Offset) {
        LOG(LOG_INFO, "Found UStruct::Children offset: 0x%X", SDK::UStruct::Children_Offset);
        return true;
    }

    LOG(LOG_ERROR, "Failed to find UStruct::Children offset!");
    return false;
}
bool Setup_UField_Next() {
    std::vector<std::pair<void*, void*>> Pairs = {
        { SDK::UObject::FindObjectFast<SDK::UClass>("KismetMathLibrary", SDK::EClassCastFlags::Class)->Children(), SDK::UObject::FindObjectFast<SDK::UClass>("Xor_Int64Int64", SDK::EClassCastFlags::Function) }
    };

    SDK::UField::Next_Offset = Memory::FindMatchingValueOffset(Pairs, 0x10, 0x40);
    if (SDK::UField::Next_Offset) {
        LOG(LOG_INFO, "Found UField::Next offset: 0x%X", SDK::UField::Next_Offset);
        return true;
    }

    LOG(LOG_ERROR, "Failed to find UField::Next offset!");
    return false;
}
bool Setup_UFunction_FunctionFlags() {
    std::vector<std::pair<void*, SDK::EFunctionFlags>> Pairs = {
        { SDK::UObject::FindObjectFast("WasInputKeyJustPressed"), SDK::EFunctionFlags::Final | SDK::EFunctionFlags::Native | SDK::EFunctionFlags::Public | SDK::EFunctionFlags::BlueprintCallable | SDK::EFunctionFlags::BlueprintPure | SDK::EFunctionFlags::Const },
        { SDK::UObject::FindObjectFast("ToggleSpeaking"), SDK::EFunctionFlags::Exec | SDK::EFunctionFlags::Native | SDK::EFunctionFlags::Public },
        { SDK::UObject::FindObjectFast("SwitchLevel"), SDK::EFunctionFlags::Exec | SDK::EFunctionFlags::Native | SDK::EFunctionFlags::Public }
    };

    SDK::UFunction::FunctionFlags_Offset = Memory::FindMatchingValueOffset(Pairs);
    if (!SDK::UFunction::FunctionFlags_Offset) {
        for (auto& [_, Flags] : Pairs)
            Flags = Flags | SDK::EFunctionFlags::RequiredAPI;
        SDK::UFunction::FunctionFlags_Offset = Memory::FindMatchingValueOffset(Pairs);
    }

    if (SDK::UFunction::FunctionFlags_Offset) {
        LOG(LOG_INFO, "Found UFunction::FunctionFlags offset: 0x%X", SDK::UFunction::FunctionFlags_Offset);
        return true;
    }

    LOG(LOG_ERROR, "Failed to find UFunction::FunctionFlags offset!");
    return false;
}

bool SetupUnrealStructOffsets() {
    return Setup_UProperty_Offset_Internal() &&
        Setup_UClass_ClassCastFlags() &&
        Setup_UClass_ClassDefaultObject() &&
        Setup_UStruct_SuperStruct() &&
        Setup_UStruct_Children() &&
        Setup_UField_Next() &&
        Setup_UFunction_FunctionFlags();
}


// Unreal-Engine & Fortnite general offsets...

bool SetupProcessEvent() {
    auto Resolve32BitRelativeJump = [](void* FunctionPtr) -> uintptr_t {
            uint8_t* Address = reinterpret_cast<uint8_t*>(FunctionPtr);
            if (*Address == 0xE9) {
                uintptr_t Ret = (uintptr_t)((Address + 5) + *reinterpret_cast<int32_t*>(Address + 1));
                if (Memory::IsAddressInsideImage(Ret))
                    return Ret;
            }

            return reinterpret_cast<uintptr_t>(FunctionPtr);
        };

    void** VTable = SDK::UObject::Objects.GetByIndex(0)->VTable;
    for (int i = 0; i < 0x150; i++) {
        if (!VTable[i] || !Memory::IsAddressInsideImage(reinterpret_cast<uintptr_t>(VTable[i])))
            break;

        if (Memory::PatternScanRangeBytes<int32_t>(Resolve32BitRelativeJump(VTable[i]), 0x400, { 0xF7, -0x1, (int32_t)SDK::UFunction::FunctionFlags_Offset, 0x0, 0x0, 0x0, 0x0, 0x04, 0x0, 0x0 }, false, -1, false, false)
            && Memory::PatternScanRangeBytes<int32_t>(Resolve32BitRelativeJump(VTable[i]), 0x400, { 0xF7, -0x1, (int32_t)SDK::UFunction::FunctionFlags_Offset, 0x0, 0x0, 0x0, 0x0, 0x0, 0x40, 0x0 }, false, -1, false, false)) {
            SDK::UObject::ProcessEvent_Idx = i;
            LOG(LOG_INFO, "Found UObject::ProcessEvent VFT index: 0x%X", SDK::UObject::ProcessEvent_Idx);
            return true;
        }
    }

    LOG(LOG_ERROR, "Failed to find UObject::ProcessEvent VFT index!");
    return false;
}
bool SetupViewProjectionMatrix() {
    uintptr_t StringReference = Memory::FindStringRef(L"/Engine/EngineResources/GradientTexture0.");
    if (StringReference) {
        // Resolve the jmp if the function is split into multiple parts.
        uintptr_t Jmp = Memory::PatternScanRange<int32_t>(StringReference, 0x30, "E9", false, 1, true);
        if (Jmp)
            StringReference = Jmp;

        // The signature we are looking for is:
        /*
        0F 28 05 50 50 22 02      movaps  xmm0, cs:xmmword_474C280
        0F 11 83 C0 02 00 00      movups  xmmword ptr [rbx+2C0h], xmm0
        0F 28 0D 02 50 22 02      movaps  xmm1, cs:xmmword_474C240
        0F 11 8B 80 02 00 00      movups  xmmword ptr [rbx+280h], xmm1
        */

        // The second movups instruction is writing to the ViewProjectionMatrix, so we will get the offset of that instruction.
        uintptr_t Sig = Memory::PatternScanRange<int32_t>(StringReference, 0x500, "0F 28 ? ? ? ? ? 0F 11 ? ? ? ? ? 0F 28 ? ? ? ? ? 0F 11 ?");
        if (Sig) {
            SDK::UCanvas::ViewProjectionMatrix_Offset = *(uint32_t*)(Sig + 24);
            LOG(LOG_INFO, "Found UCanvas::ViewProjectionMatrix offset: 0x%X", SDK::UCanvas::ViewProjectionMatrix_Offset);
            return true;
        }
    }

    LOG(LOG_ERROR, "Failed to find UCanvas::ViewProjectionMatrix offset!");
    return false;
}
bool SetupLevelActors() {
    SDK::PropertyInfo Info = SDK::UObject::GetPropertyInfo("Level", "OwningWorld");
    SDK::ULevel* Level = SDK::GetWorld()->PersistentLevel();
    if (Info.Found && Level) {
        for (int i = 0x70; i < Info.Offset; i += 8)
        {
            // We will check if the address is a valid TArray<AActor*>
            // by checking if the Max() is greater than Num()
            // and if the pointer to the data is valid.
            SDK::TArray<SDK::AActor*>* Actors = reinterpret_cast<SDK::TArray<SDK::AActor*>*>((uintptr_t)Level + i);
            if (!Actors->IsValid())
                continue;
            if (Actors->Max() <= 0 || Actors->Num() <= 0)
                continue;
            if (Actors->Max() < Actors->Num())
                continue;

            SDK::ULevel::Actors_Offset = i;
            LOG(LOG_INFO, "Found ULevel::Actors offset: 0x%X", SDK::ULevel::Actors_Offset);
            return true;
        }
    }

    LOG(LOG_ERROR, "Failed to find ULevel::Actors offset!");
    return false;
}

bool SetupUnrealFortniteOffsets() {
    return SetupProcessEvent() && SetupViewProjectionMatrix() && SetupLevelActors();
}


bool SDK::Init() {
    return SetupUnrealGeneralOffsets() && SetupUnrealStructOffsets() && SetupUnrealFortniteOffsets();
}
