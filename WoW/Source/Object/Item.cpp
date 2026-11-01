#include "Item.h"
#include "WoW/Source/Object/Object.h"

// STATIC: 0x00AC47F0
const uint32_t CGItem::s_mirrorFields[CGItem::NUM_MIRROR_FIELDS] = {
    CGItemData::FIELD_ID::ITEM_FIELD_OWNER,
    CGItemData::FIELD_ID::ITEM_FIELD_OWNER + 1,
    CGItemData::FIELD_ID::ITEM_FIELD_STACK_COUNT,
    CGItemData::FIELD_ID::ITEM_FIELD_SPELL_CHARGES,
    CGItemData::FIELD_ID::ITEM_FIELD_SPELL_CHARGES + 1,
    CGItemData::FIELD_ID::ITEM_FIELD_SPELL_CHARGES + 2,
    CGItemData::FIELD_ID::ITEM_FIELD_SPELL_CHARGES + 3,
    CGItemData::FIELD_ID::ITEM_FIELD_SPELL_CHARGES + 4,
    CGItemData::FIELD_ID::ITEM_FIELD_FLAGS,
    CGItemData::FIELD_ID::ITEM_FIELD_ENCHANTMENT_1_1,
    CGItemData::FIELD_ID::ITEM_FIELD_ENCHANTMENT_1_1 + 1,
    CGItemData::FIELD_ID::ITEM_FIELD_ENCHANTMENT_1_3,
    CGItemData::FIELD_ID::ITEM_FIELD_ENCHANTMENT_2_1,
    CGItemData::FIELD_ID::ITEM_FIELD_ENCHANTMENT_2_1 + 1,
    CGItemData::FIELD_ID::ITEM_FIELD_ENCHANTMENT_2_3,
    CGItemData::FIELD_ID::ITEM_FIELD_ENCHANTMENT_3_1,
    CGItemData::FIELD_ID::ITEM_FIELD_ENCHANTMENT_3_1 + 1,
    CGItemData::FIELD_ID::ITEM_FIELD_ENCHANTMENT_3_3,
    CGItemData::FIELD_ID::ITEM_FIELD_ENCHANTMENT_4_1,
    CGItemData::FIELD_ID::ITEM_FIELD_ENCHANTMENT_4_1 + 1,
    CGItemData::FIELD_ID::ITEM_FIELD_ENCHANTMENT_4_3,
    CGItemData::FIELD_ID::ITEM_FIELD_ENCHANTMENT_5_1,
    CGItemData::FIELD_ID::ITEM_FIELD_ENCHANTMENT_5_1 + 1,
    CGItemData::FIELD_ID::ITEM_FIELD_ENCHANTMENT_5_3,
    CGItemData::FIELD_ID::ITEM_FIELD_ENCHANTMENT_6_1,
    CGItemData::FIELD_ID::ITEM_FIELD_ENCHANTMENT_6_1 + 1,
    CGItemData::FIELD_ID::ITEM_FIELD_ENCHANTMENT_6_3,
    CGItemData::FIELD_ID::ITEM_FIELD_ENCHANTMENT_7_1,
    CGItemData::FIELD_ID::ITEM_FIELD_ENCHANTMENT_7_1 + 1,
    CGItemData::FIELD_ID::ITEM_FIELD_ENCHANTMENT_7_3,
    CGItemData::FIELD_ID::ITEM_FIELD_ENCHANTMENT_8_1,
    CGItemData::FIELD_ID::ITEM_FIELD_ENCHANTMENT_8_1 + 1,
    CGItemData::FIELD_ID::ITEM_FIELD_ENCHANTMENT_8_3,
    CGItemData::FIELD_ID::ITEM_FIELD_ENCHANTMENT_9_1,
    CGItemData::FIELD_ID::ITEM_FIELD_ENCHANTMENT_9_1 + 1,
    CGItemData::FIELD_ID::ITEM_FIELD_ENCHANTMENT_9_3,
    CGItemData::FIELD_ID::ITEM_FIELD_ENCHANTMENT_10_1,
    CGItemData::FIELD_ID::ITEM_FIELD_ENCHANTMENT_10_1 + 1,
    CGItemData::FIELD_ID::ITEM_FIELD_ENCHANTMENT_10_3,
    CGItemData::FIELD_ID::ITEM_FIELD_ENCHANTMENT_11_1,
    CGItemData::FIELD_ID::ITEM_FIELD_ENCHANTMENT_11_1 + 1,
    CGItemData::FIELD_ID::ITEM_FIELD_ENCHANTMENT_11_3,
    CGItemData::FIELD_ID::ITEM_FIELD_ENCHANTMENT_12_1,
    CGItemData::FIELD_ID::ITEM_FIELD_ENCHANTMENT_12_1 + 1,
    CGItemData::FIELD_ID::ITEM_FIELD_ENCHANTMENT_12_3,
    CGItemData::FIELD_ID::ITEM_FIELD_DURABILITY,
    CGItemData::FIELD_ID::ITEM_FIELD_MAXDURABILITY,
};

// VERIFY: a26e03f9 auto
// OFFSET: 0x004F51D0
uint32_t CGItem::GetSavedIndexFromDataIndex(uint32_t index, uint32_t unused_count, int32_t unused_local) {
    for (uint32_t i = 0; i < sizeof(CGItem::s_mirrorFields) / sizeof(CGItem::s_mirrorFields[0]); i++) {
        if (CGItem::s_mirrorFields[i] == index) {
            return i;
        }
    }

    return CGItemData::NUM_FIELDS;
}

// VERIFY: cd0589de auto
// OFFSET: 0x004D42C0
uint32_t CGItem::DescriptorToMirrorOffset(uint32_t offset, uint32_t size, int32_t local, uint32_t base) {
    int32_t total = CGObject::TotalFieldsSaved();

    return (offset & 3) + 4 * (total + CGItem::GetSavedIndexFromDataIndex((offset - base) >> 2, (size + 3) >> 2, local));
}

// VERIFY: 11b0e3ff auto
// OFFSET: 0x004F51C0
int32_t CGItem::TotalFieldsSaved() {
    return CGObject::TotalFieldsSaved() + sizeof(CGItem::s_mirrorFields) / sizeof(CGItem::s_mirrorFields[0]);
}
