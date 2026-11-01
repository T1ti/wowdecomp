#include "Corpse.h"
#include "WoW/Source/Object/Object.h"

// STATIC: 0x00AC5C34
const uint32_t CGCorpse::s_mirrorFields[CGCorpse::NUM_MIRROR_FIELDS] = {
    CGCorpseData::FIELD_ID::CORPSE_FIELD_FLAGS,
    CGCorpseData::FIELD_ID::CORPSE_FIELD_DYNAMIC_FLAGS,
    CGCorpseData::FIELD_ID::CORPSE_END,
};

// VERIFY: ebadb458 auto
// OFFSET: 0x004D4380
uint32_t CGCorpse::DescriptorToMirrorOffset(uint32_t offset, uint32_t size, int32_t local, uint32_t base) {
    int32_t total = CGObject::TotalFieldsSaved();

    return (offset & 3) + 4 * (total + CGCorpse::GetSavedIndexFromDataIndex((offset - base) >> 2, (size + 3) >> 2, local));
}

// VERIFY: a1e13f02 auto
// OFFSET: 0x004F5680
int32_t CGCorpse::TotalFieldsSaved() {
    return CGObject::TotalFieldsSaved() + sizeof(CGCorpse::s_mirrorFields) / sizeof(CGCorpse::s_mirrorFields[0]);
}

// VERIFY: 04bd9804 auto
// OFFSET: 0x004F5690
uint32_t CGCorpse::GetSavedIndexFromDataIndex(uint32_t index, uint32_t unused_count, int32_t unused_local) {
    for (uint32_t i = 0; i < sizeof(CGCorpse::s_mirrorFields) / sizeof(CGCorpse::s_mirrorFields[0]); i++) {
        if (CGCorpse::s_mirrorFields[i] == index) {
            return i;
        }
    }

    return CGCorpseData::NUM_FIELDS;
}
