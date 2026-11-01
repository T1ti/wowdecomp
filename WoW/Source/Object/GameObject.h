#ifndef WOW_SOURCE_OBJECT_GAMEOBJECT_H
#define WOW_SOURCE_OBJECT_GAMEOBJECT_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

class CGGameObjectData;

class CGGameObject {
    public:
    static const int32_t NUM_MIRROR_FIELDS = 4;
    static uint32_t s_mirrorFieldIndex[NUM_MIRROR_FIELDS];

    static uint32_t DescriptorToMirrorOffset(uint32_t offset, uint32_t size, int32_t local, uint32_t base);
    static uint32_t GetSavedIndexFromDataIndex(uint32_t fieldIndex, uint32_t unused_count, int32_t unused_local);
    static int32_t TotalFieldsSaved();

    /* 0000 */ CGGameObjectData* m_gameObject;
    /* 0004 */ void* m_gameObjectMirror;
};

class CGGameObjectData {
    public:
    static const int32_t NUM_FIELDS = 12;
    /* 0000 */ WOWGUID m_createdBy;
    /* 0008 */ uint32_t m_displayID;
    /* 000C */ uint32_t m_flags;
    /* 0010 */ float m_parentRotation[4];
    /* 0020 */ uint16_t m_dynamicFlags;
    /* 0022 */ uint16_t m_pathProgress;
    /* 0024 */ uint32_t m_faction;
    /* 0028 */ uint32_t m_level;
    /* 002C */ int8_t m_state;
    /* 002D */ int8_t m_typeID;
    /* 002E */ int8_t m_artKit;
    /* 002F */ uint8_t m_animProgress;
};

#endif
