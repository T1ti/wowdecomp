#ifndef ENGINE_SOURCE_SCRN_TYPES_H
#define ENGINE_SOURCE_SCRN_TYPES_H

#include <cstdint>

enum SCRNSTOCK {
    STOCK_SYSFONT = 0,
    STOCK_PERFFONT = 1,
    SCRNSTOCKOBJECTS = 2,
};

// CILayer::m_flags, as used by the layer paint pass (TRANSPARENT after LoadingScreenSetTransparent).
enum SCRN_LAYER_FLAGS : uint32_t {
    SCRN_LAYER_TRANSPARENT = 0x1, // Does not clip the visible area of the layers below.
    SCRN_LAYER_ORTHO = 0x2, // Sets an orthographic view and projection over the visible rect.
    SCRN_LAYER_FULLSCREEN_VIEWPORT = 0x4, // Paints with the full-screen viewport.
};

struct HLAYER__ {
    /* 0000 */ int32_t m_unused;
};

typedef HLAYER__* HLAYER;

#endif
