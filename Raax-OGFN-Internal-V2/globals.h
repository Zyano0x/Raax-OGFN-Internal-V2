#pragma once

/*
 * Creates a new thread for the cheat. This is required if you are not manually mapping
 * the DLL since Windows API functions are heavily limited while the loader locked is being held.
 */
#define CFG_MAINTHREAD 1
#define CFG_UNLOADTHREAD 1
#define CFG_USELOGGING 0
#define CFG_SHOWCONSOLE 0
#define CFG_AVXOPTIMISATIONS 1
