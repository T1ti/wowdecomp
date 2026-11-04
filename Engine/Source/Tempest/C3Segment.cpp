#include "Engine/Source/Tempest/C3Segment.h"

// VERIFY: 92f6ed53 auto
// OFFSET: 0x0078F4D0
C3Vector C3Segment::Lerp(float t) const {
    float y = (this->end.y - this->start.y) * t;
    float z = (this->end.z - this->start.z) * t;
    return C3Vector((this->end.x - this->start.x) * t + this->start.x, y + this->start.y, z + this->start.z);
}
