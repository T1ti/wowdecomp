#include "Container.h"
#include "WoW/Source/Object/Item.h"
#include "WoW/Source/Object/Object.h"

// STATIC: 0x00AC48B0
const uint32_t CGContainer::s_mirrorFields[CGContainer::NUM_MIRROR_FIELDS] = {
    0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF, 0x10, 0x11,
    0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21,
    0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31,
    0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41,
    0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49
};

// VERIFY: b88982c6 auto
// OFFSET: 0x004F5210
uint32_t CGContainer::GetSavedIndexFromDataIndex(uint32_t index, uint32_t unused_count, int32_t unused_local) {
    for (uint32_t i = 0; i < sizeof(CGContainer::s_mirrorFields) / sizeof(CGContainer::s_mirrorFields[0]); i++) {
        if (CGContainer::s_mirrorFields[i] == index) {
            return i;
        }
    }

    return CGContainerData::NUM_FIELDS;
}

// VERIFY: f16b2571 auto
// OFFSET: 0x004D4280
uint32_t CGContainer::DescriptorToMirrorOffset(uint32_t offset, uint32_t size, int32_t local, uint32_t base) {
    int32_t total = CGItem::TotalFieldsSaved();

    return (offset & 3) + 4 * (total + CGContainer::GetSavedIndexFromDataIndex((offset - base) >> 2, (size + 3) >> 2, local));
}

// VERIFY: cb8dbba0 auto
// OFFSET: 0x004F5200
int32_t CGContainer::TotalFieldsSaved() {
    return CGItem::TotalFieldsSaved() + sizeof(CGContainer::s_mirrorFields) / sizeof(CGContainer::s_mirrorFields[0]);
}
