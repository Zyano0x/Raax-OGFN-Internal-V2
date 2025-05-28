#include "Engine.h"

#include <cheat/sdk/sdk.h>
#include <cheat/core.h>

namespace SDK {

// --- Classes & Structs ---------------------------------------------

FString UKismetSystemLibrary::GetEngineVersion() {
    static UFunction* Func = GetFunction("KismetSystemLibrary", "GetEngineVersion");
    struct {
        FString ReturnValue;
    } params{};

    if (Func)
        StaticClass()->ProcessEvent(Func, &params);

    return params.ReturnValue;
}
bool UKismetSystemLibrary::LineTraceSingle(UObject* WorldContextObject, const FVector& Start, const FVector& End,
                                           ETraceTypeQuery TraceChannel, bool bTraceComplex,
                                           TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace DrawDebugType,
                                           FHitResult& OutHit, bool bIgnoreSelf, const FLinearColor& TraceColor,
                                           const FLinearColor& TraceHitColor, float DrawTime) {
    return KismetSystemLibraryLineTraceSingle(WorldContextObject, Start, End, TraceChannel, bTraceComplex,
                                              ActorsToIgnore, EDrawDebugTrace::None, OutHit, bIgnoreSelf,
                                              SDK::FLinearColor::White, SDK::FLinearColor::White, 0.f);
}

void UCanvas::K2_DrawLine(const FVector2D& ScreenPositionA, const FVector2D& ScreenPositionB, float Thickness,
                          const FLinearColor& RenderColor) {
    static UFunction* Func = GetFunction("Canvas", "K2_DrawLine");
    struct {
        FVector2D    ScreenPositionA;
        FVector2D    ScreenPositionB;
        float        Thickness;
        FLinearColor RenderColor;
    } params_K2_DrawLine{};

    params_K2_DrawLine.ScreenPositionA = ScreenPositionA;
    params_K2_DrawLine.ScreenPositionB = ScreenPositionB;
    params_K2_DrawLine.Thickness = Thickness;
    params_K2_DrawLine.RenderColor = RenderColor;

    if (Func)
        ProcessEvent(Func, &params_K2_DrawLine);
}
void UCanvas::K2_DrawText(UFont* RenderFont, const FString& RenderText, const FVector2D& ScreenPosition,
                          const FVector2D& Scale, const FLinearColor& RenderColor, float Kerning,
                          const FLinearColor& ShadowColor, const FVector2D& ShadowOffset, bool bCentreX, bool bCentreY,
                          bool bOutlined, const FLinearColor& OutlineColor) {
    static UFunction* Func = GetFunction("Canvas", "K2_DrawText");
    if (g_EngineVersion >= 4.22f) {
        struct {
            UFont*       RenderFont;
            FString      RenderText;
            FVector2D    ScreenPosition;
            FVector2D    Scale;
            FLinearColor RenderColor;
            float        Kerning;
            FLinearColor ShadowColor;
            FVector2D    ShadowOffset;
            bool         bCentreX;
            bool         bCentreY;
            bool         bOutlined;
            FLinearColor OutlineColor;
        } params_K2_DrawText_1{};

        params_K2_DrawText_1.RenderFont = RenderFont;
        params_K2_DrawText_1.RenderText = RenderText;
        params_K2_DrawText_1.ScreenPosition = {ScreenPosition.X, ScreenPosition.Y};
        params_K2_DrawText_1.Scale = Scale;
        params_K2_DrawText_1.RenderColor = RenderColor;
        params_K2_DrawText_1.Kerning = Kerning;
        params_K2_DrawText_1.ShadowColor = ShadowColor;
        params_K2_DrawText_1.ShadowOffset = ShadowOffset;
        params_K2_DrawText_1.bCentreX = bCentreX;
        params_K2_DrawText_1.bCentreY = bCentreY;
        params_K2_DrawText_1.bOutlined = bOutlined;
        params_K2_DrawText_1.OutlineColor = OutlineColor;

        if (Func)
            ProcessEvent(Func, &params_K2_DrawText_1);
    } else {
        struct {
            UFont*       RenderFont;
            FString      RenderText;
            FVector2D    ScreenPosition;
            FLinearColor RenderColor;
            float        Kerning;
            FLinearColor ShadowColor;
            FVector2D    ShadowOffset;
            bool         bCentreX;
            bool         bCentreY;
            bool         bOutlined;
            FLinearColor OutlineColor;
        } params_K2_DrawText_2{};

        params_K2_DrawText_2.RenderFont = RenderFont;
        params_K2_DrawText_2.RenderText = RenderText;
        params_K2_DrawText_2.ScreenPosition = {ScreenPosition.X, ScreenPosition.Y};
        params_K2_DrawText_2.RenderColor = RenderColor;
        params_K2_DrawText_2.Kerning = Kerning;
        params_K2_DrawText_2.ShadowColor = ShadowColor;
        params_K2_DrawText_2.ShadowOffset = ShadowOffset;
        params_K2_DrawText_2.bCentreX = bCentreX;
        params_K2_DrawText_2.bCentreY = bCentreY;
        params_K2_DrawText_2.bOutlined = bOutlined;
        params_K2_DrawText_2.OutlineColor = OutlineColor;

        if (Func)
            ProcessEvent(Func, &params_K2_DrawText_2);
    }
}

FTransform& USceneComponent::getprop_ComponentToWorld() {
    if (ComponentToWorld_Offset) {
        return *(FTransform*)((uintptr_t)this + ComponentToWorld_Offset);
    } else {
        static UFunction* Func = GetFunction("SceneComponent", "K2_GetComponentToWorld");
        struct {
            FTransform ReturnValue;
        } params_ComponentToWorld{};

        if (Func)
            ProcessEvent(Func, &params_ComponentToWorld);
        else
            LOG(LOG_WARN, "Failed to find USceneComponent::ComponentToWorld!");

        return params_ComponentToWorld.ReturnValue;
    }
}

TArray<FTransform>& USkinnedMeshComponent::getprop_ComponentSpaceTransformsArray() {
    TArray<FTransform>& FirstArray = *(TArray<FTransform>*)((uintptr_t)this + ComponentSpaceTransformsArray_Offset);
    if (FirstArray.IsValid())
        return FirstArray;

    TArray<FTransform>& SecondArray =
        *(TArray<FTransform>*)((uintptr_t)this + ComponentSpaceTransformsArray_Offset + sizeof(TArray<FTransform>));
    if (SecondArray.IsValid())
        return SecondArray;
}
int32_t USkinnedMeshComponent::GetBoneIndex(FName BoneName) {
    static UFunction* Func = GetFunction("SkinnedMeshComponent", "GetBoneIndex");
    struct {
        FName   BoneName;
        int32_t ReturnValue;
    } params_GetBoneIndex{};

    params_GetBoneIndex.BoneName = BoneName;

    if (Func)
        ProcessEvent(Func, &params_GetBoneIndex);

    return params_GetBoneIndex.ReturnValue;
}

FTransform USkeletalMeshComponent::GetBoneMatrix(int32_t BoneIndex) {
    TArray<FTransform>& Array = ComponentSpaceTransformsArray;
    if (Array.IsValid())
        return Array.GetByIndex(BoneIndex);
}
FVector USkeletalMeshComponent::GetBoneLocation(int32_t BoneIndex) {
    FTransform BoneMatrix = GetBoneMatrix(BoneIndex);
    FTransform ComponentToWrld = ComponentToWorld;

    FMatrix Matrix = BoneMatrix.ToMatrixWithScale() * ComponentToWrld.ToMatrixWithScale();
    return FVector(Matrix.M[3][0], Matrix.M[3][1], Matrix.M[3][2]);
}

float AActor::WasRecentlyRendered(float Tolerence) {
    static UFunction* Func = GetFunction("Actor", "WasRecentlyRendered");
    struct {
        float Tolerence;
        float ReturnValue;
    } params_WasRecentlyRendered{};

    params_WasRecentlyRendered.Tolerence = Tolerence;

    if (Func)
        ProcessEvent(Func, &params_WasRecentlyRendered);

    return params_WasRecentlyRendered.ReturnValue;
}

FVector APlayerCameraManager::GetCameraLocation() {
    static UFunction* Func = GetFunction("PlayerCameraManager", "GetCameraLocation");
    struct {
        FVector ReturnValue;
    } params_GetCameraLocation{};

    if (Func)
        ProcessEvent(Func, &params_GetCameraLocation);

    return params_GetCameraLocation.ReturnValue;
}
FRotator APlayerCameraManager::GetCameraRotation() {
    static UFunction* Func = GetFunction("PlayerCameraManager", "GetCameraRotation");
    struct {
        FRotator ReturnValue;
    } params_GetCameraRotation{};

    if (Func)
        ProcessEvent(Func, &params_GetCameraRotation);

    return params_GetCameraRotation.ReturnValue;
}
float APlayerCameraManager::GetFOVAngle() {
    static UFunction* Func = GetFunction("PlayerCameraManager", "GetFOVAngle");
    struct {
        float ReturnValue;
    } params_GetFOVAngle{};

    if (Func)
        ProcessEvent(Func, &params_GetFOVAngle);

    return params_GetFOVAngle.ReturnValue;
}

void APlayerController::AddYawInput(float Val) {
    static UFunction* Func = GetFunction("PlayerController", "AddYawInput");
    struct {
        float Val;
    } params_AddYawInput{};

    params_AddYawInput.Val = Val;

    if (Func)
        ProcessEvent(Func, &params_AddYawInput);
}
void APlayerController::AddPitchInput(float Val) {
    static UFunction* Func = GetFunction("PlayerController", "AddPitchInput");
    struct {
        float Val;
    } params_AddPitchInput{};

    params_AddPitchInput.Val = Val;

    if (Func)
        ProcessEvent(Func, &params_AddPitchInput);
}
bool APlayerController::WasInputKeyJustReleased(FKey& Key) {
    static UFunction* Func = GetFunction("PlayerController", "WasInputKeyJustReleased");
    struct {
        FKey    Key;
        bool    ReturnValue;
        uint8_t Pad[7];
    } params_WasInputKeyJustReleased{};

    params_WasInputKeyJustReleased.Key = Key;

    if (Func)
        ProcessEvent(Func, &params_WasInputKeyJustReleased);

    return params_WasInputKeyJustReleased.ReturnValue;
}
bool APlayerController::WasInputKeyJustPressed(FKey& Key) {
    static UFunction* Func = GetFunction("PlayerController", "WasInputKeyJustPressed");
    struct {
        FKey    Key;
        bool    ReturnValue;
        uint8_t Pad[7];
    } params_WasInputKeyJustPressed{};

    params_WasInputKeyJustPressed.Key = Key;

    if (Func)
        ProcessEvent(Func, &params_WasInputKeyJustPressed);

    return params_WasInputKeyJustPressed.ReturnValue;
}
bool APlayerController::IsInputKeyDown(FKey& Key) {
    static UFunction* Func = GetFunction("PlayerController", "IsInputKeyDown");
    struct {
        FKey    Key;
        bool    ReturnValue;
        uint8_t Pad[7];
    } params_IsInputKeyDown{};

    params_IsInputKeyDown.Key = Key;

    if (Func)
        ProcessEvent(Func, &params_IsInputKeyDown);

    return params_IsInputKeyDown.ReturnValue;
}
bool APlayerController::GetMousePosition(float& LocationX, float& LocationY) {
    static UFunction* Func = GetFunction("PlayerController", "GetMousePosition");
    struct {
        float   LocationX;
        float   LocationY;
        bool    ReturnValue;
        uint8_t Pad[3];
    } params_GetMousePosition{};

    if (Func)
        ProcessEvent(Func, &params_GetMousePosition);

    LocationX = params_GetMousePosition.LocationX;
    LocationY = params_GetMousePosition.LocationY;

    return params_GetMousePosition.ReturnValue;
}
void APlayerController::ServerChangeName(const FString& S) {
    static UFunction* Func = GetFunction("PlayerController", "ServerChangeName");
    struct {
        FString S;
    } params_ServerChangeName{};

    if (Func)
        ProcessEvent(Func, &params_ServerChangeName);
}

FString APlayerState::GetPlayerName() {
    static UFunction* Func = GetFunction("PlayerState", "GetPlayerName");
    struct {
        FString return_value;
    } params_GetPlayerName{};

    if (Func)
        ProcessEvent(Func, &params_GetPlayerName);

    return params_GetPlayerName.return_value;
}

// --- Public Functions ----------------------------------------------

ULocalPlayer* GetLocalPlayer() {
    return Core::g_LocalPlayer;
}
APlayerController* GetLocalController() {
    return Core::g_LocalPlayerController;
}
APawn* GetLocalPawn() {
    return Core::g_LocalPawn;
}

FVector GetCameraLocation() {
    return Core::g_CameraLocation;
}
FRotator GetCameraRotation() {
    return Core::g_CameraRotation;
}
float GetFOV() {
    return Core::g_FOV;
}

UEngine* GetEngine() {
    static UEngine* GEngine = nullptr;
    if (!GEngine) {
        for (int i = 0; i < UObject::Objects.Num(); i++) {
            UObject* Obj = UObject::Objects.GetByIndex(i);
            if (!Obj)
                continue;

            if (Obj->IsA(UEngine::StaticClass()) && !Obj->IsDefaultObject()) {
                GEngine = static_cast<UEngine*>(Obj);
                break;
            }
        }
    }

    return GEngine;
}
UWorld* GetWorld() {
    return Core::g_World;
}
UCanvas* GetCanvas() {
    static UCanvas* GCanvas = nullptr;
    if (!GCanvas || !GCanvas->Class || !GCanvas->IsA(UCanvas::StaticClass())) {
        for (int i = 0; i < UObject::Objects.Num(); i++) {
            UObject* Obj = UObject::Objects.GetByIndex(i);
            if (!Obj)
                continue;

            if (Obj->IsA(UCanvas::StaticClass()) && !Obj->IsDefaultObject() && Obj->Name == "DebugCanvasObject") {
                GCanvas = static_cast<UCanvas*>(Obj);
                break;
            }
        }
    }

    return GCanvas;
}

bool IsPositionVisible(const FVector& Position, AActor* IgnoredActor, AActor* IgnoredActor2) {
    static TArray<AActor*> IgnoredActors(2);
    IgnoredActors.Clear();
    if (IgnoredActor) {
        IgnoredActors.Add(IgnoredActor);
    }
    if (IgnoredActor2) {
        IgnoredActors.Add(IgnoredActor2);
    }

    FHitResult Hit = {};
    bool       bHitSomething = SDK::UKismetSystemLibrary::LineTraceSingle(
        SDK::GetWorld(), Core::g_CameraLocation, Position, ETraceTypeQuery::TraceTypeQuery1, true, IgnoredActors,
        EDrawDebugTrace::None, Hit, true, FLinearColor(0.f, 0.f, 0.f, 0.f), FLinearColor(0.f, 0.f, 0.f, 0.f), 0.f);

    return !bHitSomething;
}

FVector Project3D(const FVector& Location) {
    SDK::UCanvas* Canvas = GetCanvas();
    if (!Canvas)
        return {};

    FMatrix& ViewProjectionMatrix = Canvas->ViewProjectionMatrix;
    float    X = Location.X * ViewProjectionMatrix.M[0][0] + Location.Y * ViewProjectionMatrix.M[1][0] +
              Location.Z * ViewProjectionMatrix.M[2][0] + ViewProjectionMatrix.M[3][0];
    float Y = Location.X * ViewProjectionMatrix.M[0][1] + Location.Y * ViewProjectionMatrix.M[1][1] +
              Location.Z * ViewProjectionMatrix.M[2][1] + ViewProjectionMatrix.M[3][1];
    float Z = Location.X * ViewProjectionMatrix.M[0][2] + Location.Y * ViewProjectionMatrix.M[1][2] +
              Location.Z * ViewProjectionMatrix.M[2][2] + ViewProjectionMatrix.M[3][2];
    float W = Location.X * ViewProjectionMatrix.M[0][3] + Location.Y * ViewProjectionMatrix.M[1][3] +
              Location.Z * ViewProjectionMatrix.M[2][3] + ViewProjectionMatrix.M[3][3];

    X /= W;
    Y /= W;
    Z /= W;

    float ScreenX = (X + 1.f) * Core::g_ScreenCenterX;
    float ScreenY = (1.f - Y) * Core::g_ScreenCenterY;
    float ScreenZ = Z;

    return FVector(ScreenX, ScreenY, ScreenZ);
}
FVector2D Project(const FVector& Location) {
    FVector ScreenLocation = Project3D(Location);

    // If the depth (Z) value is not in front of the camera.
    if (ScreenLocation.Z < 0.f)
        return FVector2D(-1.f, -1.f);

    return FVector2D(ScreenLocation.X, ScreenLocation.Y);
}

} // namespace SDK
