#ifndef WOW_SOURCE_UI_MODIFIEDCLICK_H
#define WOW_SOURCE_UI_MODIFIEDCLICK_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Ui/Types.h"

struct HASHKEY_STRI;
class MODIFIEDCLICK;

struct MODIFIEDCLICK_DATA {
    /* 0000 */ uint32_t m_flags;
    /* 0004 */ uint32_t m_modifiers;
    /* 0008 */ const char* m_button;
};

class MODIFIEDCLICK : public TSHashObject<MODIFIEDCLICK, HASHKEY_STRI> {
    public:
    /* 0018 */ MODIFIEDCLICK_DATA m_data [4];

    void GetBinding(BINDING_SET set, char* binding, int32_t maxLength, uint32_t* flags);
    BOOL IsActive(BINDING_SET set);
    int32_t SetBinding(BINDING_SET set, const char* binding);
};

#endif
