#include "log.h"
#include <Windows.h>
#include <sstream>
#include <ctime>

namespace Log {

// --- Logging Functions & Data --------------------------------------

std::string GenerateTimeDateStamp() {
    std::time_t T = std::time(nullptr);
    std::tm     TMInfo;
    localtime_s(&TMInfo, &T);

    // Format DD-MM-YYYY_HH:MN:SS
    std::stringstream ss;
    ss << (TMInfo.tm_mday < 10 ? "0" : "") << TMInfo.tm_mday << "-" << (TMInfo.tm_mon + 1 < 10 ? "0" : "")
        << (TMInfo.tm_mon + 1) << "-" << (TMInfo.tm_year + 1900) << "_" << (TMInfo.tm_hour < 10 ? "0" : "")
        << TMInfo.tm_hour << "-" << (TMInfo.tm_min < 10 ? "0" : "") << TMInfo.tm_min << "-"
        << (TMInfo.tm_sec < 10 ? "0" : "") << TMInfo.tm_sec;

    return ss.str();
}

void SetThreadLogName(const std::string& Name) {
    loguru::set_thread_name(Name.c_str());
}

void InitLogger() {
    std::string LogFilePath = "raax_" + GenerateTimeDateStamp() + ".log";
    loguru::add_file(LogFilePath.c_str(), loguru::FileMode::Truncate, loguru::g_stderr_verbosity);
    SetThreadLogName("InitThread");
}

// --- Console Functions ---------------------------------------------

void CreateConsole() {
    AllocConsole();

    FILE* out;
    freopen_s(&out, "CONOUT$", "w", stdout);
    FILE* err;
    freopen_s(&err, "CONOUT$", "w", stderr);
}

void DestroyConsole() {
    fclose(stdout);
    fclose(stderr);
    FreeConsole();
}

} // namespace Log
