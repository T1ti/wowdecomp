#ifndef WOW_SOURCE_WORLDCLIENT_CDETAILDOODADDATA_H
#define WOW_SOURCE_WORLDCLIENT_CDETAILDOODADDATA_H

#include <cstddef>
#include <cstdint>

class CM2Model;
class CTexture;
class GroundEffectDoodadRec;

// GroundEffectDoodadRec::m_flags, named from the detail doodad build code.
enum GROUND_EFFECT_DOODAD_FLAGS : int32_t {
    // Orients each instance to the terrain normal (GroundEffectDoodad.dbc: bones, stones and leaves).
    GROUND_EFFECT_DOODAD_ALIGN_TO_TERRAIN = 0x1,
    // Uses white instead of the chunk vertex shading; no 3.3.5 record sets it.
    GROUND_EFFECT_DOODAD_NO_VERTEX_SHADING = 0x2,
};

class CDetailDoodadData {
    public:
    static void ModelLoadedCallback(CM2Model* model, void* arg);

    /* 0000 */ GroundEffectDoodadRec* m_record;
    /* 0004 */ CM2Model* m_model;
    /* 0008 */ CTexture* m_texture;

    int32_t Load();
};

#endif
