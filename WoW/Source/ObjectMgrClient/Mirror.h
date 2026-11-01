#ifndef WOW_SOURCE_OBJECTMGRCLIENT_MIRROR_H
#define WOW_SOURCE_OBJECTMGRCLIENT_MIRROR_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/Container.h"
#include "WoW/Source/Object/Corpse.h"
#include "WoW/Source/Object/DynamicObject.h"
#include "WoW/Source/Object/GameObject.h"
#include "WoW/Source/Object/Item.h"
#include "WoW/Source/Object/Object.h"
#include "WoW/Source/Object/Player.h"
#include "WoW/Source/Object/Unit.h"

static const int32_t NUM_OBJECT_FIELD_DESCRIPTORS = 5;
static const int32_t NUM_ITEM_FIELD_DESCRIPTORS = 38;
static const int32_t NUM_CONTAINER_FIELD_DESCRIPTORS = 3;
static const int32_t NUM_UNIT_FIELD_DESCRIPTORS = 89;
static const int32_t NUM_PLAYER_FIELD_DESCRIPTORS = 215;
static const int32_t NUM_GAMEOBJECT_FIELD_DESCRIPTORS = 8;
static const int32_t NUM_DYNAMICOBJECT_FIELD_DESCRIPTORS = 5;
static const int32_t NUM_CORPSE_FIELD_DESCRIPTORS = 10;

struct CGFieldDescriptor {
    /* 0000 */ const char* m_name;
    /* 0004 */ uint32_t m_offset;
    /* 0008 */ uint32_t m_count;
    /* 000C */ uint32_t m_type;
    /* 0010 */ uint32_t m_flags;
};

extern CGFieldDescriptor OBJECT_DISCRIPTORS[NUM_OBJECT_FIELD_DESCRIPTORS];
extern CGFieldDescriptor ITEM_DESCRIPTORS[NUM_ITEM_FIELD_DESCRIPTORS];
extern CGFieldDescriptor CONTAINER_DESCRIPTORS[NUM_CONTAINER_FIELD_DESCRIPTORS];
extern CGFieldDescriptor UNIT_DESCRIPTORS[NUM_UNIT_FIELD_DESCRIPTORS];
extern CGFieldDescriptor PLAYER_DESCRIPTORS[NUM_PLAYER_FIELD_DESCRIPTORS];
extern CGFieldDescriptor GAMEOBJECT_DESCRIPTORS[NUM_GAMEOBJECT_FIELD_DESCRIPTORS];
extern CGFieldDescriptor DYNAMICOBJECT_DESCRIPTORS[NUM_DYNAMICOBJECT_FIELD_DESCRIPTORS];
extern CGFieldDescriptor CORPSE_DESCRIPTORS[NUM_CORPSE_FIELD_DESCRIPTORS];
extern CGFieldDescriptor s_objectDescriptors[sizeof(CGObjectData) / sizeof(uint32_t)];
extern CGFieldDescriptor s_itemDescriptors[(sizeof(CGObjectData) + sizeof(CGItemData)) / sizeof(uint32_t)];
extern CGFieldDescriptor s_containerDescriptors[(sizeof(CGObjectData) + sizeof(CGItemData) + sizeof(CGContainerData)) / sizeof(uint32_t)];
extern CGFieldDescriptor s_unitDescriptors[(sizeof(CGObjectData) + sizeof(CGUnitData)) / sizeof(uint32_t)];
extern CGFieldDescriptor s_playerDescriptors[(sizeof(CGObjectData) + sizeof(CGUnitData) + sizeof(CGPlayerLocalData)) / sizeof(uint32_t)];
extern CGFieldDescriptor s_gameObjectDescriptors[(sizeof(CGObjectData) + sizeof(CGGameObjectData)) / sizeof(uint32_t)];
extern CGFieldDescriptor s_dynamicObjectDescriptors[(sizeof(CGObjectData) + sizeof(CGDynamicObjectData)) / sizeof(uint32_t)];
extern CGFieldDescriptor s_corpseDescriptors[(sizeof(CGObjectData) + sizeof(CGCorpseData)) / sizeof(uint32_t)];

void CopyAndExpandDescriptors(const CGFieldDescriptor* descriptors, CGFieldDescriptor* expanded, uint32_t count);

#endif
