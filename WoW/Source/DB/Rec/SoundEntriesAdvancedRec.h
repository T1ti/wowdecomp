#ifndef WOW_SOURCE_DB_REC_SOUNDENTRIESADVANCEDREC_H
#define WOW_SOURCE_DB_REC_SOUNDENTRIESADVANCEDREC_H

#include <cstddef>
#include <cstdint>

class SFileImpl;
enum SI2_SOUND_USAGE : int32_t;
enum SI2_SOUND_GROUP : int32_t;

class SoundEntriesAdvancedRec {
    public:
    static const int32_t s_numColumns = 24;
    static const int32_t s_rowSize = 96;
    static const bool s_generatedID = false;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_soundEntryID;
    /* 0008 */ float m_innerRadius2d;
    /* 000C */ int32_t m_timeA;
    /* 0010 */ int32_t m_timeB;
    /* 0014 */ int32_t m_timeC;
    /* 0018 */ int32_t m_timeD;
    /* 001C */ int32_t m_randomOffsetRange;
    /* 0020 */ SI2_SOUND_USAGE m_usage;
    /* 0024 */ int32_t m_timeIntervalMin;
    /* 0028 */ int32_t m_timeIntervalMax;
    /* 002C */ SI2_SOUND_GROUP m_volumeSliderCategory;
    /* 0030 */ float m_duckToSfx;
    /* 0034 */ float m_duckToMusic;
    /* 0038 */ float m_duckToAmbience;
    /* 003C */ float m_innerRadiusOfInfluence;
    /* 0040 */ float m_outerRadiusOfInfluence;
    /* 0044 */ int32_t m_timeToDuck;
    /* 0048 */ int32_t m_timeToUnduck;
    /* 004C */ float m_insideAngle;
    /* 0050 */ float m_outsideAngle;
    /* 0054 */ float m_outsideVolume;
    /* 0058 */ float m_outerRadius2d;
    /* 005C */ const char* m_name;

    SoundEntriesAdvancedRec();
    ~SoundEntriesAdvancedRec();
    static const char* GetFilename();
    bool Read(SFileImpl* file, const char* stringBuffer);
};

#endif
