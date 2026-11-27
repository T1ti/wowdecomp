#ifndef ENGINE_SOURCE_GX_CGXMATRIXSTACK_H
#define ENGINE_SOURCE_GX_CGXMATRIXSTACK_H

#include "Engine/Source/Storm/H/storm.h"
#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C44Matrix.h"

class CGxMatrixStack {
    public:
    enum EMatrixFlags {
        F_Identity = 0x1
    };

    /* 0000 */ uint32_t m_level;
    /* 0004 */ int8_t m_dirty;
    /* 0005 */ uint8_t unused_0005[3];
    /* 0008 */ C44Matrix m_mtx [4];
    /* 0108 */ uint32_t m_flags [4];

    CGxMatrixStack();
#if defined(WOWDECOMP_ASSERTIONS)
    // 12213 CGxDevice.h inline destructor; 3.3.5a has no destructor code.
    ~CGxMatrixStack() {
        STORM_ASSERT(m_level == 0);
    }
#endif
    void Identity();
};

#endif
