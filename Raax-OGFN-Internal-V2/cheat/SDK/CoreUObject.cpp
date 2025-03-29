#include "CoreUObject.h"

namespace SDK
{
    std::string UObject::GetName() const
    {
        return this ? Name.ToString() : "None";
    }

    std::wstring UObject::GetWName() const
    {
        return this ? Name.ToWString() : L"None";
    }

    std::string UObject::GetFullName() const
    {
        if (Class)
        {
            std::string Temp;
            for (UObject* NextOuter = Outer; NextOuter; NextOuter = NextOuter->Outer)
            {
                Temp = NextOuter->GetName();
            }
            
            return Class->GetName() + " " + Temp + GetName();
        }

        return "None";
    }

    std::wstring UObject::GetFullWName() const
    {
        if (Class)
        {
            std::wstring Temp;
            for (UObject* NextOuter = Outer; NextOuter; NextOuter = NextOuter->Outer)
            {
                Temp = NextOuter->GetWName();
            }

            return Class->GetWName() + L" " + Temp + GetWName();
        }

        return L"None";
    }
}
