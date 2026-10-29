#ifndef WOW_SOURCE_WORLDCLIENT_MAPLOAD_H
#define WOW_SOURCE_WORLDCLIENT_MAPLOAD_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CAaSphere.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "WoW/Source/WorldClient/MapBaseObj.h"

class CM2Lighting;
class CMapObjDef;

class CM2Model;

class CMapObj;
class CMapObjDef;
class CMapObjDefGroup;
class CMapObjGroup;

// Runtime CMapStaticEntity state, reconstructed from the 3.3.5 client.
// Earlier bitfield layouts differ; CMapBaseObj::m_flags is a separate domain.
enum MAP_STATIC_FLAGS : uint32_t {
    // Classified inside a WMO interior.
    MAP_STATIC_FLAG_INSIDE                       = 0x00000001u,
    // Include this entity in object intersection/collision queries.
    MAP_STATIC_FLAG_COLLIDABLE                   = 0x00000002u,
    MAP_STATIC_FLAG_HIDDEN                       = 0x00000004u,
    // Terrain shadow shading is applied to the entity.
    MAP_STATIC_FLAG_SHADOWED                     = 0x00000008u,
    // Created by CWorld::ObjectCreate; bypasses terrain-doodad preparation.
    MAP_STATIC_FLAG_RUNTIME_DOODAD               = 0x00000010u,
    MAP_STATIC_FLAG_IN_LIQUID                    = 0x00000020u,
    // Liquid height is at or below the bounds top; clip lighting against its surface.
    MAP_STATIC_FLAG_LIQUID_SURFACE_CLIP          = 0x00000040u,
    // Doodad liquid classification is cached until its parent/group invalidates it.
    MAP_STATIC_FLAG_LIQUID_QUERY_CACHED          = 0x00000080u,
    // LiquidType flags bit 0; permits ripples.
    MAP_STATIC_FLAG_LIQUID_RIPPLES               = 0x00000100u,
    // LiquidType flags bit 1; permits splashes.
    MAP_STATIC_FLAG_LIQUID_SPLASHES              = 0x00000200u,
    // Keep animating even when culled or outside the normal update distance.
    MAP_STATIC_FLAG_ALWAYS_ANIMATE               = 0x00000400u,
    // Render the projected blob shadow through the legacy RequestModelStreaming path.
    MAP_STATIC_FLAG_CAST_BLOB_SHADOW             = 0x00000800u,
    // Blend WMO interior lighting with outdoor daylight.
    MAP_STATIC_FLAG_BLEND_OUTDOOR_LIGHT          = 0x00001000u,
    // Use the multiple-WMO-group linking path.
    MAP_STATIC_FLAG_LINK_MULTIPLE_WMO_GROUPS     = 0x00002000u,
    // Invisible-stalker model handling, including attachment and split-body culling.
    MAP_STATIC_FLAG_INVISIBLE_STALKER            = 0x00004000u,
    // Scale ambient lighting by s_characterAmbient.
    MAP_STATIC_FLAG_USE_CHARACTER_AMBIENT        = 0x00008000u,
    // Doodad loading requested; cleared after LinkToAsyncHead handles the request.
    MAP_STATIC_FLAG_STREAMING_LOAD_PENDING       = 0x00010000u,
    // Bits 17-31 have no observed consumers; reset masks preserve them.
    MAP_STATIC_FLAG_UNKNOWN_17                   = 0x00020000u,
    MAP_STATIC_FLAG_UNKNOWN_18                   = 0x00040000u,
    MAP_STATIC_FLAG_UNKNOWN_19                   = 0x00080000u,
    MAP_STATIC_FLAG_UNKNOWN_20                   = 0x00100000u,
    MAP_STATIC_FLAG_UNKNOWN_21                   = 0x00200000u,
    MAP_STATIC_FLAG_UNKNOWN_22                   = 0x00400000u,
    MAP_STATIC_FLAG_UNKNOWN_23                   = 0x00800000u,
    MAP_STATIC_FLAG_UNKNOWN_24                   = 0x01000000u,
    MAP_STATIC_FLAG_UNKNOWN_25                   = 0x02000000u,
    MAP_STATIC_FLAG_UNKNOWN_26                   = 0x04000000u,
    MAP_STATIC_FLAG_UNKNOWN_27                   = 0x08000000u,
    MAP_STATIC_FLAG_UNKNOWN_28                   = 0x10000000u,
    MAP_STATIC_FLAG_UNKNOWN_29                   = 0x20000000u,
    MAP_STATIC_FLAG_UNKNOWN_30                   = 0x40000000u,
    MAP_STATIC_FLAG_UNKNOWN_31                   = 0x80000000u,

    MAP_STATIC_FLAG_LIQUID_STATE_MASK = MAP_STATIC_FLAG_IN_LIQUID
        | MAP_STATIC_FLAG_LIQUID_SURFACE_CLIP,
    MAP_STATIC_FLAG_LIQUID_CACHE_MASK = MAP_STATIC_FLAG_IN_LIQUID
        | MAP_STATIC_FLAG_LIQUID_SURFACE_CLIP
        | MAP_STATIC_FLAG_LIQUID_QUERY_CACHED,
    MAP_STATIC_FLAG_CONSTRUCTOR_CLEAR_MASK = MAP_STATIC_FLAG_INSIDE
        | MAP_STATIC_FLAG_COLLIDABLE
        | MAP_STATIC_FLAG_HIDDEN
        | MAP_STATIC_FLAG_SHADOWED
        | MAP_STATIC_FLAG_RUNTIME_DOODAD
        | MAP_STATIC_FLAG_IN_LIQUID
        | MAP_STATIC_FLAG_LIQUID_SURFACE_CLIP
        | MAP_STATIC_FLAG_LIQUID_QUERY_CACHED
        | MAP_STATIC_FLAG_LIQUID_RIPPLES
        | MAP_STATIC_FLAG_LIQUID_SPLASHES
        | MAP_STATIC_FLAG_ALWAYS_ANIMATE
        | MAP_STATIC_FLAG_CAST_BLOB_SHADOW
        | MAP_STATIC_FLAG_BLEND_OUTDOOR_LIGHT
        | MAP_STATIC_FLAG_LINK_MULTIPLE_WMO_GROUPS
        | MAP_STATIC_FLAG_USE_CHARACTER_AMBIENT
        | MAP_STATIC_FLAG_STREAMING_LOAD_PENDING,
    MAP_STATIC_FLAG_CREATE_CLEAR_MASK = MAP_STATIC_FLAG_COLLIDABLE
        | MAP_STATIC_FLAG_ALWAYS_ANIMATE
        | MAP_STATIC_FLAG_CAST_BLOB_SHADOW
        | MAP_STATIC_FLAG_LINK_MULTIPLE_WMO_GROUPS
        | MAP_STATIC_FLAG_INVISIBLE_STALKER
        | MAP_STATIC_FLAG_USE_CHARACTER_AMBIENT,
    MAP_STATIC_FLAG_UPDATE_CLEAR_MASK = MAP_STATIC_FLAG_INSIDE
        | MAP_STATIC_FLAG_SHADOWED
        | MAP_STATIC_FLAG_IN_LIQUID
        | MAP_STATIC_FLAG_LIQUID_SURFACE_CLIP
        | MAP_STATIC_FLAG_LIQUID_RIPPLES
        | MAP_STATIC_FLAG_LIQUID_SPLASHES,
    MAP_STATIC_FLAG_STATIC_CREATE_CLEAR_MASK = MAP_STATIC_FLAG_INSIDE
        | MAP_STATIC_FLAG_COLLIDABLE
        | MAP_STATIC_FLAG_HIDDEN
        | MAP_STATIC_FLAG_SHADOWED
        | MAP_STATIC_FLAG_IN_LIQUID
        | MAP_STATIC_FLAG_LIQUID_SURFACE_CLIP
        | MAP_STATIC_FLAG_ALWAYS_ANIMATE
        | MAP_STATIC_FLAG_CAST_BLOB_SHADOW
        | MAP_STATIC_FLAG_BLEND_OUTDOOR_LIGHT
        | MAP_STATIC_FLAG_INVISIBLE_STALKER,
    MAP_STATIC_FLAG_STATIC_EX_CREATE_CLEAR_MASK = MAP_STATIC_FLAG_INSIDE
        | MAP_STATIC_FLAG_COLLIDABLE
        | MAP_STATIC_FLAG_HIDDEN
        | MAP_STATIC_FLAG_SHADOWED
        | MAP_STATIC_FLAG_IN_LIQUID
        | MAP_STATIC_FLAG_CAST_BLOB_SHADOW
        | MAP_STATIC_FLAG_BLEND_OUTDOOR_LIGHT
        | MAP_STATIC_FLAG_INVISIBLE_STALKER,
};

// Translate World::ObjectCreate input flags into this separate state word.
constexpr uint32_t MapStaticFlagsFromObjectCreateFlags(uint32_t flags) {
    return ((flags & 0x1u) ? MAP_STATIC_FLAGS::MAP_STATIC_FLAG_COLLIDABLE : 0u)
        | ((flags & 0x4u) ? MAP_STATIC_FLAGS::MAP_STATIC_FLAG_ALWAYS_ANIMATE : 0u)
        | (!(flags & 0x2u) ? MAP_STATIC_FLAGS::MAP_STATIC_FLAG_CAST_BLOB_SHADOW : 0u)
        | ((flags & 0x8u) ? MAP_STATIC_FLAGS::MAP_STATIC_FLAG_LINK_MULTIPLE_WMO_GROUPS : 0u)
        | ((flags & 0x10u) ? MAP_STATIC_FLAGS::MAP_STATIC_FLAG_USE_CHARACTER_AMBIENT : 0u);
}

class CMapStaticEntity : public CMapBaseObj {
    public:
    static float s_characterAmbient;
    static C3Vector s_interiorSunDir;

    static void ModelEventCallback(CM2Model* model, uint32_t boneId, uint32_t eventId, uint32_t data, const C3Vector* position, uint32_t time, uint64_t param);
    static void ModelLightingCallback(CM2Model* model, CM2Lighting* lighting, void* userArg);

    ~CMapStaticEntity() override;

    /* 0024 */ uint8_t m_fadeLevel;
    /* 0025 */ uint8_t m_culled;
    /* 0026 */ uint8_t unused_0026;
    /* 0027 */ uint8_t unused_0027;
    /* 0028 */ uint8_t m_shadowStamps[3];
    /* 002B */ uint8_t unused_002B;
    /* 002C */ int32_t m_unkCounter;
    /* 0030 */ float m_distanceToCamera;
    /* 0034 */ CM2Model* m_model;
    /* 0038 */ CAaSphere m_sphere;
    /* 0048 */ CAaBox m_bbox;
    /* 0060 */ C3Vector m_vec2;
    /* 006C */ C3Vector m_position;
    /* 0078 */ float m_scale;
    /* 007C */ uint32_t m_staticFlags; // MAP_STATIC_FLAGS
    /* 0080 */ float m_liquidHeight;
    /* 0084 */ CImVector m_m2AmbientColor;
    /* 0088 */ CImVector m_m2DiffuseColor;
    /* 008C */ float m_diffuseLightScale;

    virtual void SelectLights(CM2Lighting* lighting);
    virtual void SelectUnderwater(CM2Lighting* lighting);
    virtual bool QueryInteriorLighting(CMapObjDef* mapObjDef, int32_t groupNum, uint16_t* faceIndex, C3Vector* pos) = 0;

    CMapStaticEntity();

    void AdjustLightmap(const CImVector* color, CImVector* diffuse, CImVector limit, CImVector* ambient, uint8_t maxBrightness);

    bool GetMapObjAndGroup(CMapObjDef** mapObjDef, CMapObj** mapObj, CMapObjDefGroup** mapObjDefGroup, CMapObjGroup** mapObjGroup, int32_t skipHidden);
};

#endif
