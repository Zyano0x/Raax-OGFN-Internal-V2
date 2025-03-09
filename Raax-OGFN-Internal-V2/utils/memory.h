#pragma once
#include <cstdint>

namespace Memory
{
    uintptr_t GetImageBase();
    uint64_t GetModuleSize(uintptr_t ModuleBase);
    bool IsAddressInsideModule(uintptr_t Address, uintptr_t ModuleBase);
}
