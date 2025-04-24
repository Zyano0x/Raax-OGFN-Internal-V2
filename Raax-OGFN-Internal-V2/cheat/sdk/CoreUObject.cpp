#include "CoreUObject.h"
#include "sdk.h"
#include <utils/log.h>

namespace SDK
{
    void UObject::ProcessEvent(class UFunction* Function, void* Parms) {
        void(*PE)(class UObject* Object, class UFunction* Function, void* Parms) = (decltype(PE))this->VTable[ProcessEvent_Idx];
        PE(this, Function, Parms);
    }

    bool UObject::IsA(class UClass* Clss) const {
        for (UStruct* Super = Class; Super; Super = Super->SuperStruct()) {
            if (Super == Clss)
                return true;
        }

        return false;
    }
    bool UObject::IsDefaultObject() const {
        return (Flags & 0x10) == 0x10;
    }

    bool UObject::HasTypeFlag(EClassCastFlags TypeFlag) const {
        return TypeFlag != EClassCastFlags::None ? Class->ClassCastFlags() & TypeFlag : true;
    }

    std::string UObject::GetName() const {
        return this ? Name.ToString() : "None";
    }
    std::wstring UObject::GetWName() const {
        return this ? Name.ToWString() : L"None";
    }
    std::string UObject::GetFullName() const {
        if (Class) {
            std::string Temp;
            for (UObject* NextOuter = Outer; NextOuter; NextOuter = NextOuter->Outer) {
                Temp = NextOuter->GetName();
            }
            
            return Class->GetName() + " " + Temp + GetName();
        }

        return "None";
    }
    std::wstring UObject::GetFullWName() const {
        if (Class) {
            std::wstring Temp;
            for (UObject* NextOuter = Outer; NextOuter; NextOuter = NextOuter->Outer) {
                Temp = NextOuter->GetWName();
            }

            return Class->GetWName() + L" " + Temp + GetWName();
        }

        return L"None";
    }

    PropertyInfo UObject::GetPropertyInfo(const FName& ClassName, const FName& PropertyName) {
        return FindObjectFast<UStruct>(ClassName, EClassCastFlags::Struct)->FindProperty(PropertyName);
    }
    UFunction* UObject::GetFunction(const FName& ClassName, const FName& FunctionName) {
        return FindObjectFast<UStruct>(ClassName, EClassCastFlags::Struct)->FindFunction(FunctionName);
    }

    class UStruct* UStruct::SuperStruct() const {
        return this ? *(UStruct**)((uintptr_t)this + SuperStruct_Offset) : nullptr;
    }
    class UField* UStruct::Children() const {
        return this ? *(UField**)((uintptr_t)this + Children_Offset) : nullptr;
    }
    class FField* UStruct::ChildProperties() const {
        return this ? *(FField**)((uintptr_t)this + ChildProperties_Offset) : nullptr;
    }

    PropertyInfo UStruct::FindProperty(const FName& Name) const {
        PropertyInfo Result = { .Found = false };

        if (SDK::EngineVersion < 4.25) {
            for (UField* Child = Children(); Child; Child = Child->Next()) {
                if (!Child->HasTypeFlag(EClassCastFlags::Property))
                    continue;

                UProperty* Property = static_cast<UProperty*>(Child);
                if (Property->Name == Name) {
                    Result.Found = true;
                    Result.Offset = Property->Offset_Internal();
                    Result.Prop = Property;

                    if (Property->HasTypeFlag(EClassCastFlags::BoolProperty)) {
                        UBoolProperty* BoolProperty = static_cast<UBoolProperty*>(Property);
                        if (!BoolProperty->IsNativeBool())
                            Result.ByteMask = BoolProperty->ByteMask();
                    }

                    return Result;
                }
            }
        }
        else {
            for (FField* ChildProperty = ChildProperties(); ChildProperty && ChildProperty->Class; ChildProperty = ChildProperty->Next) {
                if (ChildProperty->HasTypeFlag(EClassCastFlags::Property) && ChildProperty->Name == Name) {
                    FProperty* Property = static_cast<FProperty*>(ChildProperty);
                    Result.Found = true;
                    Result.Offset = Property->Offset;
                    Result.FProp = Property;

                    if (ChildProperty->HasTypeFlag(EClassCastFlags::BoolProperty)) {
                        FBoolProperty* BoolProperty = static_cast<FBoolProperty*>(ChildProperty);
                        if (BoolProperty->ByteMask != 0xFF)
                            Result.ByteMask = BoolProperty->ByteMask;
                    }

                    return Result;
                }
            }
        }

        LOG(LOG_WARN, "Failed to find property: %s", Name.ToString().c_str());
        return Result;
    }
    UFunction* UStruct::FindFunction(const FName& Name) const {
        for (UField* Child = Children(); Child; Child = Child->Next()) {
            if (!Child->HasTypeFlag(EClassCastFlags::Function))
                continue;

            UFunction* Function = reinterpret_cast<UFunction*>(Child);
            if (Function->Name == Name)
                return Function;
        }

        LOG(LOG_WARN, "Failed to find UFunction: %s", Name.ToString().c_str());
        return nullptr;
    }

    EClassCastFlags UClass::ClassCastFlags() const {
        return this ? *(EClassCastFlags*)((uintptr_t)this + ClassCastFlags_Offset) : EClassCastFlags::None;
    }
    UObject* UClass::ClassDefaultObject() const {
        return this ? *(UObject**)((uintptr_t)this + ClassDefaultObject_Offset) : nullptr;
    }

    class UField* UField::Next() const {
        return this ? *(UField**)((uintptr_t)this + Next_Offset) : nullptr;
    }

    int32_t UProperty::Offset_Internal() const {
        return this ? *(int32_t*)((uintptr_t)this + Offset_Internal_Offset) : 0;
    }

    uint8_t UBoolProperty::ByteMask() const {
        return this ? *(uint8_t*)((uintptr_t)this + ByteMask_Offset) : 0;
    }
    bool UBoolProperty::IsNativeBool() const {
        return ByteMask() == 0xFF;
    }

    EFunctionFlags UFunction::FunctionFlags() const {
        return this ? *(EFunctionFlags*)((uintptr_t)this + FunctionFlags_Offset) : EFunctionFlags::None;
    }
    void* UFunction::FuncPtr() const {
        return this ? *(void**)((uintptr_t)this + FuncPtr_Offset) : nullptr;
    }
}
