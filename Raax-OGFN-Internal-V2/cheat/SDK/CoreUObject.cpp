#include "CoreUObject.h"

namespace SDK
{
    void UObject::ProcessEvent(class UFunction* Function, void* Parms) {
        if (this) {
            void(*PE)(class UObject* Object, class UFunction* Function, void* Parms) = (decltype(PE))this->VTable[ProcessEvent_Idx];
            PE(this, Function, Parms);
        }
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

    class UStruct* UStruct::SuperStruct() {
        return this ? *(UStruct**)((uintptr_t)this + SuperStruct_Offset) : nullptr;
    }
    class UField* UStruct::Children() {
        return this ? *(UField**)((uintptr_t)this + Children_Offset) : nullptr;
    }
    //class FField* UStruct::ChildProperties() {
    //    return this ? *(FField**)((uintptr_t)this + ChildProperties_Offset) : nullptr;
    //}

    EClassCastFlags UClass::ClassCastFlags() {
        return this ? *(EClassCastFlags*)((uintptr_t)this + ClassCastFlags_Offset) : EClassCastFlags::None;
    }
    UObject* UClass::ClassDefaultObject() {
        return this ? *(UObject**)((uintptr_t)this + ClassDefaultObject_Offset) : nullptr;
    }

    class UField* UField::Next() {
        return this ? *(UField**)((uintptr_t)this + Next_Offset) : nullptr;
    }

    int32_t UProperty::Offset_Internal() {
        return this ? *(int32_t*)((uintptr_t)this + Offset_Internal_Offset) : 0;
    }

    EFunctionFlags UFunction::FunctionFlags() {
        return this ? *(EFunctionFlags*)((uintptr_t)this + FunctionFlags_Offset) : EFunctionFlags::None;
    }
}
