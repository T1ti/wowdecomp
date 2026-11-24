#include <windows.h>
#include <cstdlib>
#include <float.h>
#include "Engine/Source/Storm/H/storm.h"
#include "WowEdit/Source/App/WowEditApp.h"

// The 3.3.5 client WinMain (WoW/Source/Win32 and Win64) supplies the pre-CRT Storm,
// floating-point and fiber stack setup used here.
extern "C" int WinMainCRTStartup();

extern "C" int start() {
    StormRtlInitialize();
    return WinMainCRTStartup();
}

struct EditorMainFiber {
    LPVOID m_caller;
    int m_result;
};

static void InvalidParameterHandler(const wchar_t* expression, const wchar_t* function, const wchar_t* file, unsigned int line, uintptr_t reserved) {
}

static void CALLBACK RunEditor(LPVOID parameter) {
    auto* fiber = static_cast<EditorMainFiber*>(parameter);
    _set_invalid_parameter_handler(InvalidParameterHandler);
    _clearfp();
#if defined(WOWDECOMP_ARCH_32)
    _control87(0x9001F, 0xFFFFFFFF);
#else
    _control87(0x9001F & ~(_MCW_PC | _MCW_IC), _MCW_DN | _MCW_EM | _MCW_RC);
#endif
    fiber->m_result = CWowEditApp::Run();
    SwitchToFiber(fiber->m_caller);
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previousInstance, LPSTR commandLine, int showCommand) {
    EditorMainFiber fiber = { ConvertThreadToFiber(nullptr), -1 };
    if (!fiber.m_caller) {
        return fiber.m_result;
    }

    LPVOID editorFiber = CreateFiberEx(0, 0x800000, 0, RunEditor, &fiber);
    if (editorFiber) {
        SwitchToFiber(editorFiber);
        DeleteFiber(editorFiber);
    }

    ConvertFiberToThread();
    return fiber.m_result;
}
