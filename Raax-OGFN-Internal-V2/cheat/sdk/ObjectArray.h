#pragma once
#include <cstdint>

namespace SDK {

struct FUObjectItem {
    class UObject* Object;
    uint8_t        Pad_0[0x10];
};

class Chunked_TUObjectArray {
  public:
    enum {
        ElementsPerChunk = 0x10000,
    };

  public:
    static inline auto DecryptPtr = [](void* ObjPtr) -> uint8_t* { return reinterpret_cast<uint8_t*>(ObjPtr); };

    FUObjectItem** Objects;
    uint8_t        Pad_0[0x08];
    int32_t        MaxElements;
    int32_t        NumElements;
    int32_t        MaxChunks;
    int32_t        NumChunks;

  public:
    inline int32_t        Num() const { return NumElements; }
    inline FUObjectItem** GetDecrytedObjPtr() const { return reinterpret_cast<FUObjectItem**>(DecryptPtr(Objects)); }
    inline class UObject* GetByIndex(const int32_t Index) const {
        if (Index < 0 || Index > NumElements)
            return nullptr;

        const int32_t ChunkIndex = Index / ElementsPerChunk;
        const int32_t InChunkIdx = Index % ElementsPerChunk;

        return GetDecrytedObjPtr()[ChunkIndex][InChunkIdx].Object;
    }
};
class Fixed_TUObjectArray {
  public:
    static inline auto DecryptPtr = [](void* ObjPtr) -> uint8_t* { return reinterpret_cast<uint8_t*>(ObjPtr); };

  public:
    FUObjectItem* Objects;
    int32_t       MaxElements;
    int32_t       NumElements;

  public:
    inline int            Num() const { return NumElements; }
    inline FUObjectItem*  GetDecrytedObjPtr() const { return reinterpret_cast<FUObjectItem*>(DecryptPtr(Objects)); }
    inline class UObject* GetByIndex(const int32_t Index) const {
        if (Index < 0 || Index > NumElements)
            return nullptr;

        return GetDecrytedObjPtr()[Index].Object;
    }
};

class TUObjectArray {
  private:
    void* m_ObjectArray;
    bool  m_IsChunked;

  public:
    TUObjectArray() : m_ObjectArray(nullptr), m_IsChunked(false) {}

  public:
    inline void Initialize(void* Address, bool IsChunked) {
        m_ObjectArray = Address;
        m_IsChunked = IsChunked;
    }
    inline int32_t Num() const {
        return m_IsChunked ? static_cast<Chunked_TUObjectArray*>(m_ObjectArray)->Num()
                           : static_cast<Fixed_TUObjectArray*>(m_ObjectArray)->Num();
    }
    inline class UObject* GetByIndex(int32_t Index) const {
        return m_IsChunked ? static_cast<Chunked_TUObjectArray*>(m_ObjectArray)->GetByIndex(Index)
                           : static_cast<Fixed_TUObjectArray*>(m_ObjectArray)->GetByIndex(Index);
    }
};

} // namespace SDK
