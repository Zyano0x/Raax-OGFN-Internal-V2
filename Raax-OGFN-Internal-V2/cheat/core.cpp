#include "core.h"
#include <globals.h>
#include <cheat/sdk/sdk.h>
#include <utils/log.h>
#include <gui/gui.h>
#include <cheat/hooks.h>
#include <cheat/drawtransition.h>
#include <cheat/features/playerfeatures.h>

bool Core::Init() {
    LOG(LOG_TRACE, "Setting up core...");
#if CFG_USELOGGING
    InitLogger();
#endif
#if CFG_SHOWCONSOLE
    CreateConsole();
#endif
    return Hooks::Init() && SDK::Init() && GUI::Init() && DrawTransition::Init();
}

void Core::Destroy() {
    GUI::Destroy();
    DrawTransition::Destroy();
    Hooks::Destroy();

#if CFG_SHOWCONSOLE
    DestroyConsole();
#endif
}


void Core::TickGameThread() {
    ScreenSizeX = SDK::GetCanvas()->SizeX();
    ScreenSizeY = SDK::GetCanvas()->SizeY();
    Features::Player::TickGameThread();
}

void Core::TickRenderThread() {
    Features::Player::TickRenderThread();
}
