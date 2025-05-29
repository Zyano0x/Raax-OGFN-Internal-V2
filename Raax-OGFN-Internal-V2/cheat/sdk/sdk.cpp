#include "sdk.h"
#include <sstream>

#include <cheat/core.h>
#include <utils/error.h>
#include <utils/memory.h>
#include <utils/log.h>
#include <utils/math.h>

namespace SDK {

// --- Unreal-Engine general offsets ---------------------------------

bool SetupGObjects() {
    // Two standard patterns for finding chunked and non-chunked GObjects.
    static const std::vector<std::pair<const std::string, const std::string>> Patterns = {
        {"Chunked", "48 8B 05 ? ? ? ? 48 8B 0C C8 48 8D 04 D1"}, {"Fixed", "48 8B 05 ? ? ? ? 48 8D 14 C8 EB 02"}};

    for (const auto& [Type, Pattern] : Patterns) {
        uintptr_t Address = Memory::PatternScan<uint32_t>(Pattern.c_str(), 3, true);
        if (Memory::IsAddressInsideImage(Address)) {
            LOG(LOG_INFO, "Found GObjects (%s) offset: 0x%p", Type.c_str(),
                reinterpret_cast<void*>(Address - Memory::GetImageBase()));
            UObject::Objects.Initialize(reinterpret_cast<void*>(Address), Type == "Chunked");
            return true;
        }
    }

    Error::ThrowError("Failed to find GObjects!");
    return false;
}
bool SetupFMemoryRealloc() {
    // This signature has been very reliable from what I have tested.
    // Since Realloc can be used as Alloc and Free aswell, it is easier to only get Realloc.
    /*
    48 89 5C 24 08	mov     [rsp+arg_0], rbx
    48 89 74 24 10	mov     [rsp+arg_8], rsi
    57				push    rdi
    48 83 EC 20		sub     rsp, 20h
    48 8B F1		mov     rsi, rcx
    41 8B D8		mov     ebx, r8d
    48 8B 0D 64		mov     rcx, cs:qword_6024FE0
    5A 10 04
    48 8B FA		mov     rdi, rdx
    48 85 C9		test    rcx, rcx
    */
    uintptr_t FMemoryRealloc = Memory::PatternScan(
        "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8B F1 41 8B D8 48 8B 0D ? ? ? ? 48 8B FA 48 85 C9");
    if (FMemoryRealloc) {
        FMemory::FMemoryRealloc = reinterpret_cast<decltype(FMemory::FMemoryRealloc)>(FMemoryRealloc);
        LOG(LOG_INFO, "Found FMemoryRealloc offset: 0x%p",
            reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(FMemory::FMemoryRealloc) - Memory::GetImageBase()));
        return true;
    }

    Error::ThrowError("Failed to find FMemoryRealloc!");
    return false;
}
bool SetupFNameToString() {
    uintptr_t StringRef = Memory::FindStringRef(L"TSoftObjectPtr<%s%s>");
    if (StringRef) {
        uintptr_t FNameToString = Memory::PatternScanRange<int32_t>(StringRef, 0x50, "E8 ? ? ? ? 48", true, 1, true);
        if (FNameToString) {
            FName::FNameToString = reinterpret_cast<decltype(FName::FNameToString)>(FNameToString);
            LOG(LOG_INFO, "Found FNameToString offset: 0x%p",
                reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(FName::FNameToString) - Memory::GetImageBase()));
            return true;
        }
    }

    Error::ThrowError("Failed to find FNameToString!");
    return false;
}
bool SetupFNameConstructorW() {
    uintptr_t StringRef = Memory::FindStringRef(L"CanvasObject");
    if (!StringRef) {
        Error::ThrowError("Failed to find FNameConstructorW: \"CanvasObject\" string");
        return false;
    }

    uintptr_t FNameConstructorW = Memory::PatternScanRange<int32_t>(StringRef, 0x50, "E8", false, 1, true);
    if (!FNameConstructorW) {
        Error::ThrowError("Failed to find FNameConstructorW: Call FNameConstructorW instruction");
        return false;
    }

    FName::FNameConstructorW = reinterpret_cast<decltype(FName::FNameConstructorW)>(FNameConstructorW);
    LOG(LOG_INFO, "Found FNameConstructorW offset: 0x%p",
        reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(FName::FNameConstructorW) - Memory::GetImageBase()));
    return true;
}
bool SetupUKismetSystemLibraryLineTraceSingle() {
    // Two patterns for LineTraceSingle. One supports older UE4 versions, and the other supports later UE4 versions and
    // some UE5 versions.
    static const std::vector<std::pair<size_t, const std::string>> Patterns = {
        {151, "48 8B 43 20 48 8D 4D E0 0F 10 45 C0 44 0F B6 8D ? ? ? ? 48 85 C0 0F 10 4D D0 4C 8D 45 A0 40 0F 95 C7 66 "
              "0F 7F 45 ? 48 8D 55 B0 F2 0F 10 45 ? 48 03 F8 8B 45 88 F2 0F 11 45 ? F2 0F 10 45 ? 89 45 A8 8B 45 98 F2 "
              "0F 11 45 ? F3 0F 10 44 24 ? F3 0F 11 44 24 ? 48 89 4C 24 ? 48 8D 4D F0 48 89 4C 24 ? 48 8B 4C 24 ? 44 "
              "88 7C 24 ? 48 89 74 24 ? 89 45 B8 0F B6 85 ? ? ? ? 89 44 24 30 4C 89 74 24 ? 44 88 64 24 ? 48 89 7B 20 "
              "66 0F 7F 4D ? E8"},
        {88, "4C 39 6B 20 4C 8D 45 A0 F2 0F 10 45 ? 48 8D 55 B0 44 0F B6 8D ? ? ? ? 49 8B C5 48 8B 4C 24 ? 0F 95 C0 48 "
             "01 43 20 8B 45 88 44 88 74 24 ? F2 0F 11 45 ? F2 0F 10 45 ? 89 45 A8 8B 45 98 48 89 7C 24 ? 48 89 74 24 "
             "? 89 45 B8 F2 0F 11 45 ? 44 88 7C 24 ? E8"}};

    for (const auto& [Offset, Pattern] : Patterns) {
        uintptr_t Address = Memory::PatternScan<int32_t>(Pattern.c_str(), Offset, true);
        if (Memory::IsAddressInsideImage(Address)) {
            UKismetSystemLibrary::KismetSystemLibraryLineTraceSingle =
                reinterpret_cast<decltype(UKismetSystemLibrary::KismetSystemLibraryLineTraceSingle)>(Address);
            LOG(LOG_INFO, "Found UKismetSystemLibrary::LineTraceSingle offset: 0x%p",
                reinterpret_cast<void*>(Address - Memory::GetImageBase()));
            return true;
        }
    }

    Error::ThrowError("Failed to find UKismetSystemLibrary::LineTraceSingle!");
    return false;
}

bool SetupUnrealGeneralOffsets() {
    return SetupGObjects() && SetupFMemoryRealloc() && SetupFNameToString() && SetupFNameConstructorW() &&
           SetupUKismetSystemLibraryLineTraceSingle();
}

// --- Unreal-Engine struct/class offsets ----------------------------

bool Setup_UProperty_Offset_Internal() {
    /*temporary*/
    static bool bUnrealVersionHigherThan23 = false;
    UProperty::Offset_Internal_Offset = bUnrealVersionHigherThan23 ? 0x4C : 0x44;
    LOG(LOG_INFO, "Using hardcoded UProperty::Offset_Internal offset: 0x%X", UProperty::Offset_Internal_Offset);
    return true;
}
bool Setup_UBoolProperty_ByteMask() {
    UBoolProperty::ByteMask_Offset = 0x72;
    LOG(LOG_INFO, "Using hardcoded UBoolProperty::ByteMask offset: 0x%X", UBoolProperty::ByteMask_Offset);
    return true;
}
bool Setup_UClass_ClassCastFlags() {
    std::vector<std::pair<void*, EClassCastFlags>> Pairs = {
        {UObject::FindObjectFast("Actor"), EClassCastFlags::Actor},
        {UObject::FindObjectFast("Class"), EClassCastFlags::Field | EClassCastFlags::Struct | EClassCastFlags::Class}};

    UClass::ClassCastFlags_Offset = Memory::FindMatchingValueOffset(Pairs);
    if (UClass::ClassCastFlags_Offset) {
        LOG(LOG_INFO, "Found UClass::ClassCastFlags offset: 0x%X", UClass::ClassCastFlags_Offset);
        return true;
    }

    Error::ThrowError("Failed to find UClass::ClassCastFlags offset!");
    return false;
}
bool Setup_UClass_ClassDefaultObject() {
    std::vector<std::pair<void*, void*>> Pairs = {
        {UObject::FindObjectFast("Object"), UObject::FindObjectFast("Default__Object")},
        {UObject::FindObjectFast("Field"), UObject::FindObjectFast("Default__Field")}};

    UClass::ClassDefaultObject_Offset = Memory::FindMatchingValueOffset(Pairs);
    if (UClass::ClassDefaultObject_Offset) {
        LOG(LOG_INFO, "Found UClass::ClassDefaultObject offset: 0x%X", UClass::ClassDefaultObject_Offset);
        return true;
    }

    Error::ThrowError("Failed to find UClass::ClassDefaultObject offset!");
    return false;
}
bool Setup_UStruct_SuperStruct() {
    std::vector<std::pair<void*, void*>> Pairs = {
        {UObject::FindObjectFast("Struct"), UObject::FindObjectFast("Field")},
        {UObject::FindObjectFast("Class"), UObject::FindObjectFast("Struct")}};

    if (!Pairs[0].first)
        Pairs[0].first = Pairs[1].second = UObject::FindObjectFast("struct");

    UStruct::SuperStruct_Offset = Memory::FindMatchingValueOffset(Pairs);
    if (UStruct::SuperStruct_Offset) {
        LOG(LOG_INFO, "Found UStruct::SuperStruct offset: 0x%X", UStruct::SuperStruct_Offset);
        return true;
    }

    Error::ThrowError("Failed to find UStruct::SuperStruct offset!");
    return false;
}
bool Setup_UStruct_Children() {
    std::vector<std::pair<void*, void*>> Pairs = {
        {UObject::FindObjectFast("PlayerController"),
         UObject::FindObjectFastWithOuter("WasInputKeyJustReleased", "PlayerController")},
        {UObject::FindObjectFast("Controller"), UObject::FindObjectFastWithOuter("UnPossess", "Controller")}};

    UStruct::Children_Offset = Memory::FindMatchingValueOffset(Pairs);
    if (!UStruct::Children_Offset) {
        LOG(LOG_INFO, "Failed to find UStruct::Children with object pairs 1. Attempting again with object pairs 2.");

        Pairs = {{UObject::FindObjectFast("Vector"), UObject::FindObjectFastWithOuter("X", "Vector")},
                 {UObject::FindObjectFast("Vector4"), UObject::FindObjectFastWithOuter("X", "Vector4")},
                 {UObject::FindObjectFast("Vector2D"), UObject::FindObjectFastWithOuter("X", "Vector2D")},
                 {UObject::FindObjectFast("Guid"), UObject::FindObjectFastWithOuter("A", "Guid")}};

        UStruct::Children_Offset = Memory::FindMatchingValueOffset(Pairs);
    }

    if (UStruct::Children_Offset) {
        LOG(LOG_INFO, "Found UStruct::Children offset: 0x%X", UStruct::Children_Offset);
        return true;
    }

    Error::ThrowError("Failed to find UStruct::Children offset!");
    return false;
}
bool Setup_UStruct_ChildProperties() {
    UStruct::ChildProperties_Offset = 0x50; // Potentially unsafe to be hardcoded.
    LOG(LOG_INFO, "Using potentially unsafe hardcoded UStruct::ChildProperties offset: 0x%X",
        UStruct::ChildProperties_Offset);
    return true;
}
bool Setup_UField_Next() {
    std::vector<std::pair<void*, void*>> Pairs = {
        {UObject::FindObjectFast<UClass>("KismetArrayLibrary", EClassCastFlags::Class)->Children,
         UObject::FindObjectFast<UClass>("FilterArray", EClassCastFlags::Function)}};

    UField::Next_Offset = Memory::FindMatchingValueOffset(Pairs, 0x10, 0x40);
    if (UField::Next_Offset) {
        LOG(LOG_INFO, "Found UField::Next offset: 0x%X", UField::Next_Offset);
        return true;
    }

    Error::ThrowError("Failed to find UField::Next offset!");
    return false;
}
bool Setup_UFunction_FunctionFlags() {
    std::vector<std::pair<void*, EFunctionFlags>> Pairs = {
        {UObject::FindObjectFast("WasInputKeyJustPressed"),
         EFunctionFlags::Final | EFunctionFlags::Native | EFunctionFlags::Public | EFunctionFlags::BlueprintCallable |
             EFunctionFlags::BlueprintPure | EFunctionFlags::Const},
        {UObject::FindObjectFast("ToggleSpeaking"),
         EFunctionFlags::Exec | EFunctionFlags::Native | EFunctionFlags::Public},
        {UObject::FindObjectFast("SwitchLevel"),
         EFunctionFlags::Exec | EFunctionFlags::Native | EFunctionFlags::Public}};

    UFunction::FunctionFlags_Offset = Memory::FindMatchingValueOffset(Pairs);
    if (!UFunction::FunctionFlags_Offset) {
        for (auto& [_, Flags] : Pairs)
            Flags = Flags | EFunctionFlags::RequiredAPI;
        UFunction::FunctionFlags_Offset = Memory::FindMatchingValueOffset(Pairs);
    }

    if (UFunction::FunctionFlags_Offset) {
        LOG(LOG_INFO, "Found UFunction::FunctionFlags offset: 0x%X", UFunction::FunctionFlags_Offset);
        return true;
    }

    Error::ThrowError("Failed to find UFunction::FunctionFlags offset!");
    return false;
}
bool Setup_UFunction_FuncPtr() {
    PIMAGE_SECTION_HEADER TextSection = Memory::GetImageTextSection();
    uintptr_t             TextStart = TextSection->VirtualAddress + Memory::GetImageBase();
    uintptr_t             TextEnd = TextStart + TextSection->Misc.VirtualSize;

    UFunction* Func = UObject::FindObjectFast<UFunction>("WasInputKeyJustPressed");

    for (uint32_t i = UFunction::FunctionFlags_Offset; i < UFunction::FunctionFlags_Offset + 0x30; i++) {
        uintptr_t As64BitInt = *(uintptr_t*)((uintptr_t)Func + i);
        if (As64BitInt > TextStart && As64BitInt < TextEnd) {
            UFunction::FuncPtr_Offset = i;
            LOG(LOG_INFO, "Found UFunction::FuncPtr offset: 0x%X", UFunction::FuncPtr_Offset);
            return true;
        }
    }

    Error::ThrowError("Failed to find UFunction::FuncPtr offset!");
    return false;
}

bool SetupUnrealStructOffsets() {
    return Setup_UProperty_Offset_Internal() && Setup_UBoolProperty_ByteMask() && Setup_UClass_ClassCastFlags() &&
           Setup_UClass_ClassDefaultObject() && Setup_UStruct_SuperStruct() && Setup_UStruct_Children() &&
           Setup_UStruct_ChildProperties() && Setup_UField_Next() && Setup_UFunction_FunctionFlags() &&
           Setup_UFunction_FuncPtr();
}

// --- Unreal-Engine & Fortnite general offsets ----------------------

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

    void** VTable = UObject::Objects.GetByIndex(0)->VTable;
    for (int i = 0; i < 0x150; i++) {
        if (!VTable[i] || !Memory::IsAddressInsideImage(reinterpret_cast<uintptr_t>(VTable[i])))
            break;

        if (Memory::PatternScanRangeBytes<int32_t>(
                Resolve32BitRelativeJump(VTable[i]), 0x400,
                {0xF7, -0x1, (int32_t)UFunction::FunctionFlags_Offset, 0x0, 0x0, 0x0, 0x0, 0x04, 0x0, 0x0}, false, -1,
                false, false) &&
            Memory::PatternScanRangeBytes<int32_t>(
                Resolve32BitRelativeJump(VTable[i]), 0x400,
                {0xF7, -0x1, (int32_t)UFunction::FunctionFlags_Offset, 0x0, 0x0, 0x0, 0x0, 0x0, 0x40, 0x0}, false, -1,
                false, false)) {
            UObject::ProcessEvent_Idx = i;
            LOG(LOG_INFO, "Found UObject::ProcessEvent VFT index: 0x%X", UObject::ProcessEvent_Idx);
            return true;
        }
    }

    Error::ThrowError("Failed to find UObject::ProcessEvent VFT index!");
    return false;
}
bool SetupEngineVersion() {
    FString EngineVersionStr = UKismetSystemLibrary::GetEngineVersion();

    std::istringstream stream(EngineVersionStr.ToString());
    std::string        line;
    while (std::getline(stream, line)) {
        size_t      DashPos = line.find('-');
        std::string EngineStr = line.substr(0, DashPos);

        size_t      ReleasePos = line.find("Release-");
        std::string GameStr = line.substr(ReleasePos + 8);

        size_t      PlusPos = line.find("+++");
        std::string CLStr = line.substr(DashPos + 1, PlusPos - DashPos - 1);

        g_EngineVersion = std::stof(EngineStr.substr(0, EngineStr.find_last_of('.')));
        g_GameVersion = std::stof(GameStr);
        g_CL = std::stoi(CLStr);
    }

    LOG(LOG_INFO, "EngineVersion: %f, GameVersions: %f, CL: %d (%s)", g_EngineVersion, g_GameVersion, g_CL,
        EngineVersionStr.ToString().c_str());
    return true;
}
bool SetupDrawTransition() {
    void**       VTable = UObject::FindObjectFast("Default__GameViewportClient")->VTable;
    PropertyInfo GameInstanceProp = UObject::GetPropertyInfo("GameViewportClient", "GameInstance");
    int          bSuppressTransitionMessage = GameInstanceProp.Offset + sizeof(void*);

    for (int i = 0x30; i < 0x80 - 1; i++) {
        if (!VTable[i] || !Memory::IsAddressInsideImage(reinterpret_cast<uintptr_t>(VTable[i])))
            break;

        uintptr_t Address = Memory::PatternScanRangeBytes<int32_t>(
            (uintptr_t)VTable[i], 0x35, {0x80, 0xB9, bSuppressTransitionMessage, 0x00, 0x00, 0x00, 0x00}, false, -1,
            false, false);
        if (Address) {
            UGameViewportClient::DrawTransition_Idx = i;
            LOG(LOG_INFO, "Found UGameViewportClient::DrawTransition VFT index: 0x%X",
                UGameViewportClient::DrawTransition_Idx);
            return true;
        }
    }

    Error::ThrowError("Failed to find UCanvas::DrawTransition VFT index!");
    return false;
}
bool SetupViewProjectionMatrix() {
    uintptr_t SearchStart = Memory::FindStringRef(L"/Engine/EngineResources/GradientTexture0.");
    if (SearchStart) {
        // Resolve the jmp if the function is split into multiple parts.
        uintptr_t Jmp = Memory::PatternScanRange<int32_t>(SearchStart, 0x30, "E9", false, 1, true);
        if (Jmp)
            SearchStart = Jmp;
    } else {
        SearchStart = Memory::PatternScan("89 ? 38 48 8B 05 ? ? ? ? 48 89 ? 60 48 8B 05");
    }

    if (SearchStart) {
        // The signature we are looking for is:
        /*
        0F 28 05 50 50 22 02      movaps  xmm0, cs:xmmword_474C280
        0F 11 83 C0 02 00 00      movups  xmmword ptr [rbx+2C0h], xmm0
        0F 28 0D 02 50 22 02      movaps  xmm1, cs:xmmword_474C240
        0F 11 8B 80 02 00 00      movups  xmmword ptr [rbx+280h], xmm1
        */

        // The second movups instruction is writing to the ViewProjectionMatrix, so we will get the offset of that
        // instruction.
        uintptr_t Sig = Memory::PatternScanRange<int32_t>(
            SearchStart, 0x150, "0F 28 ? ? ? ? ? 0F 11 ? ? ? ? ? 0F 28 ? ? ? ? ? 0F 11 ? ? ? ? ? 48");
        if (!Sig)
            Sig = Memory::PatternScanRange<int32_t>(
                SearchStart, 0x150, "0F 28 ? ? ? ? ? 0F 11 ? ? ? ? ? 0F 28 ? ? ? ? ? 0F 11 ? ? ? ? ? 4C");

        if (Sig) {
            UCanvas::ViewProjectionMatrix_Offset = *(uint32_t*)(Sig - 18);
            LOG(LOG_INFO, "Found UCanvas::ViewProjectionMatrix offset: 0x%X", UCanvas::ViewProjectionMatrix_Offset);
            return true;
        }
    }

    Error::ThrowError("Failed to find UCanvas::ViewProjectionMatrix offset!");
    return false;
}
bool SetupCanvas() {
    UCanvas::Canvas_Offset = UCanvas::ViewProjectionMatrix_Offset - (sizeof(void*) * 2);
    LOG(LOG_INFO, "Using hardcoded UCanvas::Canvas offset: 0x%X", UCanvas::Canvas_Offset);
    return true;
}
bool SetupFCanvasGetBatchedElements() {
    uintptr_t StringRef = Memory::FindStringRef(L"STAT_Canvas_GetBatchElementsTime");
    if (!StringRef) {
        Error::ThrowError("Failed to find \"STAT_Canvas_GetBatchElementsTime\" for FCanvas::GetBatchedElements!");
        return false;
    }

    uintptr_t Address = Memory::PatternScanRange<int32_t>(StringRef, 0x100, "48 8B C4", true);
    if (Memory::IsAddressInsideImage(Address)) {
        FCanvas::FCanvasGetBatchedElements = reinterpret_cast<decltype(FCanvas::FCanvasGetBatchedElements)>(Address);
        LOG(LOG_INFO, "Found FCanvas::GetBatchedElements offset: 0x%p",
            reinterpret_cast<void*>(Address - Memory::GetImageBase()));
        return true;
    }

    Error::ThrowError("Failed to find FCanvas::GetBatchedElements!");
    return false;
}
bool SetupBatchedThickLines() {
    FBatchedElements::BatchedThickLines_Offset = 0x40;
    LOG(LOG_INFO, "Using hardcoded FBatchedElements::BatchedThickLines offset: 0x%X",
        FBatchedElements::BatchedThickLines_Offset);
    return true;
}
bool SetupLevelActors() {
    UWorld* World = Core::GetNewWorld();
    if (!World) {
        Error::ThrowError("Failed to get World to find ULevel::Actors offset!");
        return false;
    }

    ULevel* Level = World->PersistentLevel;
    if (!Level) {
        Error::ThrowError(
            "Failed to get PersistentLevel to find ULevel::Actors offset! Injected DLL before game loaded?");
        return false;
    }

    PropertyInfo Info = UObject::GetPropertyInfo("Level", "OwningWorld");
    if (Info.Found) {
        for (int i = 0x70; i < Info.Offset; i += 8) {
            // We will check if the address is a valid TArray<AActor*> by checking if the Max() is greater than Num()
            // and if the pointer to the data is valid.
            TArray<AActor*>* Actors = reinterpret_cast<TArray<AActor*>*>((uintptr_t)Level + i);
            if (!Actors->IsValid())
                continue;
            if (Actors->Max() <= 0 || Actors->Num() <= 0)
                continue;
            if (Actors->Max() < Actors->Num())
                continue;

            ULevel::Actors_Offset = i;
            LOG(LOG_INFO, "Found ULevel::Actors offset: 0x%X", ULevel::Actors_Offset);
            return true;
        }
    }

    Error::ThrowError("Failed to find ULevel::Actors offset!");
    return false;
}
bool SetupComponentSpaceTransformsArray() {
    PropertyInfo Info = UObject::GetPropertyInfo("SkinnedMeshComponent", "VertexOffsetUsage", true);
    int32_t      Offset = Info.Offset + 0x10;
    if (!Info.Found) {
        Info = UObject::GetPropertyInfo("SkinnedMeshComponent", "MasterPoseComponent");
        Offset = Info.Offset + 0x8;
    }

    if (Info.Found) {
        USkinnedMeshComponent::ComponentSpaceTransformsArray_Offset = Offset;
        LOG(LOG_INFO, "Found USkinnedMeshComponent::ComponentSpaceTransformsArray offset: 0x%X",
            USkinnedMeshComponent::ComponentSpaceTransformsArray_Offset);
        return true;
    }

    Error::ThrowError("Failed to find USkinnedMeshComponent::ComponentSpaceTransformsArray offset!");
    return false;
}
bool SetupEditModeInputComponent0Offset(uint64_t& Output) {
    Output = Memory::FindStringRef(L"EditModeInputComponent0");
    if (Output) {
        LOG(LOG_INFO, "Found EditModeInputComponent0 offset: %p",
            reinterpret_cast<void*>(Output - Memory::GetImageBase()));
        return true;
    }

    Error::ThrowError("Failed to find EditModeInputComponent0 offset!");
    return false;
}
bool SetupFireOffset(uint64_t EditModeInputComponent0) {
    uintptr_t FireString = Memory::FindStringRefRange("Fire", (uint8_t*)(EditModeInputComponent0 - 0x4500), 0x4500);
    if (!FireString) {
        Error::ThrowError("Failed to find Fire_Press \"Fire\" string");
        return false;
    }

    LOG(LOG_INFO, "Found Fire_Press \"Fire\" string ref: %p",
        reinterpret_cast<void*>(FireString - Memory::GetImageBase()));

    // Search for all lea r64,m patterns, and return the earliest one
    auto FindLeaInstruction = [](uintptr_t SearchStart, uintptr_t SearchSize) {
        constexpr const char* Patterns[] = {
            "48 8D", // Legacy registers
            "4C 8D", // Numbered registers
        };

        uintptr_t EarliestResult = UINT64_MAX;
        for (const char* Pattern : Patterns) {
            uintptr_t Result = Memory::PatternScanRange<int32_t>(SearchStart, SearchSize, Pattern, false, 3, true);

            // We would normally virtual query the result to make sure it's a function (executable) and not
            // something like a string, this isn't possible since fortnite uses rwx for all sections.
            if (Memory::IsAddressInsideImage(Result) && Result < EarliestResult)
                EarliestResult = Result;
        }

        return EarliestResult == UINT64_MAX ? 0 : EarliestResult;
    };

    uintptr_t FirePressAddress = FindLeaInstruction(FireString - 0x28, 0x50);
    if (!FirePressAddress) {
        Error::ThrowError("Failed to find Fire_Press");
        return false;
    }

    // Find the next "Fire" string, used for Fire_Release
    FireString = Memory::FindStringRefRange("Fire", reinterpret_cast<uint8_t*>(FireString + 1), 0x100);
    if (!FireString) {
        Error::ThrowError("Failed to find Fire_Release \"Fire\" string");
        return false;
    }

    LOG(LOG_INFO, "Found Fire_Release \"Fire\" string ref: %p",
        reinterpret_cast<void*>(FireString - Memory::GetImageBase()));

    uintptr_t FireReleaseAddress = FindLeaInstruction(FireString - 0x28, 0x50);
    if (!FireReleaseAddress) {
        Error::ThrowError("Failed to find Fire_Release");
        return false;
    }

    AFortPlayerController::pFire_Press =
        reinterpret_cast<decltype(AFortPlayerController::pFire_Press)>(FirePressAddress);
    LOG(LOG_INFO, "Found AFortPlayerController::Fire_Press offset: %p",
        reinterpret_cast<void*>(FirePressAddress - Memory::GetImageBase()));

    AFortPlayerController::pFire_Release =
        reinterpret_cast<decltype(AFortPlayerController::pFire_Release)>(FireReleaseAddress);
    LOG(LOG_INFO, "Found AFortPlayerController::Fire_Release offset: %p",
        reinterpret_cast<void*>(FireReleaseAddress - Memory::GetImageBase()));

    return true;
}
void FindGetWeaponStats() {
    void** VTable = UObject::FindObjectFast("Default__FortWeapon")->VTable;

    for (int i = 0x30; i < 0x100; i++) {
        if (!VTable[i] || !Memory::IsAddressInsideImage(reinterpret_cast<uintptr_t>(VTable[i])))
            break;

        if (Memory::PatternScanRangeBytes<int32_t>(
                (uintptr_t)VTable[i], 0x35,
                {0x48, 0x83, 0xEC, 0x58, 0x48, 0x8B, -0x01, -0x01, -0x01, -0x01, -0x01, 0x48, 0x85}) ||
            Memory::PatternScanRangeBytes<int32_t>(
                (uintptr_t)VTable[i], 0x35,
                {0x48, 0x8B, 0x89, -0x01, -0x01, 0x00, 0x00, 0x48, -0x01, -0x01, 0x74, 0x13}) ||
            Memory::PatternScanRangeBytes<int32_t>(
                (uintptr_t)VTable[i], 0x35,
                {0x48, 0x8B, 0x60, -0x01, -0x01, 0x00, 0x00, 0x48, -0x01, -0x01, 0x8B, 0xFA})) {
            AFortWeapon::GetWeaponStats_Idx = i;
            LOG(LOG_INFO, "Found AFortWeapon::GetWeaponStats VFT index: 0x%X", UGameViewportClient::DrawTransition_Idx);
            return;
        }
    }

    LOG(LOG_WARN,
        "Failed to find AFortWeapon::GetWeaponStats VFT index! WeaponStats based exploits will no longer work.");
}
void FindComponentToWorldOffset() {
    void* execK2_GetComponentToWorld = UObject::GetFunction("SceneComponent", "K2_GetComponentToWorld")->FuncPtr;
    if (execK2_GetComponentToWorld) {
        uintptr_t K2_GetComponentToWorld =
            Memory::PatternScanRange<int32_t>((uintptr_t)execK2_GetComponentToWorld, 0x50, "E8", false, 1, true);
        if (!K2_GetComponentToWorld)
            K2_GetComponentToWorld = reinterpret_cast<uintptr_t>(execK2_GetComponentToWorld);

        uint8_t* Data = (uint8_t*)Memory::PatternScanRange<int32_t>(K2_GetComponentToWorld, 0x30, "0F 10");
        if (Data && Data[0] == 0x0F && Data[1] == 0x10 && Data[7] == 0x0F && Data[8] == 0x10 && Data[14] == 0x0F &&
            Data[15] == 0x10) {
            USceneComponent::ComponentToWorld_Offset = *(uint32_t*)(Data + 3);
            LOG(LOG_INFO, "Found USceneComponent::ComponentToWorld offset: 0x%X",
                USceneComponent::ComponentToWorld_Offset);
            return;
        }
    }

    LOG(LOG_WARN, "Failed to find USceneComponent::ComponentToWorld offset! Will fallback to calling UFunction.");
}

bool SetupUnrealFortniteOffsets() {
    bool Result = SetupProcessEvent() && SetupEngineVersion() && SetupDrawTransition() && SetupViewProjectionMatrix() &&
                  SetupCanvas() && SetupFCanvasGetBatchedElements() && SetupBatchedThickLines() && SetupLevelActors() &&
                  SetupComponentSpaceTransformsArray();
    if (Result) {
        FindGetWeaponStats();
        FindComponentToWorldOffset();
    }

    uint64_t EditModeInputComponent0 = 0;
    if (SetupEditModeInputComponent0Offset(EditModeInputComponent0)) {
        Result = SetupFireOffset(EditModeInputComponent0);
    }

    return Result;
}

// --- Initialization ------------------------------------------------

bool Init() {
    LOG(LOG_TRACE, "Setting up SDK...");
    return SetupUnrealGeneralOffsets() && SetupUnrealStructOffsets() && SetupUnrealFortniteOffsets();
}

// --- Global Variables ----------------------------------------------

float g_EngineVersion = 0.f;
float g_GameVersion = 0.f;
int   g_CL = 0;

} // namespace SDK
