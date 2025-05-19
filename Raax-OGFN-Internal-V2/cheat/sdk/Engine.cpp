#include "Engine.h"

#include <cheat/sdk/sdk.h>
#include <cheat/core.h>

namespace SDK {

// --- Classes & Structs ---------------------------------------------

bool (*UKismetSystemLibrary::KismetSystemLibraryLineTraceSingle)(UObject*, const FVector&, const FVector&,
                                                                 ETraceTypeQuery, bool, TArray<class AActor*>&,
                                                                 EDrawDebugTrace, FHitResult&, bool,
                                                                 const FLinearColor&, const FLinearColor&,
                                                                 float) = nullptr;
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

UGameViewportClient* UEngine::GameViewport() {
    static PropertyInfo Prop = GetPropertyInfo("Engine", "GameViewport");
    if (this && Prop.Found)
        return *(UGameViewportClient**)((uintptr_t)this + Prop.Offset);
    return nullptr;
}

ULevel* UWorld::PersistentLevel() {
    static PropertyInfo Prop = GetPropertyInfo("World", "PersistentLevel");
    if (this && Prop.Found)
        return *(ULevel**)((uintptr_t)this + Prop.Offset);
    return nullptr;
}
TArray<ULevel*>* UWorld::Levels() {
    static PropertyInfo Prop = GetPropertyInfo("World", "Levels");
    if (this && Prop.Found)
        return (TArray<ULevel*>*)((uintptr_t)this + Prop.Offset);
    return nullptr;
}

uint32_t         ULevel::Actors_Offset;
TArray<AActor*>* ULevel::Actors() {
    if (this)
        return (TArray<AActor*>*)((uintptr_t)this + Actors_Offset);
    return nullptr;
}
AWorldSettings* ULevel::WorldSettings() {
    static PropertyInfo Prop = GetPropertyInfo("Level", "WorldSettings");
    if (this && Prop.Found)
        return *(AWorldSettings**)((uintptr_t)this + Prop.Offset);
    return nullptr;
}

TArray<ULocalPlayer*>* UGameInstance::LocalPlayers() {
    static PropertyInfo Prop = GetPropertyInfo("GameInstance", "LocalPlayers");
    if (this && Prop.Found)
        return (TArray<ULocalPlayer*>*)((uintptr_t)this + Prop.Offset);
    return nullptr;
}

UWorld* UGameViewportClient::World() {
    static PropertyInfo Prop = GetPropertyInfo("GameViewportClient", "World");
    if (this && Prop.Found)
        return *(UWorld**)((uintptr_t)this + Prop.Offset);
    return nullptr;
}
UGameInstance* UGameViewportClient::GameInstance() {
    static PropertyInfo Prop = GetPropertyInfo("GameViewportClient", "GameInstance");
    if (this && Prop.Found)
        return *(UGameInstance**)((uintptr_t)this + Prop.Offset);
    return nullptr;
}

int32_t UFont::LegacyFontSize() {
    static PropertyInfo Prop = GetPropertyInfo("Font", "LegacyFontSize");
    if (this && Prop.Found)
        return *(int32_t*)((uintptr_t)this + Prop.Offset);
    return {};
}
void UFont::Set_LegacyFontSize(int32_t Value) {
    static PropertyInfo Prop = GetPropertyInfo("Font", "LegacyFontSize");
    if (this && Prop.Found)
        *(int32_t*)((uintptr_t)this + Prop.Offset) = Value;
}

uint32_t UCanvas::ViewProjectionMatrix_Offset;
int32_t  UCanvas::SizeX() {
    static PropertyInfo Prop = GetPropertyInfo("Canvas", "SizeX");
    if (this && Prop.Found)
        return *(int32_t*)((uintptr_t)this + Prop.Offset);
    return {};
}
int32_t UCanvas::SizeY() {
    static PropertyInfo Prop = GetPropertyInfo("Canvas", "SizeY");
    if (this && Prop.Found)
        return *(int32_t*)((uintptr_t)this + Prop.Offset);
    return {};
}
FMatrix* UCanvas::ViewProjectionMatrix() {
    if (this)
        return (FMatrix*)((uintptr_t)this + ViewProjectionMatrix_Offset);
    return nullptr;
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

    if (this && Func)
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

        if (this && Func)
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

        if (this && Func)
            ProcessEvent(Func, &params_K2_DrawText_2);
    }
}

APlayerController* UPlayer::PlayerController() {
    static PropertyInfo Prop = GetPropertyInfo("Player", "PlayerController");
    if (this && Prop.Found)
        return *(APlayerController**)((uintptr_t)this + Prop.Offset);
    return nullptr;
}

uint32_t USceneComponent::ComponentToWorld_Offset;
FVector  USceneComponent::RelativeLocation() {
    static PropertyInfo Prop = GetPropertyInfo("SceneComponent", "RelativeLocation");
    if (this && Prop.Found)
        return *(FVector*)((uintptr_t)this + Prop.Offset);
    return {};
}
FVector USceneComponent::ComponentVelocity() {
    static PropertyInfo Prop = GetPropertyInfo("SceneComponent", "ComponentVelocity");
    if (this && Prop.Found)
        return *(FVector*)((uintptr_t)this + Prop.Offset);
    return {};
}
FTransform USceneComponent::ComponentToWorld() {
    if (this) {
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

    return {};
}

uint32_t            USkinnedMeshComponent::ComponentSpaceTransformsArray_Offset;
TArray<FTransform>* USkinnedMeshComponent::ComponentSpaceTransformsArray() {
    if (this) {
        TArray<FTransform>* FirstArray = (TArray<FTransform>*)((uintptr_t)this + ComponentSpaceTransformsArray_Offset);
        if (FirstArray && FirstArray->IsValid())
            return FirstArray;

        TArray<FTransform>* SecondArray =
            (TArray<FTransform>*)((uintptr_t)this + ComponentSpaceTransformsArray_Offset + sizeof(TArray<FTransform>));
        if (SecondArray && SecondArray->IsValid())
            return SecondArray;
    }
    return nullptr;
}
int32_t USkinnedMeshComponent::GetBoneIndex(FName BoneName) {
    static UFunction* Func = GetFunction("SkinnedMeshComponent", "GetBoneIndex");
    struct {
        FName   BoneName;
        int32_t ReturnValue;
    } params_GetBoneIndex{};

    params_GetBoneIndex.BoneName = BoneName;

    if (this && Func)
        ProcessEvent(Func, &params_GetBoneIndex);

    return params_GetBoneIndex.ReturnValue;
}

FTransform USkeletalMeshComponent::GetBoneMatrix(int32_t BoneIndex) {
    if (this) {
        TArray<FTransform>* Array = ComponentSpaceTransformsArray();
        if (Array && Array->IsValid())
            return Array->GetByIndex(BoneIndex);
    }
    return {};
}
FVector USkeletalMeshComponent::GetBoneLocation(int32_t BoneIndex) {
    if (this) {
        FTransform BoneMatrix = GetBoneMatrix(BoneIndex);
        FTransform ComponentToWrld = ComponentToWorld();

        FMatrix Matrix = BoneMatrix.ToMatrixWithScale() * ComponentToWrld.ToMatrixWithScale();
        return FVector(Matrix.M[3][0], Matrix.M[3][1], Matrix.M[3][2]);
    }
    return {};
}

void USkeletalMeshComponent::Set_LastPoseTickFrame(uint32_t Value) {
    static PropertyInfo Prop = GetPropertyInfo("SkeletalMeshComponent", "LastPoseTickFrame");
    if (this && Prop.Found)
        *(uint32_t*)((uintptr_t)this + Prop.Offset) = Value;
    return;
}

USceneComponent* AActor::RootComponent() {
    static PropertyInfo Prop = GetPropertyInfo("Actor", "RootComponent");
    if (this && Prop.Found)
        return *(USceneComponent**)((uintptr_t)this + Prop.Offset);
    return nullptr;
}
float AActor::WasRecentlyRendered(float Tolerence) {
    static UFunction* Func = GetFunction("Actor", "WasRecentlyRendered");
    struct {
        float Tolerence;
        float ReturnValue;
    } params_WasRecentlyRendered{};

    params_WasRecentlyRendered.Tolerence = Tolerence;

    if (this && Func)
        ProcessEvent(Func, &params_WasRecentlyRendered);

    return params_WasRecentlyRendered.ReturnValue;
}

float AWorldSettings::WorldGravityZ() {
    static PropertyInfo Prop = GetPropertyInfo("WorldSettings", "WorldGravityZ");
    if (this && Prop.Found)
        return *(float*)((uintptr_t)this + Prop.Offset);
    return 0.f;
}

FVector APlayerCameraManager::GetCameraLocation() {
    static UFunction* Func = GetFunction("PlayerCameraManager", "GetCameraLocation");
    struct {
        FVector ReturnValue;
    } params_GetCameraLocation{};

    if (this && Func)
        ProcessEvent(Func, &params_GetCameraLocation);

    return params_GetCameraLocation.ReturnValue;
}
FRotator APlayerCameraManager::GetCameraRotation() {
    static UFunction* Func = GetFunction("PlayerCameraManager", "GetCameraRotation");
    struct {
        FRotator ReturnValue;
    } params_GetCameraRotation{};

    if (this && Func)
        ProcessEvent(Func, &params_GetCameraRotation);

    return params_GetCameraRotation.ReturnValue;
}
float APlayerCameraManager::GetFOVAngle() {
    static UFunction* Func = GetFunction("PlayerCameraManager", "GetFOVAngle");
    struct {
        float ReturnValue;
    } params_GetFOVAngle{};

    if (this && Func)
        ProcessEvent(Func, &params_GetFOVAngle);

    return params_GetFOVAngle.ReturnValue;
}

APawn* APlayerController::AcknowledgedPawn() {
    static PropertyInfo Prop = GetPropertyInfo("PlayerController", "AcknowledgedPawn");
    if (this && Prop.Found)
        return *(APawn**)((uintptr_t)this + Prop.Offset);
    return nullptr;
}
APlayerCameraManager* APlayerController::PlayerCameraManager() {
    static PropertyInfo Prop = GetPropertyInfo("PlayerController", "PlayerCameraManager");
    if (this && Prop.Found)
        return *(APlayerCameraManager**)((uintptr_t)this + Prop.Offset);
    return nullptr;
}
float APlayerController::InputYawScale() {
    static PropertyInfo Prop = GetPropertyInfo("PlayerController", "InputYawScale");
    if (this && Prop.Found)
        return *(float*)((uintptr_t)this + Prop.Offset);
    return 0.f;
}
float APlayerController::InputPitchScale() {
    static PropertyInfo Prop = GetPropertyInfo("PlayerController", "InputPitchScale");
    if (this && Prop.Found)
        return *(float*)((uintptr_t)this + Prop.Offset);
    return 0.f;
}
void APlayerController::AddYawInput(float Val) {
    static UFunction* Func = GetFunction("PlayerController", "AddYawInput");
    struct {
        float Val;
    } params_AddYawInput{};

    params_AddYawInput.Val = Val;

    if (this && Func)
        ProcessEvent(Func, &params_AddYawInput);
}
void APlayerController::AddPitchInput(float Val) {
    static UFunction* Func = GetFunction("PlayerController", "AddPitchInput");
    struct {
        float Val;
    } params_AddPitchInput{};

    params_AddPitchInput.Val = Val;

    if (this && Func)
        ProcessEvent(Func, &params_AddPitchInput);
}

APlayerState* APawn::PlayerState() {
    static PropertyInfo Prop = GetPropertyInfo("Pawn", "PlayerState");
    if (this && Prop.Found)
        return *(APlayerState**)((uintptr_t)this + Prop.Offset);
    return nullptr;
}

USkeletalMeshComponent* ACharacter::Mesh() {
    static PropertyInfo Prop = GetPropertyInfo("Character", "Mesh");
    if (this && Prop.Found)
        return *(USkeletalMeshComponent**)((uintptr_t)this + Prop.Offset);
    return nullptr;
}

FString APlayerState::GetPlayerName() {
    static UFunction* Func = GetFunction("PlayerState", "GetPlayerName");
    struct {
        FString return_value;
    } params_GetPlayerName{};

    if (this && Func)
        ProcessEvent(Func, &params_GetPlayerName);

    return params_GetPlayerName.return_value;
}

// --- Public Functions ----------------------------------------------

ULocalPlayer* GetLocalPlayer() {
    return GetEngine()->GameViewport()->GameInstance()->LocalPlayers()->GetByIndex(0);
}
APlayerController* GetLocalController() {
    return GetLocalPlayer()->PlayerController();
}
APawn* GetLocalPawn() {
    return GetLocalController()->AcknowledgedPawn();
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
    return GetEngine()->GameViewport()->World();
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

SDK::FVector GetCameraLocation() {
    return SDK::GetLocalController()->PlayerCameraManager()->GetCameraLocation();
}
SDK::FRotator GetCameraRotation() {
    return SDK::GetLocalController()->PlayerCameraManager()->GetCameraRotation();
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
    int32_t ScreenWidth = GetCanvas()->SizeX();
    int32_t ScreenHeight = GetCanvas()->SizeY();

    FMatrix* ViewProjectionMatrix = GetCanvas()->ViewProjectionMatrix();
    if (!ViewProjectionMatrix)
        return FVector();

    float X = Location.X * ViewProjectionMatrix->M[0][0] + Location.Y * ViewProjectionMatrix->M[1][0] +
              Location.Z * ViewProjectionMatrix->M[2][0] + ViewProjectionMatrix->M[3][0];
    float Y = Location.X * ViewProjectionMatrix->M[0][1] + Location.Y * ViewProjectionMatrix->M[1][1] +
              Location.Z * ViewProjectionMatrix->M[2][1] + ViewProjectionMatrix->M[3][1];
    float Z = Location.X * ViewProjectionMatrix->M[0][2] + Location.Y * ViewProjectionMatrix->M[1][2] +
              Location.Z * ViewProjectionMatrix->M[2][2] + ViewProjectionMatrix->M[3][2];
    float W = Location.X * ViewProjectionMatrix->M[0][3] + Location.Y * ViewProjectionMatrix->M[1][3] +
              Location.Z * ViewProjectionMatrix->M[2][3] + ViewProjectionMatrix->M[3][3];

    X /= W;
    Y /= W;
    Z /= W;

    float ScreenX = (X + 1.f) * (ScreenWidth / 2.f);
    float ScreenY = (1.f - Y) * (ScreenHeight / 2.f);
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
