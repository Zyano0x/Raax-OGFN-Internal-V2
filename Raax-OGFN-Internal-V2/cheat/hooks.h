#pragma once

/*
 * Hooks are abstracted in this way to allow you to add your own implementation of hooking
 * since MinHook's hooking method is fairly detected.
 */
namespace Hooks {

// --- Initialization ------------------------------------------------

bool Init();
void Destroy();

// --- Hook Management -----------------------------------------------

bool CreateHook(void* target, void* detour, void** original);
bool EnableHook(void* target);
bool RemoveHook(void* target);
bool DisableHook(void* target);

} // namespace Hooks
