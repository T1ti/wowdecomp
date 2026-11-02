#ifndef WOW_COMMON_ITEMTYPES_H
#define WOW_COMMON_ITEMTYPES_H

#include <cstdint>

// Number of parallel spell entries in an ItemStats_C cache record.
constexpr int32_t NUM_ITEM_SPELLS = 5;

// 3.3.5 ItemClass.dbc IDs. NONE is the signed no-class/restriction sentinel;
// actual item records keep their existing raw int32/uint32/uint8 carriers.
enum ITEM_CLASS : int32_t {
    ITEM_CLASS_NONE                          = -1,
    ITEM_CLASS_CONSUMABLE                    = 0,
    ITEM_CLASS_CONTAINER                     = 1,
    ITEM_CLASS_WEAPON                        = 2,
    ITEM_CLASS_GEM                           = 3,
    ITEM_CLASS_ARMOR                         = 4,
    ITEM_CLASS_REAGENT                       = 5,
    ITEM_CLASS_PROJECTILE                    = 6,
    ITEM_CLASS_TRADE_GOODS                   = 7,
    ITEM_CLASS_GENERIC                       = 8,
    ITEM_CLASS_RECIPE                        = 9,
    ITEM_CLASS_MONEY                         = 10,
    ITEM_CLASS_QUIVER                        = 11,
    ITEM_CLASS_QUEST                         = 12,
    ITEM_CLASS_KEY                           = 13,
    ITEM_CLASS_PERMANENT                     = 14,
    ITEM_CLASS_MISCELLANEOUS                 = 15,
    ITEM_CLASS_GLYPH                         = 16,
    NUM_ITEM_CLASSES                         = 17,
};

// ItemSubClass.dbc IDs only for ITEM_CLASS_WEAPON. Other item classes use
// independent subclass domains; subclass masks and sound overrides stay raw.
enum ITEM_SUBCLASS_WEAPON : int32_t {
    ITEM_SUBCLASS_WEAPON_AXE                 = 0,
    ITEM_SUBCLASS_WEAPON_AXE2                = 1,
    ITEM_SUBCLASS_WEAPON_BOW                 = 2,
    ITEM_SUBCLASS_WEAPON_GUN                 = 3,
    ITEM_SUBCLASS_WEAPON_MACE                = 4,
    ITEM_SUBCLASS_WEAPON_MACE2               = 5,
    ITEM_SUBCLASS_WEAPON_POLEARM             = 6,
    ITEM_SUBCLASS_WEAPON_SWORD               = 7,
    ITEM_SUBCLASS_WEAPON_SWORD2              = 8,
    ITEM_SUBCLASS_WEAPON_OBSOLETE            = 9,
    ITEM_SUBCLASS_WEAPON_STAFF               = 10,
    ITEM_SUBCLASS_WEAPON_EXOTIC              = 11,
    ITEM_SUBCLASS_WEAPON_EXOTIC2             = 12,
    ITEM_SUBCLASS_WEAPON_FIST_WEAPON         = 13,
    ITEM_SUBCLASS_WEAPON_MISCELLANEOUS       = 14,
    ITEM_SUBCLASS_WEAPON_DAGGER              = 15,
    ITEM_SUBCLASS_WEAPON_THROWN              = 16,
    ITEM_SUBCLASS_WEAPON_SPEAR               = 17,
    ITEM_SUBCLASS_WEAPON_CROSSBOW            = 18,
    ITEM_SUBCLASS_WEAPON_WAND                = 19,
    ITEM_SUBCLASS_WEAPON_FISHING_POLE        = 20,
    NUM_ITEM_WEAPON_SUBCLASSES               = 21,
};

// Binding policies recognized by the 3.3.5 client. Packet/cache words remain
// raw uint32_t; other values retain the existing fallback behavior.
enum ITEM_BINDING : uint32_t {
    ITEM_BINDING_NONE                    = 0,
    ITEM_BINDING_ON_PICKUP               = 1,
    ITEM_BINDING_ON_EQUIP                = 2,
    ITEM_BINDING_ON_USE                  = 3,
    ITEM_BINDING_QUEST                   = 4,
};

// Item spell-trigger tags interpreted by the 3.3.5 client. Other raw tags
// (including 3, 4 and 5) retain their existing default handling.
// Commented entries preserve later ItemEffect::TriggerType reference names;
// their availability and meaning in 3.3.5 are unverified. TrinityCore labels
// are separate, non-authoritative guesses where noted.
enum ITEM_SPELL_TRIGGER : uint32_t {
    ITEM_SPELL_TRIGGER_ON_USE            = 0,
    ITEM_SPELL_TRIGGER_ON_EQUIP          = 1,
    ITEM_SPELL_TRIGGER_ON_PROC           = 2,
    // ITEM_SPELL_TRIGGER_SUMMONED_BY_SPELL = 3,
    // ITEM_SPELL_TRIGGER_ON_DEATH         = 4, // TC 3.3.5: ITEM_SPELLTRIGGER_SOULSTONE.
    // ITEM_SPELL_TRIGGER_ON_PICKUP        = 5, // TC 3.3.5: ITEM_SPELLTRIGGER_ON_NO_DELAY_USE (uncertain).
    ITEM_SPELL_TRIGGER_LEARN_SPELL       = 6, // Later name: OnLearn.
    // ITEM_SPELL_TRIGGER_ON_LOOTED        = 7,
    // ITEM_SPELL_TRIGGER_TEACH_MOUNT      = 8,
};

#endif
