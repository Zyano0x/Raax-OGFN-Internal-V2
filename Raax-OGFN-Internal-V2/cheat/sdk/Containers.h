#pragma once

#include <cstdint>
#include <string>

#include "FMemory.h"

namespace SDK {

template <typename ElementType> class TArray {
  protected:
    static constexpr uint64_t ElementAlign = alignof(ElementType);
    static constexpr uint64_t ElementSize = sizeof(ElementType);

  protected:
    ElementType* Data;
    int32_t      NumElements;
    int32_t      MaxElements;

  public:
    TArray() : Data(nullptr), NumElements(0), MaxElements(0) {}

    TArray(int32_t Size)
        : Data(static_cast<ElementType*>(FMemory::Malloc(Size * ElementSize, ElementAlign))), NumElements(0),
          MaxElements(Size) {}

    TArray(const TArray& Other) : Data(nullptr), NumElements(0), MaxElements(0) { CopyFrom(Other); }

    TArray(TArray&& Other) noexcept : Data(Other.Data), NumElements(Other.NumElements), MaxElements(Other.MaxElements) {
        Other.Data = nullptr;
        Other.NumElements = 0;
        Other.MaxElements = 0;
    }

    ~TArray() { Free(); }

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
    inline bool IsValid() const { return Data != nullptr; }

    inline bool IsValidIndex(int32_t Index) const { return Data && Index >= 0 && Index < NumElements; }

    inline int32_t Num() const { return NumElements; }

    inline int32_t Max() const { return MaxElements; }

    inline int32_t GetSlack() const { return MaxElements - NumElements; }

    inline ElementType* GetData() { return Data; }

    inline ElementType GetByIndex(int32_t Idx) { return Data[Idx]; }

  public:
    inline bool Add(const ElementType& Element) {
        if (GetSlack() <= 0)
            return false;

        Data[NumElements] = Element;
        NumElements++;

        return true;
    }

    inline bool Remove(int32_t Index) {
        if (!IsValidIndex(Index))
            return false;

        NumElements--;

        for (int i = Index; i < NumElements; i++) {
            /* NumElements was decremented, acessing i + 1 is safe */
            Data[i] = Data[i + 1];
        }

        return true;
    }

    inline void Clear() {
        NumElements = 0;

        if (Data)
            memset(Data, 0, NumElements * ElementSize);
    }

  public:
    inline void CopyFrom(const TArray& Other) {
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

    inline void Free() noexcept {
        if (Data)
            FMemory::Free(Data);

        NumElements = 0;
        MaxElements = 0;
    }

  public:
    inline ElementType&       operator[](int32_t Index) { return Data[Index]; }
    inline const ElementType& operator[](int32_t Index) const { return Data[Index]; }
};

class FString : public TArray<wchar_t> {
  public:
    FString() : TArray<wchar_t>() {}

    FString(const wchar_t* Str) {
        const uint32_t NullTerminatedLength = static_cast<uint32_t>(wcslen(Str) + 1);

        Data = static_cast<wchar_t*>(FMemory::Malloc(NullTerminatedLength, 0));
        NumElements = NullTerminatedLength;
        MaxElements = NullTerminatedLength;

        memcpy(Data, Str, NullTerminatedLength * sizeof(wchar_t));
    }

  public:
    inline std::wstring ToWString() const { return IsValid() ? Data : L""; }

    inline std::string ToString() const {
        if (IsValid()) {
            std::wstring WData = ToWString();
            return std::string(WData.begin(), WData.end());
        }
        return "";
    }

  public:
    inline wchar_t*       CStr() { return Data; }
    inline const wchar_t* CStr() const { return Data; }
};

} // namespace SDK
