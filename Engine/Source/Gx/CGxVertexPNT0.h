#ifndef ENGINE_SOURCE_GX_CGXVERTEXPNT0_H
#define ENGINE_SOURCE_GX_CGXVERTEXPNT0_H

#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"

// GxVBF_PNT: 32-byte position/normal/one-UV records in locked vertex buffers.
// SSE skinning stores four floats at p and n, then overwrites tc[0].
class CGxVertexPNT0 {
    public:
    /* 0000 */ C3Vector p;
    /* 000C */ C3Vector n;
    /* 0018 */ C2Vector tc[1];
};

#endif
