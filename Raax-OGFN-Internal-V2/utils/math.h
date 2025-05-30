#pragma once
#include <cheat/sdk/Basic.h>

constexpr auto M_PI = 3.14159265358979323f;

namespace Math {

void          SinCos(float* Sine, float* Cosine, float AngleRad);
float         InvSqrt(float F);
float         DegreesToRadians(float Degrees);
float         RadiansToDegrees(float Radians);
SDK::FRotator FindLookAtRotation(const SDK::FVector& Start, const SDK::FVector& Target);
SDK::FVector  GetForwardVector(const SDK::FRotator& InRot);
float         GetDegreeDistance(const SDK::FRotator& Rotator1, const SDK::FRotator& Rotator2);
bool          IsOnScreen(const SDK::FVector2D& Position);

} // namespace Math
