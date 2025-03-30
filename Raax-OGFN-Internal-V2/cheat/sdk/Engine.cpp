#include "Engine.h"

namespace SDK
{
	class ULocalPlayer* GetLocalPlayer() {
		return GetEngine()->GameViewport()->GameInstance()->LocalPlayers()[0];
	}
	class APlayerController* GetLocalController() {
		return GetLocalPlayer()->PlayerController();
	}
	class APawn* GetLocalPawn() {
		return GetLocalController()->AcknowledgedPawn();
	}

	class UEngine* GetEngine() {
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
	class UWorld* GetWorld() {
		return GetEngine()->GameViewport()->World();
	}
	class UCanvas* GetCanvas() {
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

	FVector Project3D(const FVector& Location) {
		int32_t ScreenWidth = GetCanvas()->SizeX();
		int32_t ScreenHeight = GetCanvas()->SizeY();

		FMatrix* ViewProjectionMatrix = GetCanvas()->ViewProjectionMatrix();
		if (!ViewProjectionMatrix)
			return FVector();

		float X = Location.X * ViewProjectionMatrix->M[0][0] + Location.Y * ViewProjectionMatrix->M[1][0] + Location.Z * ViewProjectionMatrix->M[2][0] + ViewProjectionMatrix->M[3][0];
		float Y = Location.X * ViewProjectionMatrix->M[0][1] + Location.Y * ViewProjectionMatrix->M[1][1] + Location.Z * ViewProjectionMatrix->M[2][1] + ViewProjectionMatrix->M[3][1];
		float Z = Location.X * ViewProjectionMatrix->M[0][2] + Location.Y * ViewProjectionMatrix->M[1][2] + Location.Z * ViewProjectionMatrix->M[2][2] + ViewProjectionMatrix->M[3][2];
		float W = Location.X * ViewProjectionMatrix->M[0][3] + Location.Y * ViewProjectionMatrix->M[1][3] + Location.Z * ViewProjectionMatrix->M[2][3] + ViewProjectionMatrix->M[3][3];

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
			return {};

		return FVector2D(ScreenLocation.X, ScreenLocation.Y);
	}
}
