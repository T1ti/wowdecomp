#ifndef WOW_SOURCE_DB_REC_OBJECTEFFECTREC_H
#define WOW_SOURCE_DB_REC_OBJECTEFFECTREC_H

#include <cstddef>
#include <cstdint>

class SFileImpl;

// ObjectEffect.dbc m_effectRecType: what m_effectRecID refers to.
enum OBJECT_EFFECT_REC_TYPE : int32_t {
    OBJECT_EFFECT_REC_TYPE_SOUND_KIT = 1, // m_effectRecID is a sound kit (SoundEntries) ID.
    OBJECT_EFFECT_REC_TYPE_UNKNOWN_2 = 2, // Skipped by the one-shot update; no other client reader.
};

// ObjectEffect.dbc m_triggerType: which CObjectEffectGroupDef list the effect joins.
enum OBJECT_EFFECT_TRIGGER_TYPE : int32_t {
    OBJECT_EFFECT_TRIGGER_ENTER = 1,
    OBJECT_EFFECT_TRIGGER_LOOP = 2,
    OBJECT_EFFECT_TRIGGER_EXIT = 3,
    OBJECT_EFFECT_TRIGGER_EVENT = 4, // Grouped by m_eventType.
};

class ObjectEffectRec {
    public:
    static const int32_t s_numColumns = 12;
    static const int32_t s_rowSize = 48;
    static const bool s_generatedID = false;

    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_objectEffectGroupID;
    /* 000C */ int32_t m_triggerType; // OBJECT_EFFECT_TRIGGER_TYPE
    /* 0010 */ int32_t m_eventType;
    /* 0014 */ int32_t m_effectRecType; // OBJECT_EFFECT_REC_TYPE
    /* 0018 */ int32_t m_effectRecID;
    /* 001C */ int32_t m_attachment;
    /* 0020 */ float m_offset [3];
    /* 002C */ int32_t m_objectEffectModifierID;

    ObjectEffectRec();
    ~ObjectEffectRec();
    static const char* GetFilename();
    bool Read(SFileImpl* file, const char* stringBuffer);
};

#endif
