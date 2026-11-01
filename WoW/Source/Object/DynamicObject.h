#ifndef WOW_SOURCE_OBJECT_DYNAMICOBJECT_H
#define WOW_SOURCE_OBJECT_DYNAMICOBJECT_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

class CGDynamicObjectData;

class CGDynamicObject {
    public:
    static uint32_t DescriptorToMirrorOffset(uint32_t offset, uint32_t size, int32_t local, uint32_t base);
    static uint32_t GetSavedIndexFromDataIndex(uint32_t unused_index, uint32_t unused_count, int32_t unused_local);
    static int32_t TotalFieldsSaved();

    /* 0000 */ CGDynamicObjectData* m_dynamicObject;
    /* 0004 */ void* m_dynamicObjectMirror;
};

class CGDynamicObjectData {
    public:
    static const int32_t NUM_FIELDS = 6;
    /* 0000 */ WOWGUID m_caster;
    /* 0008 */ uint8_t m_bytes[4];
    /* 000C */ uint32_t m_spellID;
    /* 0010 */ float m_radius;
    /* 0014 */ uint32_t m_castTime;
};

#endif
