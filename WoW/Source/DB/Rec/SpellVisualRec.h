#ifndef WOW_SOURCE_DB_REC_SPELLVISUALREC_H
#define WOW_SOURCE_DB_REC_SPELLVISUALREC_H

#include <cstddef>
#include <cstdint>

class SFileImpl;

// Descriptive names from the 3.3.5 client tests of SpellVisualRec::m_flags.
// UNKNOWN records an unresolved meaning; it does not imply an unused data bit.
enum SPELL_VISUAL_FLAGS : int32_t {
    SPELL_VISUAL_FLAG_PREFER_UNIT_TARGETS = 0x1,
    SPELL_VISUAL_FLAG_UNKNOWN_1 = 0x2,
    SPELL_VISUAL_FLAG_RELEASE_MISSILES_IMMEDIATELY = 0x4,
    SPELL_VISUAL_FLAG_HAND_ITEM_STATE_KIT = 0x8,
    SPELL_VISUAL_FLAG_DETACHED_SOUND = 0x10,
    SPELL_VISUAL_FLAG_PRECAST_MISSILE_MODEL = 0x20,
    SPELL_VISUAL_FLAG_NO_WOUND_ANIMATION = 0x40,
    SPELL_VISUAL_FLAG_MISSILE_DISTANCE_FROM_LAUNCH = 0x80,
    SPELL_VISUAL_FLAG_REQUIRE_TARGETS_FOR_DESTINATION_MISSILE = 0x100,
    SPELL_VISUAL_FLAG_RAW_ATTACHMENT_IDS = 0x200,
};

// Fixed-point scale of the three missile follow-ground DBC parameters.
constexpr float SPELL_VISUAL_GROUND_PARAMETER_SCALE = 0.001f;

enum SPELL_VISUAL_FOLLOW_GROUND_FLAGS : int32_t {
    SPELL_VISUAL_FOLLOW_GROUND_ENABLED = 0x1,
};

class SpellVisualRec {
    public:
    static const int32_t s_numColumns = 32;
    static const int32_t s_rowSize = 128;
    static const bool s_generatedID = false;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_precastKit;
    /* 0008 */ int32_t m_castKit;
    /* 000C */ int32_t m_impactKit;
    /* 0010 */ int32_t m_stateKit;
    /* 0014 */ int32_t m_stateDoneKit;
    /* 0018 */ int32_t m_channelKit;
    /* 001C */ int32_t m_hasMissile;
    /* 0020 */ int32_t m_missileModel;
    /* 0024 */ int32_t m_missilePathType;
    /* 0028 */ int32_t m_missileDestinationAttachment;
    /* 002C */ int32_t m_missileSound;
    /* 0030 */ int32_t m_animEventSoundID;
    /* 0034 */ int32_t m_flags; // SPELL_VISUAL_FLAGS
    /* 0038 */ int32_t m_casterImpactKit;
    /* 003C */ int32_t m_targetImpactKit;
    /* 0040 */ int32_t m_missileAttachment;
    /* 0044 */ int32_t m_missileFollowGroundHeight;
    /* 0048 */ int32_t m_missileFollowGroundDropSpeed;
    /* 004C */ int32_t m_missileFollowGroundApproach;
    /* 0050 */ int32_t m_missileFollowGroundFlags;
    /* 0054 */ int32_t m_missileMotion;
    /* 0058 */ int32_t m_missileTargetingKit;
    /* 005C */ int32_t m_instantAreaKit;
    /* 0060 */ int32_t m_impactAreaKit;
    /* 0064 */ int32_t m_persistentAreaKit;
    /* 0068 */ float m_missileCastOffset [3];
    /* 0074 */ float m_missileImpactOffset [3];

    SpellVisualRec();
    ~SpellVisualRec();
    static const char* GetFilename();
    bool Read(SFileImpl* file, const char* stringBuffer);
};

#endif
