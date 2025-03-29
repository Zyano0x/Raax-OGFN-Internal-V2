#include "core.h"
#include <globals.h>
#include <cheat/SDK/sdk.h>
#include <utils/log.h>

bool Core::Init()
{
#if CFG_USELOGGING
    InitLogger();
#endif
#if CFG_SHOWCONSOLE
    CreateConsole();
#endif
    return SDK::Init();
}
