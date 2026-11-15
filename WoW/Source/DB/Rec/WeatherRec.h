#ifndef WOW_SOURCE_DB_REC_WEATHERREC_H
#define WOW_SOURCE_DB_REC_WEATHERREC_H

#include <cstddef>
#include <cstdint>

class SFileImpl;

// Descriptive 3.3.5 precipitation selectors, distinct from Weather.dbc record IDs.
// PENDING_NONE is only the runtime pending-slot sentinel; SetType rejects it.
enum WEATHER_EFFECT_TYPE : int32_t {
    WEATHER_EFFECT_PENDING_NONE = -1,
    WEATHER_EFFECT_NONE = 0,
    WEATHER_EFFECT_RAIN = 1,
    WEATHER_EFFECT_SNOW = 2,
    WEATHER_EFFECT_SAND = 3,
};

class WeatherRec {
    public:
    static const int32_t s_numColumns = 8;
    static const int32_t s_rowSize = 32;
    static const bool s_generatedID = false;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_ambienceID;
    /* 0008 */ int32_t m_effectType; // WEATHER_EFFECT_TYPE; raw carrier retained for native code generation.
    /* 000C */ float m_transitionSkyBox;
    /* 0010 */ float m_effectColor [3];
    /* 001C */ const char* m_effectTexture;

    WeatherRec();
    ~WeatherRec();
    static const char* GetFilename();
    bool Read(SFileImpl* file, const char* stringBuffer);
};

#endif
