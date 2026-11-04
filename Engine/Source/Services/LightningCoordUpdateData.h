#ifndef ENGINE_SOURCE_SERVICES_LIGHTNINGCOORDUPDATEDATA_H
#define ENGINE_SOURCE_SERVICES_LIGHTNINGCOORDUPDATEDATA_H

#include <cstdint>

class C3Vector;

struct LightningCoordUpdateData {
    /* 0000 */ void (__cdecl *callback)(void*, uint32_t, C3Vector*, C3Vector*); // 3.3.5 callback updates source/destination positions for a bolt index.
    /* 0004 */ void* context; // Opaque caller data passed as the callback's first argument.
};

#endif
