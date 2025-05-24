#pragma once

#include <string>
#include <type_traits>

#include "ObjectArray.h"
#include "Basic.h"
#include "FProperty.h"

/* @brief Sets up StaticClass() and GetDefaultObj() functions. */
#define STATICCLASS_DEFAULTOBJECT(ClassNameStr, Type)                                                                 \
    static constexpr const char* ClassName = ClassNameStr;                                                            \
    static UClass* StaticClass() {                                                                                    \
        static UClass* Clss = nullptr;                                                                                \
        if (!Clss)                                                                                                    \
            Clss = UObject::FindObjectFast<UClass>(ClassNameStr, SDK::EClassCastFlags::Class);                        \
        return Clss;                                                                                                  \
    }                                                                                                                 \
    static Type* GetDefaultObj() {                                                                                    \
        static Type* Default = nullptr;                                                                               \
        if (!Default)                                                                                                 \
            Default = static_cast<Type*>(StaticClass()->ClassDefaultObject());                                        \
        return Default;                                                                                               \
    }

namespace SDK {

/* @brief Struct used for finding properties. */
struct PropertyInfo {
    bool    Found = false;
    int32_t Offset = 0;
    uint8_t ByteMask = 0;
    union {
        class UProperty* Prop;
        class FProperty* FProp;
    };
};

#define UPROPERTY(type, name)                                                                                          \
    static const PropertyInfo& getpropinfo##name(bool SuppressFailure = false) {                                       \
        static PropertyInfo Prop =                                                                                     \
            GetPropertyInfo(ClassName /* assumes STATICCLASS_DEFAULTOBJECT is used*/, #name, SuppressFailure);         \
        return Prop;                                                                                                   \
    }                                                                                                                  \
    void putprop_##name(const type& v) {                                                                               \
        const PropertyInfo& Prop = getpropinfo##name();                                                                \
        *reinterpret_cast<type*>((uint8_t*)this + Prop.Offset) = const_cast<type&>(v);                                 \
    }                                                                                                                  \
    type& getprop_##name() const {                                                                                     \
        const PropertyInfo& Prop = getpropinfo##name();                                                                \
        return *reinterpret_cast<type*>((uint8_t*)this + Prop.Offset);                                                 \
    }                                                                                                                  \
    __declspec(property(get = getprop_##name, put = putprop_##name)) type name


#define UPROPERTY_BITFIELD(type, name)                                                                                 \
    static_assert(std::is_same_v<type, bool>);                                                                         \
    static const PropertyInfo& getpropinfo##name(bool SuppressFailure = false) {                                       \
        static PropertyInfo Prop =                                                                                     \
            GetPropertyInfo(ClassName /* assumes STATICCLASS_DEFAULTOBJECT is used*/, #name, SuppressFailure);         \
        return Prop;                                                                                                   \
    }                                                                                                                  \
    void putprop_##name(const type& v) {                                                                               \
        const PropertyInfo& Prop = getpropinfo##name();                                                                \
        if (Prop.ByteMask) {                                                                                           \
            auto& ByteValue = *reinterpret_cast<uint8_t*>((uint8_t*)this + Prop.Offset);                               \
            ByteValue &= ~Prop.ByteMask;                                                                               \
            if (v) {                                                                                                   \
                ByteValue |= Prop.ByteMask;                                                                            \
            }                                                                                                          \
            return;                                                                                                    \
        }                                                                                                              \
        *reinterpret_cast<type*>((uint8_t*)this + Prop.Offset) = const_cast<type&>(v);                                 \
    }                                                                                                                  \
    bool getprop_##name() const {                                                                                      \
        const PropertyInfo& Prop = getpropinfo##name();                                                                \
        auto Value = *reinterpret_cast<uint8_t*>((uint8_t*)this + Prop.Offset);                                        \
        if (Prop.ByteMask)                                                                                             \
            return Value & Prop.ByteMask;                                                                              \
        return Value;                                                                                                  \
    }                                                                                                                  \
    __declspec(property(get = getprop_##name, put = putprop_##name)) type name


class UObject {
  public:
    static inline TUObjectArray Objects;

  public:
    static inline int ProcessEvent_Idx;

  public:
    void**        VTable;
    int32_t       Flags;
    int32_t       Index;
    class UClass* Class;
    class FName   Name;
    UObject*      Outer;

  public:
    void ProcessEvent(class UFunction* Function, void* Parms);

    bool IsA(class UClass* Class) const;
    bool IsDefaultObject() const;

    bool HasTypeFlag(EClassCastFlags TypeFlag) const;

    std::string  GetName() const;
    std::wstring GetWName() const;
    std::string  GetFullName() const;
    std::wstring GetFullWName() const;

  public:
    template <typename UEType = UObject>
    static UEType* FindObject(const std::string& FullName, EClassCastFlags RequiredType = EClassCastFlags::None) {
        for (int i = 0; i < Objects.Num(); ++i) {
            UObject* Object = Objects.GetByIndex(i);
            if (!Object)
                continue;

            if (Object->HasTypeFlag(RequiredType) && Object->GetFullName() == FullName)
                return static_cast<UEType*>(Object);
        }

        return nullptr;
    }

    template <typename UEType = UObject>
    static UEType* FindObjectFast(const FName& Name, EClassCastFlags RequiredType = EClassCastFlags::None) {
        for (int i = 0; i < Objects.Num(); ++i) {
            UObject* Object = Objects.GetByIndex(i);
            if (!Object)
                continue;

            if (Object->HasTypeFlag(RequiredType) && Object->Name == Name)
                return static_cast<UEType*>(Object);
        }

        return nullptr;
    }

    template <typename UEType = UObject>
    static UEType* FindObjectFastWithOuter(const FName& Name, const FName& OuterName) {
        for (int i = 0; i < Objects.Num(); ++i) {
            UObject* Object = Objects.GetByIndex(i);
            if (!Object || !Object->Outer)
                continue;

            if (Object->Name == Name && Object->Outer->Name == OuterName)
                return reinterpret_cast<UEType*>(Object);
        }

        return nullptr;
    }

    static PropertyInfo GetPropertyInfo(const FName& ClassName, const FName& PropertyName,
                                        bool SuppressFailure = false);
    static UFunction*   GetFunction(const FName& ClassName, const FName& FunctionName, bool SuppressFailure = false);
};

class UStruct : public UObject {
  public:
    static inline uint32_t SuperStruct_Offset;
    static inline uint32_t Children_Offset;
    static inline uint32_t ChildProperties_Offset;

  public:
    class UStruct* SuperStruct() const;
    class UField*  Children() const;
    class FField*  ChildProperties() const;

  public:
    PropertyInfo     FindProperty(const FName& Name) const;
    class UFunction* FindFunction(const FName& Name) const;
};

class UClass : public UStruct {
  public:
    static inline uint32_t ClassCastFlags_Offset;
    static inline uint32_t ClassDefaultObject_Offset;

  public:
    EClassCastFlags ClassCastFlags() const;
    UObject*        ClassDefaultObject() const;
};

class UField : public UObject {
  public:
    static inline uint32_t Next_Offset;

  public:
    class UField* Next() const;
};

class UProperty : public UField {
  public:
    static inline uint32_t Offset_Internal_Offset;

  public:
    int32_t Offset_Internal() const;
};

class UBoolProperty : public UProperty {
  public:
    static inline uint32_t ByteMask_Offset;

  public:
    uint8_t ByteMask() const;
    bool    IsNativeBool() const;
};

class UFunction : public UField {
  public:
    static inline uint32_t FunctionFlags_Offset;
    static inline uint32_t FuncPtr_Offset;

  public:
    EFunctionFlags FunctionFlags() const;
    void*          FuncPtr() const;
};

template <typename T, bool ForceCast = false> inline T* Cast(UObject* Object) {
    if (ForceCast || Object && Object->IsA(T::StaticClass()))
        return (T*)Object;
    return nullptr;
}

} // namespace SDK
