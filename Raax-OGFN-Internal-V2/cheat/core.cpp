#include "core.h"
#include <globals.h>
#include <cheat/sdk/sdk.h>
#include <utils/log.h>
#include <cheat/gui/gui.h>
#include <cheat/hooks.h>

bool Core::Init() {
#if CFG_USELOGGING
    InitLogger();
#endif
#if CFG_SHOWCONSOLE
    CreateConsole();
#endif
    return Hooks::Init() && SDK::Init() && GUI::Init();
}

void Core::Destroy() {
    GUI::Destroy();
    Hooks::Destroy();

#if CFG_SHOWCONSOLE
    DestroyConsole();
#endif
}
