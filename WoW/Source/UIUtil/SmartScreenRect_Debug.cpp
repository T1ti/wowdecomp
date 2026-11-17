#include "WoW/Source/DebugFeatures.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/Types.h"

#ifdef WOWDECOMP_DEBUG_FEATURES

static CVar* s_cvShowSmartRects;

// Debug feature: smart screen rect grid display.
// 0.5.3 reference: SmartScreenRectInitialize (0x0052EA70).
// 0.5.3 registered the showsmartrects CVar here; 3.3.5 has SmartScreenRectShutdown but no initializer.
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
void SmartScreenRectInitialize() {
    s_cvShowSmartRects = CVar::Register("showsmartrects", nullptr, CVAR_FLAGS::CVAR_NONE, "0", nullptr, CATEGORY_DEBUG, false, nullptr, false);
}

#endif
