#ifndef ENGINE_SOURCE_GX_CGXVERTEXPNCT2_H
#define ENGINE_SOURCE_GX_CGXVERTEXPNCT2_H

#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CImVector.h"

// GxVBF_PNCT2: position, normal, packed BGRA color and two UV sets, 44 bytes.
class CGxVertexPNCT2 {
    public:
    /* 0000 */ C3Vector p;
    /* 000C */ C3Vector n;
    /* 0018 */ CImVector c;
    /* 001C */ C2Vector tc[2];
};

#endif
