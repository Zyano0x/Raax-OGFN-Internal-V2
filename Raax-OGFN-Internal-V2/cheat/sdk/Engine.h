#pragma once
#include "Containers.h"
#include "CoreUObject.h"
#include <utils/log.h>
#include <vector>

namespace SDK
{
	class UKismetSystemLibrary : public UObject
	{
	public:
		static FString GetEngineVersion() {
			static UFunction* Func = GetFunction("KismetSystemLibrary", "GetEngineVersion");
			struct {
				FString ReturnValue;
			} params{};

			if (Func)
				StaticClass()->ProcessEvent(Func, &params);
			else
				LOG(LOG_WARN, "Failed to find UKismetSystemLibrary::GetEngineVersion!");

			return params.ReturnValue;
		}

	public:
		STATICCLASS_DEFAULTOBJECT("KismetSystemLibrary", UKismetSystemLibrary)
	};


	class UEngine : public UObject
	{
	public:
		inline class UGameViewportClient* GameViewport() {
			static PropertyInfo Prop = GetPropertyInfo("Engine", "GameViewport");
			if (this && Prop.Found)
				return *(class UGameViewportClient**)((uintptr_t)this + Prop.Offset);
			return nullptr;
		}

	public:
		STATICCLASS_DEFAULTOBJECT("Engine", UEngine)
	};

	class UWorld : public UObject
	{
	public:
		inline class ULevel* PersistentLevel() {
			static PropertyInfo Prop = GetPropertyInfo("World", "PersistentLevel");
			if (this && Prop.Found)
				return *(class ULevel**)((uintptr_t)this + Prop.Offset);
			return nullptr;
		}

		inline TArray<class ULevel*>* Levels() {
			static PropertyInfo Prop = GetPropertyInfo("World", "Levels");
			if (this && Prop.Found)
				return (TArray<class ULevel*>*)((uintptr_t)this + Prop.Offset);
			return nullptr;
		}

	public:
		STATICCLASS_DEFAULTOBJECT("World", UWorld)
	};

	class ULevel : public UObject
	{
	public:
		static inline uint32_t Actors_Offset;

	public:
		inline TArray<class AActor*>* Actors() {
			if (this)
				return (TArray<class AActor*>*)((uintptr_t)this + Actors_Offset);
			return nullptr;
		}

	public:
		STATICCLASS_DEFAULTOBJECT("Level", ULevel)
	};

	class UGameInstance : public UObject
	{
	public:
		inline TArray<class ULocalPlayer*>* LocalPlayers() {
			static PropertyInfo Prop = GetPropertyInfo("GameInstance", "LocalPlayers");
			if (this && Prop.Found)
				return (TArray<class ULocalPlayer*>*)((uintptr_t)this + Prop.Offset);
			return nullptr;
		}

	public:
		STATICCLASS_DEFAULTOBJECT("GameInstance", UGameInstance)
	};

	class UGameViewportClient : public UObject
	{
	public:
		static inline int DrawTransition_Idx;

	public:
		inline class UWorld* World() {
			static PropertyInfo Prop = GetPropertyInfo("GameViewportClient", "World");
			if (this && Prop.Found)
				return *(class UWorld**)((uintptr_t)this + Prop.Offset);
			return nullptr;
		}
		inline class UGameInstance* GameInstance() {
			static PropertyInfo Prop = GetPropertyInfo("GameViewportClient", "GameInstance");
			if (this && Prop.Found)
				return *(class UGameInstance**)((uintptr_t)this + Prop.Offset);
			return nullptr;
		}

	public:
		STATICCLASS_DEFAULTOBJECT("GameViewportClient", UGameViewportClient)
	};

	class UCanvas : public UObject
	{
	public:
		static inline uint32_t ViewProjectionMatrix_Offset;

	public:
		inline int32_t SizeX() {
			static PropertyInfo Prop = GetPropertyInfo("Canvas", "SizeX");
			if (this && Prop.Found)
				return *(int32_t*)((uintptr_t)this + Prop.Offset);
			return {};
		}
		inline int32_t SizeY() {
			static PropertyInfo Prop = GetPropertyInfo("Canvas", "SizeY");
			if (this && Prop.Found)
				return *(int32_t*)((uintptr_t)this + Prop.Offset);
			return {};
		}
		inline FMatrix* ViewProjectionMatrix() {
			if (this)
				return (FMatrix*)((uintptr_t)this + ViewProjectionMatrix_Offset);
			return nullptr;
		}

	public:
		STATICCLASS_DEFAULTOBJECT("Canvas", UCanvas)
	};


	class UPlayer : public UObject
	{
	public:
		class APlayerController* PlayerController() {
			static PropertyInfo Prop = GetPropertyInfo("Player", "PlayerController");
			if (this && Prop.Found)
				return *(class APlayerController**)((uintptr_t)this + Prop.Offset);
			return nullptr;
		}

	public:
		STATICCLASS_DEFAULTOBJECT("Player", UPlayer)
	};

	class ULocalPlayer : public UPlayer
	{
	public:
		STATICCLASS_DEFAULTOBJECT("LocalPlayer", ULocalPlayer)
	};

	class USceneComponent : public UObject
	{
	public:
		static inline uint32_t ComponentToWorld_Offset;

	public:
		FVector RelativeLocation() {
			static PropertyInfo Prop = GetPropertyInfo("SceneComponent", "RelativeLocation");
			if (this && Prop.Found)
				return *(FVector*)((uintptr_t)this + Prop.Offset);
			return {};
		}
		FTransform ComponentToWorld() {
			if (this) {
				if (ComponentToWorld_Offset) {
					return *(FTransform*)((uintptr_t)this + ComponentToWorld_Offset);
				}
				else {
					static UFunction* Func = GetFunction("SceneComponent", "K2_GetComponentToWorld");
					struct {
						FTransform ReturnValue;
					} params{};

					if (Func)
						ProcessEvent(Func, &params);
					else
						LOG(LOG_WARN, "Failed to find USceneComponent::ComponentToWorld!");

					return params.ReturnValue;
				}
			}

			return {};
		}

	public:
		STATICCLASS_DEFAULTOBJECT("SceneComponent", USceneComponent)
	};

	class USkinnedMeshComponent : public USceneComponent
	{
	public:
		static inline uint32_t ComponentSpaceTransformsArray_Offset;

	public:
		TArray<FTransform>* ComponentSpaceTransformsArray() {
			if (this) {
				TArray<FTransform>* FirstArray = (TArray<FTransform>*)((uintptr_t)this + ComponentSpaceTransformsArray_Offset);
				if (FirstArray && FirstArray->IsValid())
					return FirstArray;

				TArray<FTransform>* SecondArray = (TArray<FTransform>*)((uintptr_t)this + ComponentSpaceTransformsArray_Offset + sizeof(TArray<FTransform>));
				if (SecondArray && SecondArray->IsValid())
					return SecondArray;
			}
			return nullptr;
		}
		int32_t GetBoneIndex(FName BoneName) {
			static UFunction* Func = GetFunction("SkinnedMeshComponent", "GetBoneIndex");
			struct {
				FName BoneName;
				int32_t ReturnValue;
			} params{};

			params.BoneName = BoneName;

			if (this && Func)
				ProcessEvent(Func, &params);

			return params.ReturnValue;
		}
	};

	class USkeletalMeshComponent : public USkinnedMeshComponent
	{
	public:
		FTransform GetBoneMatrix(int32_t BoneIndex) {
			if (this) {
				TArray<FTransform>* Array = ComponentSpaceTransformsArray();
				if (Array && Array->IsValid())
					return Array->GetByIndex(BoneIndex);
			}
			return {};
		}
		FVector GetBoneLocation(int32_t BoneIndex) {
			if (this) {
				FTransform BoneMatrix = GetBoneMatrix(BoneIndex);
				FTransform ComponentToWrld = ComponentToWorld();

				FMatrix Matrix = BoneMatrix.ToMatrixWithScale() * ComponentToWrld.ToMatrixWithScale();
				return FVector(Matrix.M[3][0], Matrix.M[3][1], Matrix.M[3][2]);
			}
			return {};
		}
	};


	class AActor : public UObject
	{
	public:
		class USceneComponent* RootComponent() {
			static PropertyInfo Prop = GetPropertyInfo("Actor", "RootComponent");
			if (this && Prop.Found)
				return *(class USceneComponent**)((uintptr_t)this + Prop.Offset);
			return nullptr;
		}

	public:
		STATICCLASS_DEFAULTOBJECT("Actor", AActor)
	};

	class APlayerController : public AActor
	{
	public:
		class APawn* AcknowledgedPawn() {
			static PropertyInfo Prop = GetPropertyInfo("PlayerController", "AcknowledgedPawn");
			if (this && Prop.Found)
				return *(class APawn**)((uintptr_t)this + Prop.Offset);
			return nullptr;
		}

	public:
		STATICCLASS_DEFAULTOBJECT("PlayerController", APlayerController)
	};

	class APawn : public AActor
	{
	public:
		class APlayerState* PlayerState() {
			static PropertyInfo Prop = GetPropertyInfo("Pawn", "PlayerState");
			if (this && Prop.Found)
				return *(class APlayerState**)((uintptr_t)this + Prop.Offset);
			return nullptr;
		}

	public:
		STATICCLASS_DEFAULTOBJECT("Pawn", APawn)
	};

	class ACharacter : public APawn
	{
	public:
		class USkeletalMeshComponent* Mesh() {
			static PropertyInfo Prop = GetPropertyInfo("Character", "Mesh");
			if (this && Prop.Found)
				return *(class USkeletalMeshComponent**)((uintptr_t)this + Prop.Offset);
			return nullptr;
		}

	public:
		STATICCLASS_DEFAULTOBJECT("Character", ACharacter)
	};

	class APlayerState : public AActor
	{
	public:
		inline FString GetPlayerName() {
			static UFunction* Func = GetFunction("PlayerState", "GetPlayerName");
			struct {
				FString return_value;
			} params_GetPlayerName{};

			if (this && Func)
				ProcessEvent(Func, &params_GetPlayerName);

			return params_GetPlayerName.return_value;
		}

	public:
		STATICCLASS_DEFAULTOBJECT("PlayerState", APlayerState)
	};


	class ULocalPlayer* GetLocalPlayer();
	class APlayerController* GetLocalController();
	class APawn* GetLocalPawn();

	class UEngine* GetEngine();
	class UWorld* GetWorld();
	class UCanvas* GetCanvas();

	FVector Project3D(const FVector& Location);
	FVector2D Project(const FVector& Location);

	template<typename UEType = AActor, bool ClearVector = true>
	inline void GetAllActorsOfClass(std::vector<UEType*>& OutVector, ULevel* Level = GetWorld()->PersistentLevel()) {
		static_assert(std::is_base_of_v<AActor, UEType>, "Cannot call GetAllActorsOfClass with class that doesn't inherit from AActor!");
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
	template<typename UEType = AActor, bool ClearVector = true>
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
}
