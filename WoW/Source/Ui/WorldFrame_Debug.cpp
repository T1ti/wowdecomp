#include "WoW/Source/DebugFeatures.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "Engine/Source/ConsoleBase/ConsoleCommand.h"

#ifdef WOWDECOMP_DEBUG_FEATURES

// Debug feature: console command "SeeIfWorldFrameSucks".
// 0.5.3 reference: CheckFadeOutModels (0x004F1D30).
// Registered by the 0.5.3 CGWorldFrame constructor.
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
static int32_t CheckFadeOutModels(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: player render toggle.
// 0.5.3 reference: CGWorldFrame::TogglePlayerRender (0x004F3D90).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
int32_t CGWorldFrame::TogglePlayerRender() {
    return 0;
}

// Debug feature: registers the 0.5.3 CGWorldFrame constructor debug command and the smart rect CVar.
void WorldFrameDebugInitialize() {
    ConsoleCommandRegister("SeeIfWorldFrameSucks", CheckFadeOutModels, CATEGORY_DEBUG, nullptr);
    SmartScreenRectInitialize();
}

#endif
