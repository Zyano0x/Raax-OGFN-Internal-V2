#pragma once

/*
* Creates a new thread for the cheat. This is required if you are not manually mapping
* the DLL since Windows API functions are heavily limited while the loader locked is being held.
*/
#define CFG_CREATETHREAD 1
#define CFG_USELOGGING _DEBUG
#define CFG_SHOWCONSOLE _DEBUG
