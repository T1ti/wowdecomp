// WowEdit64.exe / WowEdit32.exe: the client executable run as the world editor (WowEdit/Source/App/WowEditApp.h).
//
// The executable is the client (WoW/Source/*.cpp, the same libraries) plus this file. Its only job is to mark the
// run as the editor before the client starts: a static initializer, run by the CRT start-up before WinMain, so no
// client source has to know about the editor. WowEditRegisterConsoleCommands then hooks the Gx window.

#include "WowEdit/Source/App/WowEditApp.h"

static bool s_wowEditEnabled = (WowEditAppEnable(), true);
