#ifndef ENGINE_SOURCE_TEMPEST_C3SEGMENT_H
#define ENGINE_SOURCE_TEMPEST_C3SEGMENT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"

struct C3Segment {
    /* 0000 */ C3Vector start;
    /* 000C */ C3Vector end;

    C3Vector Lerp(float t) const;
};

#endif
