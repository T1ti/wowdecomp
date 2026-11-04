#include "ClipInfo.h"
#include "Engine/Source/Tempest/C3Vector.h"

// VERIFY: caefdc42 auto
// OFFSET: 0x0078FFA0
void ClipInfo::Set(const C3Vector& ndc) {
    this->bc[0] = ndc.x;
    this->bc[1] = 1.0f - ndc.x;
    this->bc[2] = ndc.y;
    this->bc[3] = 1.0f - ndc.y;
    this->bc[4] = ndc.z;
    this->bc[5] = 1.0f - ndc.z;

    const uint32_t* bits = reinterpret_cast<const uint32_t*>(this->bc);

    this->mask = (bits[0] & 0x80000000) + ((bits[1] >> 1) & 0x40000000) + ((bits[2] >> 2) & 0x20000000) + ((bits[3] >> 3) & 0x10000000) + ((bits[4] >> 4) & 0x08000000) + ((bits[5] >> 5) & 0x04000000);
}
