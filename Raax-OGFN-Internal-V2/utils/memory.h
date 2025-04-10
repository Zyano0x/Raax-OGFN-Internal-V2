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
    /* @brief Wrapper for basic range check in the main executable image. */
    bool IsAddressInsideImage(uintptr_t Address);

    /**
     * @brief Scans memory range for a byte pattern.
     * 
     * @tparam OperandSize - The size of the operand used for relative address calculations.
     * @param StartAddress - The starting address for the scan.
     * @param ScanSize - The size of the memory region to scan.
     * @param Bytes - The raw bytes to scan for (use -1 for wildcard).
     * @param Backwards - Whether to scan backwards or forwards. This means it will move ScanSize backwards from StartAddress instead of forwards.
     * @param Offset - The offset to apply when computing the final address.
     * @param RelativeAddress - If true, computes a relative address based on OperandSize.
     * @param WarnIfNotFound - Log a warning message if the pattern is not found.
     * @return The address of the first match found, or 0 if no match is found.
     */
    template<typename OperandSize = int32_t>
    static uintptr_t PatternScanRangeBytes(uintptr_t StartAddress, uint64_t ScanSize, const std::vector<int>& Bytes, bool Backwards = false, int Offset = -1, bool RelativeAddress = false, bool WarnIfNotFound = true) {
        const size_t PatternLength = Bytes.size();
        const uint8_t* ScanBytes = reinterpret_cast<uint8_t*>(StartAddress);

        auto CheckPattern = [&](const uint8_t* Address) {
            bool Found = true;

            for (size_t j = 0; j < PatternLength; j++) {
                if (Address[j] != Bytes[j] && Bytes[j] != -1) {
                    Found = false;
                    break;
                }
            }

            if (Found) {
                uintptr_t uAddress = (uintptr_t)Address;

                if (RelativeAddress) {
                    if (Offset == -1)
                        Offset = PatternLength;

                    uAddress = ((uAddress + Offset + sizeof(OperandSize)) + *(OperandSize*)(uAddress + Offset));
                }

                return uAddress;
            }

            return uintptr_t();
            };

        if (Backwards) {
            for (uintptr_t i = PatternLength; i < ScanSize; i++) {
                uint8_t* Address = (uint8_t*)(ScanBytes - i);
                uintptr_t Result = CheckPattern(Address);
                if (Result)
                    return Result;
            }
        }
        else {
            for (uintptr_t i = 0; i < ScanSize - PatternLength; i++) {
                uint8_t* Address = (uint8_t*)(ScanBytes + i);
                uintptr_t Result = CheckPattern(Address);
                if (Result)
                    return Result;
            }
        }

        if (WarnIfNotFound)
            LOG(LOG_WARN, "Failed to find pattern! (0x%p, 0x%p)", StartAddress, ScanSize);

        return 0;
    }

    /**
    * @brief Scans memory range for a byte pattern using an IDA style signature.
    *
    * @tparam OperandSize - The size of the operand used for relative address calculations.
    * @param StartAddress - The starting address for the scan.
    * @param ScanSize - The size of the memory region to scan.
    * @param Pattern - The pattern to search for (supports wildcard '?').
    * @param Backwards - Whether to scan backwards or forwards. This means it will move ScanSize backwards from StartAddress instead of forwards.
    * @param Offset - The offset to apply when computing the final address.
    * @param RelativeAddress - If true, computes a relative address based on OperandSize.
    * @return The address of the first match found, or 0 if no match is found.
    */
    template<typename OperandSize = int32_t>
    static uintptr_t PatternScanRange(uintptr_t StartAddress, uint64_t ScanSize, const char* Pattern, bool Backwards = false, int Offset = -1, bool RelativeAddress = false) {
        static auto PatternToByte = [](const char* Pattern) {
                std::vector<int> Result;
                const auto PatternStart = const_cast<char*>(Pattern);
                const auto PatternEnd = const_cast<char*>(Pattern) + strlen(Pattern);

                for (auto Current = PatternStart; Current < PatternEnd; ++Current) {
                    if (*Current == '?') {
                        ++Current;

                        // Account for if 2 question marks were used in a row
                        // People generally use "?" and "??" interchangeably in patterns
                        if (*Current == '?')
                            ++Current;

                        Result.push_back(-1);
                    }
                    else {
                        Result.push_back(strtoul((const char*)Current, &Current, 16));
                    }
                }

                return Result;
            };

        std::vector<int> Bytes = PatternToByte(Pattern);
        return PatternScanRangeBytes<OperandSize>(StartAddress, ScanSize, Bytes, Backwards, Offset, RelativeAddress);
    }

    /**
    * @brief Scans the main executable image for a byte pattern.
    * This function is a convenience wrapper around PatternScan, automatically scanning the main executable image.
    */
    template<typename OperandSize = int32_t>
    static uintptr_t PatternScan(const char* Pattern, int Offset = -1, bool RelativeAddress = false) {
        return PatternScanRange<OperandSize>(GetImageBase(), GetImageSize(), Pattern, false, Offset, RelativeAddress);
    }

    /**
     * @brief Finds first instruction that references a string using a LEA instruction in the main executable image.
     * @tparam StringType - Must be `const char*` or `const wchar_t*`.
     * @param RefString - Target string to search for.
     * @return The address of the found instruction.
     */
    template<typename StringType = const char*>
    static uintptr_t FindStringRef(StringType RefString) {
        constexpr bool IsChar = std::is_same_v<StringType, const char*>;
        constexpr bool IsWChar = std::is_same_v<StringType, const wchar_t*>;
        static_assert(IsChar || IsWChar, "FindString only supports `const char*` and `const wchar_t*`!");

        const PIMAGE_SECTION_HEADER TextSection = GetImageTextSection();
        if (!TextSection)
            return 0;

        // Give head room of 7 to ensure no out of bounds issues ever occur.
        uint8_t* TextSectionData = reinterpret_cast<uint8_t*>(GetImageBase() + TextSection->VirtualAddress);
        for (size_t i = 0; i < TextSection->Misc.VirtualSize - 7; i++) {
            // Check if the instruction is a LEA REG REL32_IMM.
            if ((TextSectionData[i] == 0x4C || TextSectionData[i] == 0x48) && TextSectionData[i+1] == 0x8D) {
                // Convert REL32_IMM to absolute address.
                const void* FoundStr = *(int32_t*)(TextSectionData + i + 3) + 7 + TextSectionData + i;
                if (IsAddressInsideImage(reinterpret_cast<uintptr_t>(FoundStr))) {
                    if constexpr (IsChar) {
                        if (strcmp(RefString, static_cast<const char*>(FoundStr)) == 0)
                            return reinterpret_cast<uintptr_t>(TextSectionData + i);
                    }
                    else if constexpr (IsWChar) {
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

    /**
     * @brief Finds an offset where all objects and their value pairs match.
     * @tparam T - Value type for pairs.
     * @tparam Increment - Amount of bytes to increment from MinOffset until MaxOffset.
     * @param ObjectValuePairs - Pairs of objects to their expected values.
     * @param MinOffset - Offset to start searching from.
     * @param MaxOffset - Offset to stop search from. Scanning memory read bounds will end at MaxOffset + sizeof(T).
     * @return The offset at which all objects match their expected value, or 0 on failure.
     */
    template<typename T, int Increment = 4>
    static uint32_t FindMatchingValueOffset(const std::vector<std::pair<void*, T>>& ObjectValuePairs, uint32_t MinOffset = 0x28, uint32_t MaxOffset = 0x1A0) {
        uint32_t Offset = MinOffset;
        for (uint32_t Offset = MinOffset; Offset <= MaxOffset; Offset += Increment) {
            size_t MatchingCount = 0;
            for (const auto& Pair : ObjectValuePairs) {
                T* Value = reinterpret_cast<T*>((uintptr_t)Pair.first + Offset);
                if (*Value == Pair.second)
                    MatchingCount++;
            }

            if (MatchingCount == ObjectValuePairs.size())
                return Offset;
        }

        LOG(LOG_WARN, "Failed to find matching value offset!");
        return 0;
    }
}
