#ifndef WOW_SOURCE_DB_REC_LIGHTSKYBOXREC_H
#define WOW_SOURCE_DB_REC_LIGHTSKYBOXREC_H

#include <cstddef>
#include <cstdint>

class SFileImpl;

// LightSkybox.dbc flags tested by the client; names describe that behavior.
enum LIGHT_SKYBOX_FLAGS : int32_t {
    LIGHT_SKYBOX_FLAG_NONE                          = 0x00000000,
    // The skybox animation is timed to the time of day.
    LIGHT_SKYBOX_FLAG_FULL_DAY                      = 0x00000001,
    // Kept blended with lower skyboxes even when fully weighted.
    LIGHT_SKYBOX_FLAG_COMBINE_PROCEDURAL_AND_SKYBOX = 0x00000002,
};

class LightSkyboxRec {
    public:
    static const int32_t s_numColumns = 3;
    static const int32_t s_rowSize = 12;
    static const bool s_generatedID = false;

    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_flags; // LIGHT_SKYBOX_FLAGS

    LightSkyboxRec();
    ~LightSkyboxRec();
    static const char* GetFilename();
    bool Read(SFileImpl* file, const char* stringBuffer);
};

#endif
