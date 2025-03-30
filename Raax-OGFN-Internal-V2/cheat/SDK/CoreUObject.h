#pragma once
#include <string>
#include "ObjectArray.h"
#include "Basic.h"

namespace SDK
{
    class UObject
    {
    public:
        static inline TUObjectArray Objects;

    public:
        static inline int ProcessEvent_Idx;

    public:
        void** VTable;
        int32_t Flags;
        int32_t Index;
        class UClass* Class;
        class FName Name;
        UObject* Outer;

    public:
        void ProcessEvent(class UFunction* Function, void* Parms);

        //bool IsA(class UClass* Class) const;
        bool IsDefaultObject() const;

        bool HasTypeFlag(EClassCastFlags TypeFlag) const;

        std::string GetName() const;
        std::wstring GetWName() const;
        std::string GetFullName() const;
        std::wstring GetFullWName() const;

    public:
        template<typename UEType = UObject>
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

        template<typename UEType = UObject>
        static UEType* FindObjectFast(const std::string& Name, EClassCastFlags RequiredType = EClassCastFlags::None) {
            FName TargetName = Name.c_str();

            for (int i = 0; i < Objects.Num(); ++i) {
                UObject* Object = Objects.GetByIndex(i);
                if (!Object)
                    continue;

                if (Object->HasTypeFlag(RequiredType) && Object->Name == TargetName)
                    return static_cast<UEType*>(Object);
            }

            return nullptr;
        }

        template<typename UEType = UObject>
        static UEType* FindObjectFastWithOuter(const std::string& Name, const std::string& OuterName) {
            FName TargetName = Name.c_str();
            FName TargetOuterName = OuterName.c_str();

            for (int i = 0; i < Objects.Num(); ++i) {
                UObject* Object = Objects.GetByIndex(i);
                if (!Object || !Object->Outer)
                    continue;

                if (Object->Name == TargetName && Object->Outer->Name == TargetOuterName)
                    return reinterpret_cast<UEType*>(Object);
            }

            return nullptr;
        }
    };

    class UStruct : public UObject
    {
    public:
        static inline uint32_t SuperStruct_Offset;
        static inline uint32_t Children_Offset;
        //static inline uint32_t ChildProperties_Offset;

    public:
        class UStruct* SuperStruct();
        class UField* Children();
        //class FField* ChildProperties();
    };

    class UClass : public UStruct
    {
    public:
        static inline uint32_t ClassCastFlags_Offset;
        static inline uint32_t ClassDefaultObject_Offset;

    public:
        EClassCastFlags ClassCastFlags();
        UObject* ClassDefaultObject();
    };

    class UField : public UObject {
    public:
        static inline uint32_t Next_Offset;

    public:
        class UField* Next();
    };

    class UProperty : public UField
    {
    public:
        static inline uint32_t Offset_Internal_Offset;

    public:
        int32_t Offset_Internal();
    };

    class UFunction : public UField
    {
    public:
        static inline uint32_t FunctionFlags_Offset;

    public:
        EFunctionFlags FunctionFlags();
    };
}
