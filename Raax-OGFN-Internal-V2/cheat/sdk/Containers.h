#pragma once
#include <cstdint>
#include <string>
#include "FMemory.h"

namespace SDK
{
    template<typename ElementType>
    class TArray
    {
    protected:
        static constexpr uint64_t ElementAlign = alignof(ElementType);
        static constexpr uint64_t ElementSize = sizeof(ElementType);

    protected:
        ElementType* Data;
        int32_t NumElements;
        int32_t MaxElements;

    public:
        TArray()
            : Data(nullptr)
            , NumElements(0)
            , MaxElements(0)
        {
        }

        TArray(int32_t Size)
            : Data(static_cast<ElementType*>(FMemory::Malloc(Size * ElementSize, ElementAlign)))
            , NumElements(0)
            , MaxElements(Size)
        {
        }

        TArray(const TArray& Other)
            : Data(nullptr)
            , NumElements(0)
            , MaxElements(0)
        {
            CopyFrom(Other);
        }

        TArray(TArray&& Other) noexcept
            : Data(Other.Data)
            , NumElements(Other.NumElements)
            , MaxElements(Other.MaxElements)
        {
            Other.Data = nullptr;
            Other.NumElements = 0;
            Other.MaxElements = 0;
        }

        ~TArray()
        {
            Free();
        }

    public:
        TArray& operator=(TArray&& Other) noexcept {
            if (this == &Other)
                return *this;

            Free();

            Data = Other.Data;
            NumElements = Other.NumElements;
            MaxElements = Other.MaxElements;

            Other.Data = nullptr;
            Other.NumElements = 0;
            Other.MaxElements = 0;

            return *this;
        }

        TArray& operator=(const TArray& Other) {
            this->CopyFrom(Other);
            return *this;
        }

    public:
        inline bool IsValid() const {
            return Data != nullptr;
        }

        inline int32_t Num() const {
            return NumElements;
        }

        inline int32_t Max() const {
            return MaxElements;
        }

        inline int32_t GetSlack() const {
            return MaxElements - NumElements;
        }

        inline ElementType* GetData() {
            return Data;
        }

    public:
        inline void CopyFrom(const TArray& Other)
        {
            if (this == &Other || Other.NumElements == 0)
                return;

            NumElements = Other.NumElements;

            if (MaxElements >= Other.NumElements) {
                memcpy(Data, Other.Data, Other.NumElements);
                return;
            }

            Data = static_cast<ElementType*>(FMemory::Realloc(Data, Other.NumElements * ElementSize, ElementAlign));
            MaxElements = Other.NumElements;
            memcpy(Data, Other.Data, Other.NumElements * ElementSize);
        }

        inline void Free() noexcept
        {
            if (Data)
                FMemory::Free(Data);

            NumElements = 0;
            MaxElements = 0;
        }

    public:
        inline ElementType& operator[](int32_t Index) {
            return Data[Index];
        }
        inline const ElementType& operator[](int32_t Index) const {
            return Data[Index];
        }
    };

    class FString : public TArray<wchar_t>
    {
    public:
        inline std::wstring ToWString() const {
            return IsValid() ? Data : L"";
        }

        inline std::string ToString() const {
            if (IsValid()) {
                std::wstring WData = ToWString();
                return std::string(WData.begin(), WData.end());
            }
            return "";
        }
    };
}
