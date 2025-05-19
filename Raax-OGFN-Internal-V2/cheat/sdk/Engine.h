#pragma once

#include "Containers.h"
#include "CoreUObject.h"
#include "Basic.h"
#include <vector>

#include <utils/log.h>

namespace SDK {

// --- Classes & Structs ---------------------------------------------

enum class ETraceTypeQuery : uint8_t {
    TraceTypeQuery1 = 0,
    TraceTypeQuery2 = 1,
    TraceTypeQuery3 = 2,
    TraceTypeQuery4 = 3,
    TraceTypeQuery5 = 4,
    TraceTypeQuery6 = 5,
    TraceTypeQuery7 = 6,
    TraceTypeQuery8 = 7,
    TraceTypeQuery9 = 8,
    TraceTypeQuery10 = 9,
    TraceTypeQuery11 = 10,
    TraceTypeQuery12 = 11,
    TraceTypeQuery13 = 12,
    TraceTypeQuery14 = 13,
    TraceTypeQuery15 = 14,
    TraceTypeQuery16 = 15,
    TraceTypeQuery17 = 16,
    TraceTypeQuery18 = 17,
    TraceTypeQuery19 = 18,
    TraceTypeQuery20 = 19,
    TraceTypeQuery21 = 20,
    TraceTypeQuery22 = 21,
    TraceTypeQuery23 = 22,
    TraceTypeQuery24 = 23,
    TraceTypeQuery25 = 24,
    TraceTypeQuery26 = 25,
    TraceTypeQuery27 = 26,
    TraceTypeQuery28 = 27,
    TraceTypeQuery29 = 28,
    TraceTypeQuery30 = 29,
    TraceTypeQuery31 = 30,
    TraceTypeQuery32 = 31,
    TraceTypeQuery_MAX = 32,
    ETraceTypeQuery_MAX = 33,
};

enum class EDrawDebugTrace : uint8_t {
    None = 0,
    ForOneFrame = 1,
    ForDuration = 2,
    Persistent = 3,
    EDrawDebugTrace_MAX = 4,
};

struct FHitResult {
  public:
    char UnknownData[0x100]; // Dummy data
};

class UKismetSystemLibrary : public UObject {
  public:
    static bool (*KismetSystemLibraryLineTraceSingle)(UObject*, const FVector&, const FVector&, ETraceTypeQuery, bool,
                                                      TArray<class AActor*>&, EDrawDebugTrace, FHitResult&, bool,
                                                      const FLinearColor&, const FLinearColor&, float);

  public:
    static FString GetEngineVersion();
    static bool    LineTraceSingle(UObject* WorldContextObject, const FVector& Start, const FVector& End,
                                   ETraceTypeQuery TraceChannel, bool bTraceComplex, TArray<AActor*>& ActorsToIgnore,
                                   EDrawDebugTrace DrawDebugType, FHitResult& OutHit, bool bIgnoreSelf,
                                   const FLinearColor& TraceColor, const FLinearColor& TraceHitColor, float DrawTime);

  public:
    STATICCLASS_DEFAULTOBJECT("KismetSystemLibrary", UKismetSystemLibrary)
};

class UEngine : public UObject {
  public:
    class UGameViewportClient* GameViewport();

  public:
    STATICCLASS_DEFAULTOBJECT("Engine", UEngine)
};

class UWorld : public UObject {
  public:
    class ULevel*    PersistentLevel();
    TArray<ULevel*>* Levels();

  public:
    STATICCLASS_DEFAULTOBJECT("World", UWorld)
};

class ULevel : public UObject {
  public:
    static uint32_t Actors_Offset;

  public:
    TArray<AActor*>*      Actors();
    class AWorldSettings* WorldSettings();

  public:
    STATICCLASS_DEFAULTOBJECT("Level", ULevel)
};

class UGameInstance : public UObject {
  public:
    TArray<class ULocalPlayer*>* LocalPlayers();

  public:
    STATICCLASS_DEFAULTOBJECT("GameInstance", UGameInstance)
};

class UGameViewportClient : public UObject {
  public:
    static inline int DrawTransition_Idx;

  public:
    UWorld*        World();
    UGameInstance* GameInstance();

  public:
    STATICCLASS_DEFAULTOBJECT("GameViewportClient", UGameViewportClient)
};

class UFont : public UObject {
  public:
    int32_t LegacyFontSize();
    void    Set_LegacyFontSize(int32_t Value);

  public:
    STATICCLASS_DEFAULTOBJECT("Font", UFont)
};

class UCanvas : public UObject {
  public:
    static uint32_t ViewProjectionMatrix_Offset;

  public:
    int32_t  SizeX();
    int32_t  SizeY();
    FMatrix* ViewProjectionMatrix();

  public:
    void K2_DrawLine(const FVector2D& ScreenPositionA, const FVector2D& ScreenPositionB, float Thickness,
                     const FLinearColor& RenderColor);
    void K2_DrawText(UFont* RenderFont, const FString& RenderText, const FVector2D& ScreenPosition,
                     const FVector2D& Scale, const FLinearColor& RenderColor, float Kerning,
                     const FLinearColor& ShadowColor, const FVector2D& ShadowOffset, bool bCentreX, bool bCentreY,
                     bool bOutlined, const FLinearColor& OutlineColor);

  public:
    STATICCLASS_DEFAULTOBJECT("Canvas", UCanvas)
};

class UPlayer : public UObject {
  public:
    class APlayerController* PlayerController();

  public:
    STATICCLASS_DEFAULTOBJECT("Player", UPlayer)
};

class ULocalPlayer : public UPlayer {
  public:
    STATICCLASS_DEFAULTOBJECT("LocalPlayer", ULocalPlayer)
};

class USceneComponent : public UObject {
  public:
    static uint32_t ComponentToWorld_Offset;

  public:
    FVector    RelativeLocation();
    FVector    ComponentVelocity();
    FTransform ComponentToWorld();

  public:
    STATICCLASS_DEFAULTOBJECT("SceneComponent", USceneComponent)
};

class USkinnedMeshComponent : public USceneComponent {
  public:
    static uint32_t ComponentSpaceTransformsArray_Offset;

  public:
    TArray<FTransform>* ComponentSpaceTransformsArray();
    int32_t             GetBoneIndex(FName BoneName);

  public:
    STATICCLASS_DEFAULTOBJECT("SkinnedMeshComponent", USkinnedMeshComponent)
};

class USkeletalMeshComponent : public USkinnedMeshComponent {
  public:
    FTransform GetBoneMatrix(int32_t BoneIndex);
    FVector    GetBoneLocation(int32_t BoneIndex);
    void       Set_LastPoseTickFrame(uint32_t Value);

  public:
    STATICCLASS_DEFAULTOBJECT("SkeletalMeshComponent", USkeletalMeshComponent)
};

class AActor : public UObject {
  public:
    USceneComponent* RootComponent();
    float            WasRecentlyRendered(float Tolerence);

  public:
    STATICCLASS_DEFAULTOBJECT("Actor", AActor)
};

class AWorldSettings : AActor {
  public:
    float WorldGravityZ();

  public:
    STATICCLASS_DEFAULTOBJECT("WorldSettings", AWorldSettings)
};

class APlayerCameraManager : public AActor {
  public:
    FVector  GetCameraLocation();
    FRotator GetCameraRotation();
    float    GetFOVAngle();

  public:
    STATICCLASS_DEFAULTOBJECT("PlayerCameraManager", APlayerCameraManager)
};

class APlayerController : public AActor {
  public:
    class APawn*          AcknowledgedPawn();
    APlayerCameraManager* PlayerCameraManager();
    float                 InputYawScale();
    float                 InputPitchScale();
    void                  AddYawInput(float Val);
    void                  AddPitchInput(float Val);

  public:
    STATICCLASS_DEFAULTOBJECT("PlayerController", APlayerController)
};

class APawn : public AActor {
  public:
    class APlayerState* PlayerState();

  public:
    STATICCLASS_DEFAULTOBJECT("Pawn", APawn)
};

class ACharacter : public APawn {
  public:
    USkeletalMeshComponent* Mesh();

  public:
    STATICCLASS_DEFAULTOBJECT("Character", ACharacter)
};

class APlayerState : public AActor {
  public:
    FString GetPlayerName();

  public:
    STATICCLASS_DEFAULTOBJECT("PlayerState", APlayerState)
};

// --- Public Functions ----------------------------------------------

ULocalPlayer*      GetLocalPlayer();
APlayerController* GetLocalController();
APawn*             GetLocalPawn();

UEngine* GetEngine();
UWorld*  GetWorld();
UCanvas* GetCanvas();

FVector  GetCameraLocation();
FRotator GetCameraRotation();

bool IsPositionVisible(const FVector& Position, AActor* IgnoredActor = nullptr, AActor* IgnoredActor2 = nullptr);

FVector   Project3D(const FVector& Location);
FVector2D Project(const FVector& Location);

template <typename UEType = AActor, bool ClearVector = true>
inline void GetAllActorsOfClass(std::vector<UEType*>& OutVector, ULevel* Level = GetWorld()->PersistentLevel()) {
    static_assert(std::is_base_of_v<AActor, UEType>,
                  "Cannot call GetAllActorsOfClass with class that doesn't inherit from AActor!");
    if constexpr (ClearVector)
        OutVector.clear();

    if (!Level)
        return;

    TArray<AActor*>* Actors = Level->Actors();
    if (!Actors || !Actors->IsValid() || Actors->Num() <= 0)
        return;

    for (int i = 0; i < Actors->Num(); i++) {
        AActor* Actor = Actors->GetByIndex(i);
        if (!Actor)
            continue;

        if (Actor->IsA(UEType::StaticClass()))
            OutVector.push_back(static_cast<UEType*>(Actor));
    }
}

template <typename UEType = AActor, bool ClearVector = true>
inline void GetAllActorsOfClassAllLevels(std::vector<UEType*>& OutVector) {
    if constexpr (ClearVector)
        OutVector.clear();

    TArray<ULevel*>* Levels = GetWorld()->Levels();
    if (!Levels || !Levels->IsValid() || Levels->Num() <= 0)
        return;

    for (int i = 0; i < Levels->Num(); i++) {
        GetAllActorsOfClass<UEType, false>(OutVector, Levels->GetByIndex(i));
    }
}

} // namespace SDK
