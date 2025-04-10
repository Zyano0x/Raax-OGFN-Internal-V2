#include "error.h"
#include <utils/log.h>
#include <Windows.h>

void Error::ThrowError(const std::string& Msg) {
    LOG(LOG_ERROR, "Error thrown: %s", Msg.c_str());
    MessageBoxA(0, Msg.c_str(), "Error", MB_ICONERROR);
}
