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
        void** VTable;
        int32_t Flags;
        int32_t Index;
        class UClass* Class;
        class FName Name;
        UObject* Outer;

    public:
        void ProcessEvent(void* Function, void* Parameters);

        bool IsA(class UClass* Class) const;
        bool IsDefaultObject() const;

        //bool HasTypeFlag(EClassCastFlags TypeFlag) const;

        std::string GetName() const;
        std::wstring GetWName() const;
        std::string GetFullName() const;
        std::wstring GetFullWName() const;
    };

    class UClass : public UObject
    {
    public:
    };
}
