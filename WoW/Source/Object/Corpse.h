#ifndef WOW_SOURCE_OBJECT_CORPSE_H
#define WOW_SOURCE_OBJECT_CORPSE_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

class CGCorpseData;

// Replicated corpse flags, separate from runtime and dynamic flag words.
enum CORPSE_FIELD_FLAGS : uint32_t {
    CORPSE_FIELD_FLAG_NONE               = 0x00000000u,
    CORPSE_FIELD_FLAG_BONES              = 0x00000001u,
    // Reference only: TC 3.3.5 CORPSE_FLAG_UNK1 = 0x00000002; no client reader found.
    CORPSE_FIELD_FLAG_PVP                = 0x00000004u, // TC 3.3.5 calls this UNK2; UnitIsPVP establishes its role.
    CORPSE_FIELD_FLAG_HIDE_HELM          = 0x00000008u,
    CORPSE_FIELD_FLAG_HIDE_CLOAK         = 0x00000010u,
    CORPSE_FIELD_FLAG_SKINNABLE          = 0x00000020u, // Player-corpse skin/insignia action; TC calls this LOOTABLE.
};

class CGCorpse {
    public:
    static const int32_t NUM_MIRROR_FIELDS = 3;
    static const uint32_t s_mirrorFields[NUM_MIRROR_FIELDS];

    static uint32_t DescriptorToMirrorOffset(uint32_t offset, uint32_t size, int32_t local, uint32_t base);
    static uint32_t GetSavedIndexFromDataIndex(uint32_t index, uint32_t unused_count, int32_t unused_local);
    static int32_t TotalFieldsSaved();

    /* 0000 */ CGCorpseData* m_corpse;
    /* 0004 */ void* m_corpseMirror;
};

class CGCorpseData {
    public:
    // DWORD indices relative to this data block, not absolute network-field indices.
    enum FIELD_ID : uint32_t {
        CORPSE_FIELD_OWNER = 0,
        CORPSE_FIELD_PARTY = 2,
        CORPSE_FIELD_DISPLAY_ID = 4,
        CORPSE_FIELD_ITEM = 5,
        CORPSE_FIELD_BYTES_1 = 24,
        CORPSE_FIELD_BYTES_2 = 25,
        CORPSE_FIELD_GUILD = 26,
        CORPSE_FIELD_FLAGS = 27,
        CORPSE_FIELD_DYNAMIC_FLAGS = 28,
        CORPSE_FIELD_PAD = 29,
        CORPSE_END = 30,
    };

    // Descriptor spans are measured in DWORDs.
    enum FIELD_SIZE : uint32_t {
        CORPSE_FIELD_OWNER_SIZE = 2,
        CORPSE_FIELD_PARTY_SIZE = 2,
        CORPSE_FIELD_ITEM_SIZE = 19,
    };


    static const int32_t NUM_FIELDS = 30;
    /* 0000 */ WOWGUID m_corpseFieldOwner;
    /* 0008 */ WOWGUID m_corpseFieldParty;
    /* 0010 */ uint32_t m_corpseFieldDisplayId;
    /* 0014 */ uint32_t m_corpseFieldItem [19];
    /* 0060 */ uint8_t m_corpseFieldBytes1 [4];
    /* 0064 */ uint8_t m_corpseFieldBytes2 [4];
    /* 0068 */ uint32_t m_corpseFieldGuild;
    /* 006C */ uint32_t m_corpseFieldFlags;
    /* 0070 */ uint32_t m_corpseFieldDynamicFlags;
    /* 0074 */ uint32_t m_corpseFieldPad;
};

#endif
