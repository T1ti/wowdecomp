#include <cstdint>
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Gxu/GxuDebugDraw.h"
#include "Engine/Source/Gxu/GxuFont.h"
#include "Engine/Source/Os/Path.h"
#include "Engine/Source/Scrn/Scrn.h"
#include "Engine/Source/Scrn/Stock.h"
#include "Engine/Source/Services/TextBlock.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x009EDCC8
static const float s_stockObjectHeights[SCRNSTOCKOBJECTS] = { 0.01953125f, 0.01953125f };

// STATIC: 0x00B49A28
static HTEXTFONT s_stockObjects[SCRNSTOCKOBJECTS];

// VERIFY: eb6fef3c auto
// OFFSET: 0x004A8D40
void ScrnSetStockFont(SCRNSTOCK stockID, const char* fontTexturePath) {
    if (s_stockObjects[stockID]) {
        HandleClose(reinterpret_cast<HOBJECT>(s_stockObjects[stockID]));
    }

    s_stockObjects[stockID] = TextBlockGenerateFont(fontTexturePath, 0, NDCToDDCHeight(s_stockObjectHeights[stockID]));
    STORM_ASSERT(s_stockObjects[stockID]);
}

// VERIFY: f91e1722 auto
// OFFSET: 0x004A8DC0
HTEXTFONT ScrnGetStockFont(SCRNSTOCK stockID) {
    if (stockID >= SCRNSTOCKOBJECTS) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);

        return nullptr;
    }

    return s_stockObjects[stockID];
}

// VERIFY: 6d724882 auto
// OFFSET: 0x004A8DE0
float ScrnGetStockFontHeight(SCRNSTOCK stockID) {
    if (stockID >= SCRNSTOCKOBJECTS) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);

        return 0.0f;
    }

    return s_stockObjectHeights[stockID];
}

// VERIFY: 972b66ba auto
// OFFSET: 0x004A8E00
void IStockInitialize() {
    GxuFontInitialize();

    char fontTexturePath[260];
    OsBuildFontFilePath("FRIZQT__.TTF", fontTexturePath, sizeof(fontTexturePath));

    if (fontTexturePath[0]) {
        ScrnSetStockFont(STOCK_SYSFONT, fontTexturePath);
    } else {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
    }

    if (fontTexturePath[0]) {
        ScrnSetStockFont(STOCK_PERFFONT, fontTexturePath);
    } else {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
    }
}

// OFFSET: 0x004A8D90
void IStockDestroy() {
    for (uint32_t i = 0; i < SCRNSTOCKOBJECTS; i++) {
        if (s_stockObjects[i]) {
            HandleClose(reinterpret_cast<HOBJECT>(s_stockObjects[i]));
        }

        s_stockObjects[i] = nullptr;
    }

#ifdef WOWDECOMP_GXU_DEBUG_DRAW
    // Debug/tool features: the debug-draw text drawer holds a font and strings, so it goes before GxuFontShutdown.
    GxuShutdownDebugDraw();
#endif

    GxuFontShutdown();
}
