#pragma once
#include <cstdint>
#include <string>

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
}
