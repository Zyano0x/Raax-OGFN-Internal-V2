#pragma once

#include "Basic.h"

namespace SDK {

#ifdef _MSC_VER
#pragma pack(push, 0x1)
#endif
class FFieldClass {
  public:
    FName           Name;          // (0x00[0x08]) NOT AUTO-GENERATED PROPERTY
    uint64_t        Id;            // (0x08[0x08]) NOT AUTO-GENERATED PROPERTY
    EClassCastFlags CastFlags;     // (0x10[0x08]) NOT AUTO-GENERATED PROPERTY
    int32_t         ClassFlags;    // (0x18[0x04]) NOT AUTO-GENERATED PROPERTY
    uint8_t         Pad_74C3[0x4]; // Fixing Size After Last (Predefined) Property  [ Dumper-7 ]
    FFieldClass*    SuperClass;    // (0x20[0x08]) NOT AUTO-GENERATED PROPERTY
};
#ifdef _MSC_VER
#pragma pack(pop)
#endif

#ifdef _MSC_VER
#pragma pack(push, 0x1)
#endif
class FField {
  public:
    void*        Vft;         // (0x00[0x08]) NOT AUTO-GENERATED PROPERTY
    FFieldClass* Class;       // (0x08[0x08]) NOT AUTO-GENERATED PROPERTY
    char         Owner[0x10]; // (0x10[0x10]) NOT AUTO-GENERATED PROPERTY
    FField*      Next;        // (0x20[0x08]) NOT AUTO-GENERATED PROPERTY
    FName        Name;        // (0x28[0x08]) NOT AUTO-GENERATED PROPERTY
    int32_t      Flags;       // (0x30[0x04]) NOT AUTO-GENERATED PROPERTY

    bool HasTypeFlag(EClassCastFlags TypeFlag) const {
        return TypeFlag != EClassCastFlags::None ? Class->CastFlags & TypeFlag : true;
    }
};
#ifdef _MSC_VER
#pragma pack(pop)
#endif

#ifdef _MSC_VER
#pragma pack(push, 0x1)
#endif
class FProperty : public FField {
  public:
    uint8_t  Pad_74C4[0x8]; // Fixing Size After Last (Predefined) Property  [ Dumper-7 ]
    int32_t  ElementSize;   // (0x3C[0x04]) NOT AUTO-GENERATED PROPERTY
    uint64_t PropertyFlags; // (0x40[0x08]) NOT AUTO-GENERATED PROPERTY
    uint8_t  Pad_74C5[0x4]; // Fixing Size After Last (Predefined) Property  [ Dumper-7 ]
    int32_t  Offset;        // (0x4C[0x04]) NOT AUTO-GENERATED PROPERTY
};
#ifdef _MSC_VER
#pragma pack(pop)
#endif

#ifdef _MSC_VER
#pragma pack(push, 0x1)
#endif
class FBoolProperty : public FProperty {
  public:
    uint8_t Pad_74C7[0x28]; // Fixing Size After Last (Predefined) Property  [ Dumper-7 ]
    uint8_t FieldSize;      // (0x78[0x01]) NOT AUTO-GENERATED PROPERTY
    uint8_t ByteOffset;     // (0x79[0x01]) NOT AUTO-GENERATED PROPERTY
    uint8_t ByteMask;       // (0x7A[0x01]) NOT AUTO-GENERATED PROPERTY
    uint8_t FieldMask;      // (0x7B[0x01]) NOT AUTO-GENERATED PROPERTY
};
#ifdef _MSC_VER
#pragma pack(pop)
#endif

} // namespace SDK
