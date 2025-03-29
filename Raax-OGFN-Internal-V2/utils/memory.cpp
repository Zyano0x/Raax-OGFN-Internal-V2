#include "memory.h"
#include <intrin.h>

PIMAGE_DOS_HEADER Memory::GetDosHeader(uintptr_t ModuleBase)
{
    const PIMAGE_DOS_HEADER DosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(ModuleBase);
    if (DosHeader->e_magic == IMAGE_DOS_SIGNATURE)
        return DosHeader;

    LOG(LOG_WARN, "Attempted to get DosHeader from invalid ModuleBase! (0x%p)", ModuleBase);
    return nullptr;
}

PIMAGE_NT_HEADERS64 Memory::GetNtHeaders(uintptr_t ModuleBase)
{
    const PIMAGE_DOS_HEADER DosHeader = GetDosHeader(ModuleBase);
    if (!DosHeader)
        return nullptr;

    const PIMAGE_NT_HEADERS NtHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>((reinterpret_cast<uintptr_t>(DosHeader) + DosHeader->e_lfanew));
    if (NtHeaders->Signature == IMAGE_NT_SIGNATURE)
        return NtHeaders;

    LOG(LOG_WARN, "Attempted to get NtHeaders from invalid ModuleBase! (0x%p)", ModuleBase);
    return nullptr;
}

PIMAGE_SECTION_HEADER Memory::GetSectionFromName(const char* Name, uintptr_t ModuleBase)
{
    const PIMAGE_NT_HEADERS NtHeaders = GetNtHeaders(ModuleBase);
    if (!NtHeaders)
        return nullptr;

    PIMAGE_SECTION_HEADER Sect = IMAGE_FIRST_SECTION(NtHeaders);
    for (int i = 0; i < NtHeaders->FileHeader.NumberOfSections; i++)
    {
        if (std::strncmp(reinterpret_cast<char*>(Sect->Name), Name, IMAGE_SIZEOF_SHORT_NAME) == 0)
            return Sect;
        Sect++;
    }

    LOG(LOG_WARN, "Failed to find section from name! (%s, 0x%p)", Name, ModuleBase);
    return nullptr;
}


uint64_t Memory::GetModuleSize(uintptr_t ModuleBase)
{
    const PIMAGE_NT_HEADERS NtHeaders = GetNtHeaders(ModuleBase);
    return NtHeaders ? NtHeaders->OptionalHeader.SizeOfImage : 0;
}

bool Memory::IsAddressInsideModule(uintptr_t Address, uintptr_t ModuleBase)
{
    return Address >= ModuleBase && Address <= ModuleBase + GetModuleSize(ModuleBase);
}


uintptr_t Memory::GetImageBase()
{
    // Same as NtCurrentPeb()->ImageBaseAddress
    return *(uintptr_t*)(__readgsqword(0x60) + 0x10);
}

uint64_t Memory::GetImageSize()
{
    return GetModuleSize(GetImageBase());
}

PIMAGE_SECTION_HEADER Memory::GetImageTextSection()
{
    return GetSectionFromName(".text", GetImageBase());
}
