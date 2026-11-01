#include "GameObject.h"
#include "WoW/Source/Object/Object.h"

// STATIC: 0x00AC5C1C
uint32_t CGGameObject::s_mirrorFieldIndex[CGGameObject::NUM_MIRROR_FIELDS] = {
    CGGameObjectData::FIELD_ID::GAMEOBJECT_FLAGS,
    CGGameObjectData::FIELD_ID::GAMEOBJECT_DYNAMIC,
    CGGameObjectData::FIELD_ID::GAMEOBJECT_BYTES_1,
    CGGameObjectData::FIELD_ID::GAMEOBJECT_END,
};

// VERIFY: 93fc90a3 auto
// OFFSET: 0x004D4300
uint32_t CGGameObject::DescriptorToMirrorOffset(uint32_t offset, uint32_t size, int32_t local, uint32_t base) {
    int32_t total = CGObject::TotalFieldsSaved();

    return (offset & 3) + 4 * (total + CGGameObject::GetSavedIndexFromDataIndex((offset - base) >> 2, (size + 3) >> 2, local));
}

// VERIFY: bd7b79e2 auto
// OFFSET: 0x004F55C0
uint32_t CGGameObject::GetSavedIndexFromDataIndex(uint32_t fieldIndex, uint32_t unused_count, int32_t unused_local) {
    for (uint32_t i = 0; i < CGGameObject::NUM_MIRROR_FIELDS; i++) {
        if (s_mirrorFieldIndex[i] == fieldIndex) {
            return i;
        }
    }

    return CGGameObjectData::NUM_FIELDS;
}

// VERIFY: 9a0cff15 auto
// OFFSET: 0x004F55B0
int32_t CGGameObject::TotalFieldsSaved() {
    return CGObject::TotalFieldsSaved() + sizeof(CGGameObject::s_mirrorFieldIndex) / sizeof(CGGameObject::s_mirrorFieldIndex[0]);
}
