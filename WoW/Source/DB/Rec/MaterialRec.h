#ifndef WOW_SOURCE_DB_REC_MATERIALREC_H
#define WOW_SOURCE_DB_REC_MATERIALREC_H

#include <cstddef>
#include <cstdint>

class SFileImpl;

// Material.dbc flags tested by the client; names describe that behavior.
enum MATERIAL_FLAGS : int32_t {
    MATERIAL_FLAG_NONE          = 0x00000000,
    MATERIAL_FLAG_METAL         = 0x00000001,
    // Chest armor of this material reports impact type 2.
    MATERIAL_FLAG_IMPACT_TYPE_2 = 0x00000002,
    // Chest armor of this material reports impact type 1.
    MATERIAL_FLAG_IMPACT_TYPE_1 = 0x00000004,
};

class MaterialRec {
    public:
    static const int32_t s_numColumns = 5;
    static const int32_t s_rowSize = 20;
    static const bool s_generatedID = false;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_flags; // MATERIAL_FLAGS
    /* 0008 */ int32_t m_foleySoundID;
    /* 000C */ int32_t m_sheatheSoundID;
    /* 0010 */ int32_t m_unsheatheSoundID;

    MaterialRec();
    ~MaterialRec();
    static const char* GetFilename();
    bool Read(SFileImpl* file, const char* stringBuffer);
};

#endif
