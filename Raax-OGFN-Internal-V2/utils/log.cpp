#include "log.h"
#include <Windows.h>
#include <string>
#include <sstream>
#include <ctime>

std::string GenerateTimeDateStamp() {
    std::time_t t = std::time(nullptr);
    std::tm tm_info;
    localtime_s(&tm_info, &t);

    // Format DD-MM-YYYY_HH:MN:SS
    std::stringstream ss;
    ss << (tm_info.tm_mday < 10 ? "0" : "") << tm_info.tm_mday << "-" <<
        (tm_info.tm_mon + 1 < 10 ? "0" : "") << (tm_info.tm_mon + 1) << "-" <<
        (tm_info.tm_year + 1900) << "_"
        << (tm_info.tm_hour < 10 ? "0" : "") << tm_info.tm_hour << "-" <<
        (tm_info.tm_min < 10 ? "0" : "") << tm_info.tm_min << "-" <<
        (tm_info.tm_sec < 10 ? "0" : "") << tm_info.tm_sec;

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
