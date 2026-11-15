#ifndef WOW_SOURCE_DB_REC_LIQUIDMATERIALREC_H
#define WOW_SOURCE_DB_REC_LIQUIDMATERIALREC_H

#include <cstddef>
#include <cstdint>

class SFileImpl;

// LiquidMaterial.dbc flags tested by the client.
enum LIQUID_MATERIAL_FLAGS : int32_t {
    LIQUID_MATERIAL_FLAG_NONE = 0x0,
    // Copied into Liquid::CMaterialSettings::m_transparent.
    LIQUID_MATERIAL_FLAG_TRANSPARENT = 0x1,
};

class LiquidMaterialRec {
    public:
    static const int32_t s_numColumns = 3;
    static const int32_t s_rowSize = 12;
    static const bool s_generatedID = false;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_lvf;
    /* 0008 */ int32_t m_flags; // LIQUID_MATERIAL_FLAGS

    LiquidMaterialRec();
    ~LiquidMaterialRec();
    static const char* GetFilename();
    bool Read(SFileImpl* file, const char* stringBuffer);
};

#endif
