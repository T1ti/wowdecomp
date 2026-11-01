#include "Object.h"

// STATIC: 0x00AC47D8
const uint32_t CGObject::s_savedFields[CGObject::NUM_SAVED_FIELDS] = {
    OBJECT_FIELD_ENTRY,
    OBJECT_FIELD_SCALE_X,
    OBJECT_FIELD_PADDING,
};

// INLINED
CGObject::~CGObject() {
}

// VERIFY: 47fea3cc auto
// OFFSET: 0x004F4A10
int32_t CGObject::TotalFieldsSaved() {
    return sizeof(CGObject::s_savedFields) / sizeof(CGObject::s_savedFields[0]);
}

// VERIFY: bfa51ea9 auto
// OFFSET: 0x004F4A20
int32_t CGObject::GetSavedIndexFromDataIndex(uint32_t index, uint32_t unused_count, uint32_t unused_local) {
    for (int32_t i = 0; i < sizeof(CGObject::s_savedFields) / sizeof(CGObject::s_savedFields[0]); i++) {
        if (CGObject::s_savedFields[i] == index) {
            return i;
        }
    }

    return OBJECT_END;
}
