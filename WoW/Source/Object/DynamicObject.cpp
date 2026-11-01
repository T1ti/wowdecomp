#include "DynamicObject.h"
#include "WoW/Source/Object/Object.h"

// VERIFY: 1c11978c auto
// OFFSET: 0x004D4340
uint32_t CGDynamicObject::DescriptorToMirrorOffset(uint32_t offset, uint32_t size, int32_t local, uint32_t base) {
    int32_t total = CGObject::TotalFieldsSaved();

    return (offset & 3) + 4 * (total + CGDynamicObject::GetSavedIndexFromDataIndex((offset - base) >> 2, (size + 3) >> 2, local));
}

// VERIFY: dfd94fc0 auto
// OFFSET: 0x007F3B60
uint32_t CGDynamicObject::GetSavedIndexFromDataIndex(uint32_t unused_index, uint32_t unused_count, int32_t unused_local) {
    return CGDynamicObjectData::NUM_FIELDS;
}

// VERIFY: 5b8b167d auto
// OFFSET: 0x004F5670
int32_t CGDynamicObject::TotalFieldsSaved() {
    return CGObject::TotalFieldsSaved();
}
