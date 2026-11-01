#ifndef WOW_SOURCE_OBJECT_ITEM_H
#define WOW_SOURCE_OBJECT_ITEM_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

class CGItemData;

class CGItem {
    public:
    static const int32_t NUM_MIRROR_FIELDS = 47;
    static const uint32_t s_mirrorFields[NUM_MIRROR_FIELDS];

    static uint32_t DescriptorToMirrorOffset(uint32_t offset, uint32_t size, int32_t local, uint32_t base);
    static uint32_t GetSavedIndexFromDataIndex(uint32_t index, uint32_t unused_count, int32_t unused_local);
    static int32_t TotalFieldsSaved();

    /* 0004 */ CGItemData* m_item;
    /* 0008 */ void* m_itemMirror;

    virtual uint32_t GetItemStaticFlags(int32_t a1) = 0;

    int32_t IsUsable() const;
    uint32_t GetEnchantmentId(int32_t slot);
    uint32_t GetEnchantmentDuration(int32_t slot);
    int32_t GetEnchantmentCharges(int32_t slot);
    uint32_t GetDurability() const;
    uint32_t GetMaxDurability() const;
    int32_t GetPetitionSignatures();
};

struct ItemEnchantment {
    /* 0000 */ uint32_t m_id;
    /* 0004 */ uint32_t m_duration;
    /* 0008 */ int16_t m_charges;
    /* 000A */ uint16_t unk_000A;
};

class CGItemData {
    public:
    static const int32_t NUM_FIELDS = 58;
    /* 0000 */ WOWGUID m_itemFieldOwner;
    /* 0008 */ WOWGUID m_itemFieldContained;
    /* 0010 */ WOWGUID m_itemFieldCreator;
    /* 0018 */ WOWGUID m_itemFieldGiftcreator;
    /* 0020 */ int32_t m_itemFieldStackCount;
    /* 0024 */ uint32_t m_itemFieldDuration;
    /* 0028 */ int32_t m_itemFieldSpellCharges [5];
    /* 003C */ uint32_t m_itemFieldFlags;
    /* 0040 */ ItemEnchantment m_itemFieldEnchantment [12];
    /* 00D0 */ uint32_t m_itemFieldPropertySeed;
    /* 00D4 */ int32_t m_itemFieldRandomPropertiesId;
    /* 00D8 */ uint32_t m_itemFieldDurability;
    /* 00DC */ uint32_t m_itemFieldMaxdurability;
    /* 00E0 */ uint32_t m_itemFieldCreatePlayedTime;
    /* 00E4 */ uint32_t m_itemFieldPad;
};

#endif
