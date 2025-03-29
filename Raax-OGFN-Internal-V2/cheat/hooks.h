#pragma once

/*
* Hooks are abstracted in this way to allow you to add your own implementation of hooking
* since MinHook's hooking method is fairly detected.
*/
namespace Hooks
{
    bool Init();
    void Destroy();

    bool CreateHook(void* Target, void* Detour, void** Original);
    bool EnableHook(void* Target);
    bool RemoveHook(void* Target);
    bool DisableHook(void* Target);
}
