#include "MapLight.h"
#include "Engine/Source/Model2/M2Light.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/WorldClient/CMapBaseObjLink.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapArea.h"
#include "WoW/Source/WorldClient/MapChunk.h"
#include "WoW/Source/WorldClient/MapObj.h"
#include "WoW/Source/WorldClient/MapObjDef.h"
#include "WoW/Source/WorldClient/WorldConstants.h"

// VERIFY: e1002a60 manual
// OFFSET: 0x007D9B10
CMapLight::CMapLight() {
    this->m_sphere.r = 0.0f;
    this->AddType(CMapBaseObj::Type_Light);
    this->m_attenDenom = 0.0f;
    this->m_attenEnd = 0.0f;
    this->m_dynamic = 0;
    this->m_attenStart = 0.0f;
}

// INLINED
CMapLight::~CMapLight() {
    STORM_ASSERT(this->m_refCount == 0);
}

// VERIFY: 0aba9189 manual
// OFFSET: 0x007D9BD0
CMapLight* CMap::CreateLight(uint8_t dynamic, uint8_t excludeFromMapObjLighting) {
    CMapLight* light = CMap::AllocLight();
    light->m_flags = MAPOBJ_FLAG_NONE;
    light->m_position = C3Vector(0.0f, 0.0f, 0.0f);
    light->m_sphere.r = 0.0f;
    light->m_sphere.c = C3Vector(0.0f, 0.0f, 0.0f);
    light->m_bbox.b = C3Vector(0.0f, 0.0f, 0.0f);
    light->m_attenDenom = 0.0f;
    light->m_attenEnd = 0.0f;
    light->m_bbox.t = C3Vector(0.0f, 0.0f, 0.0f);
    light->m_attenStart = 0.0f;
    light->m_dynamic = dynamic;
    light->m_excludeFromMapObjLighting = excludeFromMapObjLighting;

    return light;
}

// VERIFY: e4a742da manual
// OFFSET: 0x007D9C80
void CMap::UpdateLightBounds(CMapLight* light) {
    float radius = light->m_attenEnd;

    if (light->m_attenDenom == 0.0f) {
        radius = 5.0f;
    }

    light->m_attenEnd = radius;

    C3Vector& pos = light->m_light.m_pos;
    light->m_position = pos;
    light->m_bbox.b = C3Vector(pos.x - radius, pos.y - radius, pos.z - radius);
    light->m_bbox.t = C3Vector(pos.x + radius, pos.y + radius, pos.z + radius);
    light->m_sphere.c = pos;
    light->m_sphere.r = radius;
}

// VERIFY: 66f3244f auto
// OFFSET: 0x007D9D50
void CMap::EnableLight(CMapLight* light) {
    STORM_ASSERT(light);
    light->m_flags = static_cast<MapObjFlag>(light->m_flags & ~MAPOBJ_FLAG_DISABLED);
    light->m_light.SetVisible(1);
}

// VERIFY: c0ee5cb9 manual
// OFFSET: 0x007D9D70
void CMap::DestroyLight(CMapLight* light) {
    STORM_ASSERT(light);

    CMapBaseObjLink* link = light->m_parentLinkList.Head();

    while (link) {
        CMapBaseObj* ref = link->ref;
        CMapBaseObjLink* next = light->m_parentLinkList.Next(link);

        if (ref) {
            if (ref->GetType() & CMapBaseObj::Type_Chunk) {
                static_cast<CMapChunk*>(ref)->UpdateLights();
            } else if (ref->GetType() & CMapBaseObj::Type_MapObjDefGroup) {
                static_cast<CMapObjDefGroup*>(ref)->UpdateLights();
            }
        }

        CMap::FreeBaseObjLink(link);
        link = next;
    }

    CMap::FreeLight(light);
}

// VERIFY: 8d2bb286 auto
// OFFSET: 0x007D9DE0
void CMap::LinkLightToChunks(CMapLight* light) {
    STORM_ASSERT(light);

    // Chunk-space bounds of the light: x runs along world y, y along world x (type name unknown).
    struct {
        float minx;
        float miny;
        float maxx;
        float maxy;
    } tLocation;

    tLocation.maxx = -(light->m_bbox.b.y - HALF_WORLD_SIZE);
    tLocation.maxy = -(light->m_bbox.b.x - HALF_WORLD_SIZE);
    tLocation.minx = -(light->m_bbox.t.y - HALF_WORLD_SIZE);
    tLocation.miny = -(light->m_bbox.t.x - HALF_WORLD_SIZE);
    STORM_ASSERT(tLocation.minx >= 0.0f && tLocation.miny >= 0.0f);
    STORM_ASSERT(tLocation.maxx < ((64*16)*((150.0f/36.0f)*8)) && tLocation.maxy < ((64*16)*((150.0f/36.0f)*8)));

    int32_t maxY = CMath::fint_(tLocation.maxx * OO_COORD_TO_CHUNK - 0.5f);
    int32_t maxX = CMath::fint_(tLocation.maxy * OO_COORD_TO_CHUNK - 0.5f);
    int32_t minY = CMath::fint_(tLocation.minx * OO_COORD_TO_CHUNK - 0.5f);
    int32_t minX = CMath::fint_(tLocation.miny * OO_COORD_TO_CHUNK - 0.5f);

    for (int32_t x = minX; x <= maxX; x++) {
        for (int32_t y = minY; y <= maxY; y++) {
            CMapArea* area = CMap::GetAreaForChunk(y, x);

            if (!area || area->asyncObject) {
                continue;
            }

            CMapChunk* chunk = area->GetChunk(y, x);

            if (!chunk) {
                continue;
            }

            CMapBaseObjLink* link = CMap::AllocBaseObjLink(light);
            link->ref = chunk;
            chunk->lightLinkList.LinkToTail(link);
            chunk->UpdateLights();
        }
    }
}

// VERIFY: 75a5e857 manual
// OFFSET: 0x007D9F90
void CMap::LinkLightToMapObjDefs(CMapLight* light) {
    STORM_ASSERT(light);

    CAaBox box;

    for (CMapObjDef* mapObjDef = CMap::mapObjDefHash.m_fulllist.Head(); mapObjDef; mapObjDef = CMap::mapObjDefHash.Next(mapObjDef)) {
        if (mapObjDef->m_flags & MAPOBJ_FLAG_DISABLED) {
            continue;
        }

        if (light->m_bbox.t.x >= mapObjDef->m_bbox.b.x && light->m_bbox.t.y >= mapObjDef->m_bbox.b.y && light->m_bbox.t.z >= mapObjDef->m_bbox.b.z
            && light->m_bbox.b.x <= mapObjDef->m_bbox.t.x && light->m_bbox.b.y <= mapObjDef->m_bbox.t.y && light->m_bbox.b.z <= mapObjDef->m_bbox.t.z) {
            CMapObj* mapObj = mapObjDef->mapObj;

            if (mapObj && mapObj->TestBounds(box)) {
                for (CMapBaseObjLink* link = mapObjDef->groupLinkList.Head(); link; link = mapObjDef->groupLinkList.Next(link)) {
                    CMapObjDefGroup* group = static_cast<CMapObjDefGroup*>(link->owner);

                    if (!(group->m_flags & MAPOBJ_FLAG_INTERIOR) && light->m_bbox.Intersects(group->aaBox)) {
                        CMapBaseObjLink* lightLink = CMap::AllocBaseObjLink(light);
                        lightLink->ref = group;
                        group->lightLinkList.LinkToTail(lightLink);
                        group->UpdateLights();
                    }
                }
            }
        }
    }
}

// VERIFY: fe97de56 manual
// OFFSET: 0x007DA100
void CMap::UpdateLight(CMapLight* light) {
    STORM_ASSERT(light);

    if (!light->m_parentLinkList.Head() && !light->m_light.m_type) {
        CMapBaseObjLink* link = CMap::AllocBaseObjLink(light);
        link->ref = nullptr;
        CMap::s_globalLightList.LinkToTail(link);
    }

    if (CMap::bActive && light->m_light.m_type) {
        CMapBaseObjLink* link = light->m_parentLinkList.Head();

        while (link) {
            CMapBaseObjLink* next = light->m_parentLinkList.Next(link);
            CMap::FreeBaseObjLink(link);
            link = next;
        }

        CMap::UpdateLightBounds(light);
        CMap::LinkLightToMapObjDefs(light);
        CMap::LinkLightToChunks(light);
    }
}
