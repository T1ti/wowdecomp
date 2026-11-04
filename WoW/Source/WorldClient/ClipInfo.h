#ifndef WOW_SOURCE_WORLDCLIENT_CLIPINFO_H
#define WOW_SOURCE_WORLDCLIENT_CLIPINFO_H

#include <cstddef>
#include <cstdint>

class C3Vector;

class ClipInfo {
    public:
    /* 0000 */ float bc[6]; // Boundary coordinates: x, 1-x, y, 1-y, z, 1-z for the normalized [0,1] clip volume.
    /* 0018 */ uint32_t mask; // Boundary-coordinate sign bits in bits 31 through 26, in the same plane order.
    /* 001C */ uint32_t filler; // Trailing word of the 32-byte record; Set does not write it.

    void Set(const C3Vector& ndc);
};

struct ClipFrame {
    /* 0000 */ C3Vector** points; // Parallel arrays of vertex pointers and their boundary coordinates/masks.
    /* 0004 */ ClipInfo** info;
    /* 0008 */ uint32_t count;
};

#endif
