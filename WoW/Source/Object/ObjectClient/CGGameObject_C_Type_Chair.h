#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_CGGAMEOBJECT_C_TYPE_CHAIR_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_CGGAMEOBJECT_C_TYPE_CHAIR_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/ObjectClient/GameObject_C.h"

constexpr uint32_t MAX_CHAIR_SLOTS = 5;

void GenerateChairPoints(const C44Matrix& matrix, uint32_t slots, C3Vector* out, float offset);

class CGGameObject_C_Type_Chair : public CGGameObject_C_TypeBase {
    public:
    /* 0010 */ C3Vector m_slots[MAX_CHAIR_SLOTS];

    virtual bool CanUse() const;
    virtual bool CanUseNow(GAME_ERROR_TYPE* error, float* distance, const char** errorText) const;
    virtual void PostInitWithStats();
    virtual uint32_t GetNumSlots() const;

    CGGameObject_C_Type_Chair(CGGameObject_C* owner);
};

#endif
