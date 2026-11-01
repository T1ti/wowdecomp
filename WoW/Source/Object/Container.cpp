#include "Container.h"
#include "WoW/Source/Object/Item.h"
#include "WoW/Source/Object/Object.h"

// STATIC: 0x00AC48B0
const uint32_t CGContainer::s_mirrorFields[CGContainer::NUM_MIRROR_FIELDS] = {
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 1,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 2,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 3,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 4,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 5,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 6,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 7,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 8,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 9,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 10,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 11,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 12,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 13,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 14,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 15,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 16,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 17,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 18,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 19,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 20,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 21,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 22,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 23,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 24,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 25,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 26,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 27,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 28,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 29,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 30,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 31,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 32,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 33,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 34,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 35,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 36,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 37,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 38,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 39,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 40,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 41,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 42,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 43,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 44,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 45,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 46,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 47,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 48,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 49,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 50,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 51,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 52,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 53,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 54,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 55,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 56,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 57,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 58,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 59,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 60,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 61,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 62,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 63,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 64,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 65,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 66,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 67,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 68,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 69,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 70,
    CGContainerData::FIELD_ID::CONTAINER_FIELD_SLOT_1 + 71,
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
