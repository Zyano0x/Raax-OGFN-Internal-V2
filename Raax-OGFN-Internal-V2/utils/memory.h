#pragma once
#include <Windows.h>
#include <vector>
#include <utils/log.h>

namespace Memory
{
    /*
    * @brief Returns valid DosHeader from ModuleBase.
    * Logs a warning on failure.
    */
    PIMAGE_DOS_HEADER GetDosHeader(uintptr_t ModuleBase);
    /*
    * @brief Returns valid NtHeaders from ModuleBase.
    * Logs a warning on failure.
    */
    PIMAGE_NT_HEADERS64 GetNtHeaders(uintptr_t ModuleBase);
    /* 
    * @brief Returns first section matching name (Name chopped to IMAGE_SIZEOF_SHORT_NAME).
    * Logs a warning on failure.
    */
    PIMAGE_SECTION_HEADER GetSectionFromName(const char* Name, uintptr_t ModuleBase);

    /* @brief Returns a modules size from it's NT headers. */
    uint64_t GetModuleSize(uintptr_t ModuleBase);
    /* @brief Wrapper for basic range check. */
    bool IsAddressInsideModule(uintptr_t Address, uintptr_t ModuleBase);

    /* @brief Returns the main executable image base address. */
    uintptr_t GetImageBase();
    /* @brief Returns the main executable image size. */
    uint64_t GetImageSize();
    /* @Brief Returns first ".text" section in the main executable image. */
    PIMAGE_SECTION_HEADER GetImageTextSection();

    /**
    * @brief Scans memory range for a byte pattern.
    *
    * @tparam OperandSize - The size of the operand used for relative address calculations.
    * @param StartAddress - The starting address for the scan.
    * @param ScanSize - The size of the memory region to scan.
    * @param Pattern - The pattern to search for (supports wildcard '?').
    * @param Offset - The offset to apply when computing the final address (default: -1).
    * @param RelativeAddress - If true, computes a relative address based on OperandSize.
    * @param Backwards - Whether to scan backwards or forwards. This means it will move ScanSize backwards from StartAddress instead of forwards.
    * @return The address of the first match found, or 0 if no match is found.
    */
    template<typename OperandSize>
    inline uintptr_t PatternScanRange(uintptr_t StartAddress, uint64_t ScanSize, const char* Pattern, bool Backwards = false, int Offset = -1, bool RelativeAddress = false)
    {
        static auto PatternToByte = [](const char* Pattern)
            {
                std::vector<int> Bytes;
                const auto PatternStart = const_cast<char*>(Pattern);
                const auto PatternEnd = const_cast<char*>(Pattern) + strlen(Pattern);

                for (auto Current = PatternStart; Current < PatternEnd; ++Current)
                {
                    if (*Current == '?')
                    {
                        ++Current;

                        // Account for if 2 question marks were used in a row
                        // People generally use "?" and "??" interchangeably in patterns
                        if (*Current == '?')
                            ++Current;

                        Bytes.push_back(-1);
                    }
                    else
                    {
                        Bytes.push_back(strtoul((const char*)Current, &Current, 16));
                    }
                }

                return Bytes;
            };

        std::vector<int> Bytes = PatternToByte(Pattern);
        const size_t PatternLength = Bytes.size();
        const uint8_t* ScanBytes = reinterpret_cast<uint8_t*>(StartAddress);

        auto CheckPattern = [&](const uint8_t* Address)
            {
                bool Found = true;

                for (size_t j = 0; j < PatternLength; j++)
                {
                    if (Address[j] != Bytes[j] && Bytes[j] != -1)
                    {
                        Found = false;
                        break;
                    }
                }

                if (Found)
                {
                    uintptr_t uAddress = (uintptr_t)Address;

                    if (RelativeAddress)
                    {
                        if (Offset == -1)
                            Offset = PatternLength;

                        uAddress = ((uAddress + Offset + sizeof(OperandSize)) + *(OperandSize*)(uAddress + Offset));
                    }

                    return uAddress;
                }

                return uintptr_t();
            };

        if (Backwards)
        {
            for (uintptr_t i = PatternLength; i < ScanSize; i++)
            {
                uint8_t* Address = (uint8_t*)(ScanBytes - i);
                uintptr_t Result = CheckPattern(Address);
                if (Result)
                    return Result;
            }
        }
        else
        {
            for (uintptr_t i = 0; i < ScanSize - PatternLength; i++)
            {
                uint8_t* Address = (uint8_t*)(ScanBytes + i);
                uintptr_t Result = CheckPattern(Address);
                if (Result)
                    return Result;
            }
        }

        LOG(LOG_WARN, "Failed to find pattern! (0x%p, 0x%p, %s)", StartAddress, ScanSize, Pattern);
        return 0;
    }

    /**
    * @brief Scans the main executable image for a byte pattern.
    * This function is a convenience wrapper around PatternScan, automatically scanning the main executable image.
    */
    template<typename OperandSize>
    inline uintptr_t PatternScan(const char* Pattern, int Offset = -1, bool RelativeAddress = false)
    {
        return PatternScanRange<OperandSize>(GetImageBase(), GetImageSize(), Pattern, false, Offset, RelativeAddress);
    }

    /**
     * @brief Finds first instruction that references a string using a LEA instruction.
     * @tparam StringType - Must be `const char*` or `const wchar_t*`.
     * @param RefString - Target string to search for.
     * @return The address of the found instruction.
     */
    template<typename StringType = const char*>
    inline uintptr_t FindStringRef(StringType RefString)
    {
        constexpr bool IsChar = std::is_same_v<StringType, const char*>;
        constexpr bool IsWChar = std::is_same_v<StringType, const wchar_t*>;
        static_assert(IsChar || IsWChar, "FindString only supports `const char*` and `const wchar_t*`!");

        const PIMAGE_SECTION_HEADER TextSection = GetImageTextSection();
        if (!TextSection)
            return 0;

        // Give head room of 7 to ensure no out of bounds issues ever occur.
        uint8_t* TextSectionData = reinterpret_cast<uint8_t*>(GetImageBase() + TextSection->VirtualAddress);
        for (size_t i = 0; i < TextSection->Misc.VirtualSize - 7; i++)
        {
            // Check if the instruction is a LEA REG REL32_IMM.
            if ((TextSectionData[i] == 0x4C || TextSectionData[i] == 0x48) && TextSectionData[i+1] == 0x8D)
            {
                // Convert REL32_IMM to absolute address.
                const void* FoundStr = *(int32_t*)(TextSectionData + i + 3) + 7 + TextSectionData + i;
                if (IsAddressInsideModule(reinterpret_cast<uintptr_t>(FoundStr), GetImageBase()))
                {
                    if constexpr (IsChar)
                    {
                        if (strcmp(RefString, static_cast<const char*>(FoundStr)) == 0)
                            return reinterpret_cast<uintptr_t>(TextSectionData + i);
                    }
                    else if constexpr (IsWChar)
                    {
                        if (wcscmp(RefString, static_cast<const wchar_t*>(FoundStr)) == 0)
                            return reinterpret_cast<uintptr_t>(TextSectionData + i);
                    }
                }

                i += 7;
            }
        }

        if constexpr (IsChar)
            LOG(LOG_WARN, "Failed to find string! (%s)", RefString);
        else if constexpr (IsWChar)
            LOG(LOG_WARN, "Failed to find string! (%S)", RefString);

        return 0;
    }
}
