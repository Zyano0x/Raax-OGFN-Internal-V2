#pragma once
#include "raaxgui_impl.h"

namespace RaaxGUI {

// --- Public GUI Functions ------------------------------------------

void SetNextWindowSize(const Impl::Vec2& Size);
bool Begin(const char* Name, bool* pOpen = nullptr);
void End();

void Checkbox(const char* Name, bool* pValue);

// --- Public Tick Functions -----------------------------------------

void NewFrame();
void EndFrame();

} // namespace RaaxGUI
