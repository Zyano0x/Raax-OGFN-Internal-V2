#pragma once
#include <cstdint>
#include <string>
#include <immintrin.h>

namespace SDK
{
    enum EFindName
    {
        FNAME_Find,
        FNAME_Add
    };

    class FName
    {
    public:
        static inline void(*FNameToString)(const FName*, class FString*) = nullptr;
        static inline void(*FNameConstructorW)(const FName*, const wchar_t*, EFindName) = nullptr;

    public:
        int32_t ComparisonIndex;
        int32_t Number;

    public:
        FName() : ComparisonIndex(0), Number(0) {}
        FName(const wchar_t* Name);
        FName(const char* Name);

    public:
        class FString ToFString() const;
        std::wstring ToWString() const;
        std::string ToString() const;

        bool operator==(const FName& Other) const;
        bool operator!=(const FName& Other) const;

        void operator=(const FName& Other);
        void operator=(const wchar_t* Name);
        void operator=(const char* Name);
    };

	enum class EFunctionFlags : uint32_t
	{
		None = 0x00000000,

		Final = 0x00000001,
		RequiredAPI = 0x00000002,
		BlueprintAuthorityOnly = 0x00000004,
		BlueprintCosmetic = 0x00000008,
		Net = 0x00000040,
		NetReliable = 0x00000080,
		NetRequest = 0x00000100,
		Exec = 0x00000200,
		Native = 0x00000400,
		Event = 0x00000800,
		NetResponse = 0x00001000,
		Static = 0x00002000,
		NetMulticast = 0x00004000,
		UbergraphFunction = 0x00008000,
		MulticastDelegate = 0x00010000,
		Public = 0x00020000,
		Private = 0x00040000,
		Protected = 0x00080000,
		Delegate = 0x00100000,
		NetServer = 0x00200000,
		HasOutParms = 0x00400000,
		HasDefaults = 0x00800000,
		NetClient = 0x01000000,
		DLLImport = 0x02000000,
		BlueprintCallable = 0x04000000,
		BlueprintEvent = 0x08000000,
		BlueprintPure = 0x10000000,
		EditorOnly = 0x20000000,
		Const = 0x40000000,
		NetValidate = 0x80000000,

		AllFlags = 0xFFFFFFFF,
	};
	inline bool operator&(EFunctionFlags Left, EFunctionFlags Right) {
		using CastFlagsType = std::underlying_type<EFunctionFlags>::type;
		return (static_cast<CastFlagsType>(Left) & static_cast<CastFlagsType>(Right)) == static_cast<CastFlagsType>(Right);
	}
	inline constexpr SDK::EFunctionFlags operator|(SDK::EFunctionFlags Left, SDK::EFunctionFlags Right) {
		return (SDK::EFunctionFlags)((std::underlying_type<SDK::EFunctionFlags>::type)(Left) | (std::underlying_type<SDK::EFunctionFlags>::type)(Right));
	}

	enum class EClassCastFlags : uint64_t
	{
		None				= 0x0000000000000000,

		Field				= 0x0000000000000001,
		Int8Property		= 0x0000000000000002,
		Enum				= 0x0000000000000004,
		Struct				= 0x0000000000000008,
		ScriptStruct		= 0x0000000000000010,
		Class				= 0x0000000000000020,
		ByteProperty		= 0x0000000000000040,
		IntProperty			= 0x0000000000000080,
		FloatProperty		= 0x0000000000000100,
		UInt64Property		= 0x0000000000000200,
		ClassProperty		= 0x0000000000000400,
		UInt32Property		= 0x0000000000000800,
		InterfaceProperty	= 0x0000000000001000,
		NameProperty		= 0x0000000000002000,
		StrProperty			= 0x0000000000004000,
		Property			= 0x0000000000008000,
		ObjectProperty		= 0x0000000000010000,
		BoolProperty		= 0x0000000000020000,
		UInt16Property		= 0x0000000000040000,
		Function			= 0x0000000000080000,
		StructProperty		= 0x0000000000100000,
		ArrayProperty		= 0x0000000000200000,
		Int64Property		= 0x0000000000400000,
		DelegateProperty	= 0x0000000000800000,
		NumericProperty		= 0x0000000001000000,
		MulticastDelegateProperty = 0x0000000002000000,
		ObjectPropertyBase	= 0x0000000004000000,
		WeakObjectProperty	= 0x0000000008000000,
		LazyObjectProperty	= 0x0000000010000000,
		SoftObjectProperty	= 0x0000000020000000,
		TextProperty		= 0x0000000040000000,
		Int16Property		= 0x0000000080000000,
		DoubleProperty		= 0x0000000100000000,
		SoftClassProperty	= 0x0000000200000000,
		Package				= 0x0000000400000000,
		Level				= 0x0000000800000000,
		Actor				= 0x0000001000000000,
		PlayerController	= 0x0000002000000000,
		Pawn				= 0x0000004000000000,
		SceneComponent		= 0x0000008000000000,
		PrimitiveComponent	= 0x0000010000000000,
		SkinnedMeshComponent = 0x0000020000000000,
		SkeletalMeshComponent = 0x0000040000000000,
		Blueprint			= 0x0000080000000000,
		DelegateFunction	= 0x0000100000000000,
		StaticMeshComponent = 0x0000200000000000,
		MapProperty			= 0x0000400000000000,
		SetProperty			= 0x0000800000000000,
		EnumProperty		= 0x0001000000000000,
	};
	inline bool operator&(EClassCastFlags Left, EClassCastFlags Right) {
		using CastFlagsType = std::underlying_type<EClassCastFlags>::type;
		return (static_cast<CastFlagsType>(Left) & static_cast<CastFlagsType>(Right)) == static_cast<CastFlagsType>(Right);
	}
	inline constexpr SDK::EClassCastFlags operator|(SDK::EClassCastFlags Left, SDK::EClassCastFlags Right) {
		return (SDK::EClassCastFlags)((std::underlying_type<SDK::EClassCastFlags>::type)(Left) | (std::underlying_type<SDK::EClassCastFlags>::type)(Right));
	}

	struct FVector
	{
	public:
		float X;
		float Y;
		float Z;

	public:
		inline FVector() : X(0.0), Y(0.0), Z(0.0) { }
		inline FVector(float X, float Y, float Z) : X(X), Y(Y), Z(Z) { }

	public:
		FVector operator+(const FVector& Other) const {
			return FVector(X + Other.X, Y + Other.Y, Z + Other.Z);
		}

		FVector operator-(const FVector& Other) const {
			return FVector(X - Other.X, Y - Other.Y, Z - Other.Z);
		}

		FVector operator*(const FVector& Other) const {
			return FVector(X * Other.X, Y * Other.Y, Z * Other.Z);
		}

		FVector operator/(const FVector& Other) const {
			return FVector(X / Other.X, Y / Other.Y, Z / Other.Z);
		}

		bool operator==(const FVector& Other) const {
			return X == Other.X && Y == Other.Y && Z == Other.Z;
		}

		bool operator!=(const FVector& Other) const {
			return X != Other.X || Y != Other.Y || Z != Other.Z;
		}

		FVector operator+=(const FVector& Other) {
			X += Other.X;
			Y += Other.Y;
			Z += Other.Z;
			return *this;
		}

		FVector operator-=(const FVector& Other) {
			X -= Other.X;
			Y -= Other.Y;
			Z -= Other.Z;
			return *this;
		}

		FVector operator*=(const FVector& Other) {
			X *= Other.X;
			Y *= Other.Y;
			Z *= Other.Z;
			return *this;
		}

		FVector operator/=(const FVector& Other) {
			X /= Other.X;
			Y /= Other.Y;
			Z /= Other.Z;
			return *this;
		}

		FVector operator*(float Scale) const {
			return FVector(X * Scale, Y * Scale, Z * Scale);
		}

		FVector operator/(float Scale) const {
			const float RScale = 1.f / Scale;
			return FVector(X * RScale, Y * RScale, Z * RScale);
		}

		FVector operator+=(float Scale) {
			X += Scale;
			Y += Scale;
			Z += Scale;
			return *this;
		}

		FVector operator-=(float Scale) {
			X -= Scale;
			Y -= Scale;
			Z -= Scale;
			return *this;
		}

		FVector operator*=(float Scale) {
			X *= Scale;
			Y *= Scale;
			Z *= Scale;
			return *this;
		}

		FVector operator/=(float Scale) {
			const float RScale = 1.f / Scale;
			X *= RScale;
			Y *= RScale;
			Z *= RScale;
			return *this;
		}

		float Dot(const FVector& Other) const {
			return X * Other.X + Y * Other.Y + Z * Other.Z;
		}

		FVector Cross(const FVector& Other) const {
			return FVector(
				Y * Other.Z - Z * Other.Y,
				Z * Other.X - X * Other.Z,
				X * Other.Y - Y * Other.X
			);
		}
	};

	struct FVector2D
	{
	public:
		float X;
		float Y;

	public:
		inline FVector2D() : X(0.0f), Y(0.0f) { }
		inline FVector2D(float X, float Y) : X(X), Y(Y) { }

	public:
		FVector2D operator+(const FVector2D& Other) const {
			return FVector2D(X + Other.X, Y + Other.Y);
		}

		FVector2D operator-(const FVector2D& Other) const {
			return FVector2D(X - Other.X, Y - Other.Y);
		}

		FVector2D operator*(const FVector2D& Other) const {
			return FVector2D(X * Other.X, Y * Other.Y);
		}

		FVector2D operator/(const FVector2D& Other) const {
			return FVector2D(X / Other.X, Y / Other.Y);
		}

		bool operator==(const FVector2D& Other) const {
			return X == Other.X && Y == Other.Y;
		}

		bool operator!=(const FVector2D& Other) const {
			return X != Other.X || Y != Other.Y;
		}

		FVector2D operator+=(const FVector2D& Other) {
			X += Other.X;
			Y += Other.Y;
			return *this;
		}

		FVector2D operator-=(const FVector2D& Other) {
			X -= Other.X;
			Y -= Other.Y;
			return *this;
		}

		FVector2D operator*=(const FVector2D& Other) {
			X *= Other.X;
			Y *= Other.Y;
			return *this;
		}

		FVector2D operator/=(const FVector2D& Other) {
			X /= Other.X;
			Y /= Other.Y;
			return *this;
		}

		FVector2D operator*(float Scale) const {
			return FVector2D(X * Scale, Y * Scale);
		}

		FVector2D operator/(float Scale) const {
			const float RScale = 1.f / Scale;
			return FVector2D(X * RScale, Y * RScale);
		}

		FVector2D operator+=(float Scale) {
			X += Scale;
			Y += Scale;
			return *this;
		}

		FVector2D operator-=(float Scale) {
			X -= Scale;
			Y -= Scale;
			return *this;
		}

		FVector2D operator*=(float Scale) {
			X *= Scale;
			Y *= Scale;
			return *this;
		}

		FVector2D operator/=(float Scale) {
			const float RScale = 1.f / Scale;
			X *= RScale;
			Y *= RScale;
			return *this;
		}

		float Dot(const FVector2D& Other) const {
			return X * Other.X + Y * Other.Y;
		}

		float Cross(const FVector2D& Other) const {
			return X * Other.Y - Y * Other.X;
		}
	};

	struct FMatrix
	{
	public:
		float M[4][4];

	public:
		FMatrix operator*(const FMatrix& B) const {
#if 0
			FMatrix out;
			for (uint8_t r = 0; r < 4; r++) {
				for (uint8_t c = 0; c < 4; c++) {
					float sum = 0.f;

					for (uint8_t i = 0; i < 4; i++)
						sum += M[r][i] * m2.M[i][c];

					out.M[r][c] = sum;
				}
			}

			return out;
#else
			FMatrix result = {0}; // Initialize to zero

			__m128 rightRow[4];  // Rows from B matrix
			__m128 resultRow[4]; // Rows for result matrix

			// Load rows from B matrix
			for (int i = 0; i < 4; ++i) {
				rightRow[i] = _mm_loadu_ps(B.M[i]);
			}

			// Initialize result rows to zero
			__m128 zero = _mm_setzero_ps();
			for (int i = 0; i < 4; ++i) {
				resultRow[i] = zero;
			}

			// Perform multiplication and accumulation
			for (int i = 0; i < 4; ++i) {          // For each row in A
				for (int j = 0; j < 4; ++j) {      // For each column in A
					__m128 a = _mm_set1_ps(this->M[i][j]); // Broadcast A[i][j]
					resultRow[i] = _mm_add_ps(resultRow[i], _mm_mul_ps(a, rightRow[j]));
				}
				_mm_storeu_ps(result.M[i], resultRow[i]);
			}

			return result;
#endif
		}
	};

	struct FQuat
	{
	public:
		float X;
		float Y;
		float Z;
		float W;
	};

	struct FTransform
	{
	public:
		FQuat Rotation;
		FVector Translation;
		int _pad1;
		FVector Scale;
		int _pad2;

	public:
		FMatrix ToMatrixWithScale() const {
#if 0
			FMatrix m;
			m.M[3][0] = Translation.X;
			m.M[3][1] = Translation.Y;
			m.M[3][2] = Translation.Z;

			float x2 = Rotation.X + Rotation.X;
			float y2 = Rotation.Y + Rotation.Y;
			float z2 = Rotation.Z + Rotation.Z;

			float xx2 = Rotation.X * x2;
			float yy2 = Rotation.Y * y2;
			float zz2 = Rotation.Z * z2;
			m.M[0][0] = (1.f - (yy2 + zz2)) * Scale.X;
			m.M[1][1] = (1.f - (xx2 + zz2)) * Scale.Y;
			m.M[2][2] = (1.f - (xx2 + yy2)) * Scale.Z;

			float yz2 = Rotation.Y * z2;
			float wx2 = Rotation.W * x2;
			m.M[2][1] = (yz2 - wx2) * Scale.Z;
			m.M[1][2] = (yz2 + wx2) * Scale.Y;

			float xy2 = Rotation.X * y2;
			float wz2 = Rotation.W * z2;
			m.M[1][0] = (xy2 - wz2) * Scale.Y;
			m.M[0][1] = (xy2 + wz2) * Scale.X;

			float xz2 = Rotation.X * z2;
			float wy2 = Rotation.W * y2;
			m.M[2][0] = (xz2 + wy2) * Scale.Z;
			m.M[0][2] = (xz2 - wy2) * Scale.X;

			m.M[0][3] = 0.f;
			m.M[1][3] = 0.f;
			m.M[2][3] = 0.f;
			m.M[3][3] = 1.f;

			return m;
#else
			FMatrix m;

			// Translation part
			m.M[3][0] = Translation.X;
			m.M[3][1] = Translation.Y;
			m.M[3][2] = Translation.Z;
			m.M[3][3] = 1.0f;

			// Load quaternion components into vectors
			__m128 vec_X = _mm_set1_ps(Rotation.X);
			__m128 vec_Y = _mm_set1_ps(Rotation.Y);
			__m128 vec_Z = _mm_set1_ps(Rotation.Z);
			__m128 vec_W = _mm_set1_ps(Rotation.W);

			// Compute products needed for rotation matrix
			__m128 vec_XY = _mm_mul_ps(vec_X, vec_Y);
			__m128 vec_XZ = _mm_mul_ps(vec_X, vec_Z);
			__m128 vec_YZ = _mm_mul_ps(vec_Y, vec_Z);
			__m128 vec_WX = _mm_mul_ps(vec_W, vec_X);
			__m128 vec_WY = _mm_mul_ps(vec_W, vec_Y);
			__m128 vec_WZ = _mm_mul_ps(vec_W, vec_Z);

			// Compute squares for diagonal terms
			__m128 vec_XX = _mm_mul_ps(vec_X, vec_X);
			__m128 vec_YY = _mm_mul_ps(vec_Y, vec_Y);
			__m128 vec_ZZ = _mm_mul_ps(vec_Z, vec_Z);

			// Compute sums for diagonal terms and multiply by 2
			float yy2_plus_zz2 = 2.0f * _mm_cvtss_f32(_mm_add_ps(vec_YY, vec_ZZ));
			float xx2_plus_zz2 = 2.0f * _mm_cvtss_f32(_mm_add_ps(vec_XX, vec_ZZ));
			float xx2_plus_yy2 = 2.0f * _mm_cvtss_f32(_mm_add_ps(vec_XX, vec_YY));

			// Compute diagonal elements
			m.M[0][0] = (1.0f - yy2_plus_zz2) * Scale.X;
			m.M[1][1] = (1.0f - xx2_plus_zz2) * Scale.Y;
			m.M[2][2] = (1.0f - xx2_plus_yy2) * Scale.Z;

			// Compute off-diagonal terms using SIMD and apply scale per row
			// Row 0: Scale.X
			__m128 row0_b = _mm_mul_ps(_mm_add_ps(vec_XY, vec_WZ), _mm_set1_ps(2.0f * Scale.X));
			__m128 row0_c = _mm_mul_ps(_mm_sub_ps(vec_XZ, vec_WY), _mm_set1_ps(2.0f * Scale.X));
			m.M[0][1] = _mm_cvtss_f32(row0_b);
			m.M[0][2] = _mm_cvtss_f32(row0_c);

			// Row 1: Scale.Y
			__m128 row1_b = _mm_mul_ps(_mm_sub_ps(vec_XY, vec_WZ), _mm_set1_ps(2.0f * Scale.Y));
			__m128 row1_c = _mm_mul_ps(_mm_add_ps(vec_YZ, vec_WX), _mm_set1_ps(2.0f * Scale.Y));
			m.M[1][0] = _mm_cvtss_f32(row1_b);
			m.M[1][2] = _mm_cvtss_f32(row1_c);

			// Row 2: Scale.Z
			__m128 row2_b = _mm_mul_ps(_mm_add_ps(vec_XZ, vec_WY), _mm_set1_ps(2.0f * Scale.Z));
			__m128 row2_c = _mm_mul_ps(_mm_sub_ps(vec_YZ, vec_WX), _mm_set1_ps(2.0f * Scale.Z));
			m.M[2][0] = _mm_cvtss_f32(row2_b);
			m.M[2][1] = _mm_cvtss_f32(row2_c);

			// Set remaining elements
			m.M[0][3] = 0.0f;
			m.M[1][3] = 0.0f;
			m.M[2][3] = 0.0f;

			return m;
#endif
		}
	};

	struct FLinearColor
	{
	public:
		float R;
		float G;
		float B;
		float A;

	public:
		inline constexpr FLinearColor() : R(0), G(0), B(0), A(0) {}
		inline constexpr FLinearColor(float R, float G, float B, float A) : R(R), G(G), B(B), A(A) {}

	public:
		bool operator==(const FLinearColor& Other) const {
			return R == Other.R && G == Other.G && B == Other.B && A == Other.A;
		}
		bool operator!=(const FLinearColor& Other) const {
			return !(*this == Other);
		}

	public:
		std::string ToStr() const {
			return std::to_string((int)(R * 255.f)) + "/" +
				std::to_string((int)(G * 255.f)) + "/" +
				std::to_string((int)(B * 255.f)) + "/" +
				std::to_string((int)(A * 255.f));
		}

	public:
		static const FLinearColor White;
		static const FLinearColor Gray;
		static const FLinearColor Black;
		static const FLinearColor Red;
		static const FLinearColor Green;
		static const FLinearColor Blue;
	};
}
