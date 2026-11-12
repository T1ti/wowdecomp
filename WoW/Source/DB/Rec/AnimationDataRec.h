#ifndef WOW_SOURCE_DB_REC_ANIMATIONDATAREC_H
#define WOW_SOURCE_DB_REC_ANIMATIONDATAREC_H

#include <cstddef>
#include <cstdint>

class SFileImpl;

class AnimationDataRec {
    public:
    // Separate flag words from AnimationData.dbc; only observed bit meanings are named.

      // commented flags are used in animationdata.dbc but unsued by the client.(possibly server related)
    enum WeaponFlags : int32_t {
        // ANIM_WEAPON_FLAG_UNUSED_0 = 0x1,
        // ANIM_WEAPON_FLAG_UNUSED_1 = 0x2,
        ANIM_WEAPON_FLAG_UNKNOWN_2 = 0x4, // sheathe related
        // ANIM_WEAPON_FLAG_UNUSED_3 = 0x8,
        ANIM_WEAPON_FLAG_SHEATHE = 0x10, // MoveWeaponsToSheath.
        ANIM_WEAPON_FLAG_UNSHEATHE = 0x20, // MoveMeleeWeaponsToHand 
    };

    enum BodyFlags : int32_t {
        // ANIM_BODY_FLAG_UNKNOWN_0 = 0x1,
        // ANIM_BODY_FLAG_UNKNOWN_1 = 0x2,
        // ANIM_BODY_FLAG_UNKNOWN_2 = 0x4,
        // ANIM_BODY_FLAG_UNKNOWN_3 = 0x8,
        // ANIM_BODY_FLAG_UNUSED_4 = 0x10,
        // ANIM_BODY_FLAG_UNKNOWN_5 = 0x20,
        // ANIM_BODY_FLAG_UNKNOWN_6 = 0x40,
        ANIM_BODY_FLAG_SUPPRESS_MOVEMENT = 0x80,
        // ANIM_BODY_FLAG_UNKNOWN_8 = 0x100, // Most emotes use this flag.
        // ANIM_BODY_FLAG_UNKNOWN_9 = 0x200,
        // ANIM_BODY_FLAG_UNKNOWN_10 = 0x400,
        // ANIM_BODY_FLAG_UNKNOWN_11 = 0x800,
    };

    enum FallbackFlags : int32_t {
        // ANIM_FLAG_UNKNOWN_0 = 0x1,
        // ANIM_FLAG_UNKNOWN_1 = 0x2,
        // ANIM_FLAG_UNKNOWN_2 = 0x4,
        // ANIM_FLAG_UNKNOWN_3 = 0x8,
        ANIM_FLAG_REVERSE_FALLBACK = 0x10, // FallbackPlaysBackwards
        ANIM_FLAG_FREEZE_FALLBACK = 0x20, // FallbackHoldsLastFrame
        // ANIM_FLAG_UNKNOWN_6 = 0x40, // all emotes use this flag.
    };

    static const int32_t s_numColumns = 8;
    static const int32_t s_rowSize = 32;
    static const bool s_generatedID = false;

    /* 0000 */ int32_t m_id; // Global ANIMATION_ID.
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_weaponflags;
    /* 000C */ int32_t m_bodyflags;
    /* 0010 */ int32_t m_flags; // FallbackFlags
    /* 0014 */ int32_t m_fallback; // Global ANIMATION_ID.
    /* 0018 */ int32_t m_behaviorID; // Base ANIMATION_ID shared by behavior tiers.
    /* 001C */ int32_t m_behaviorTier;

    AnimationDataRec();
    ~AnimationDataRec();
    static const char* GetFilename();
    bool Read(SFileImpl* file, const char* stringBuffer);
};

#endif
