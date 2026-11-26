#include "MapEntity.h"
#include "WoW/Source/WorldClient/WorldLightingConstants.h"
#include "Engine/Source/Tempest/C3Segment.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/AreaTableRec.h"
#include "WoW/Source/DB/Rec/WMOAreaTableRec.h"
#include "WoW/Source/WorldClient/MapLight.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/DayNight__DNInfo.h"
#include "WoW/Source/WorldClient/FrameTime.h"
#include <cmath>
#include <cstring>
#include <typeinfo>
#include "WoW/Source/WorldClient/MapObj.h"
#include "WoW/Source/WorldClient/MapObjDef.h"
#include "WoW/Source/WorldClient/MapObjGroup.h"
#include "WoW/Source/Ui/MinimapFrame.h"
#include "WoW/Source/WorldClient/CWorldMath.h"

static const float dirLightScaleRate = 3.3333333f;

// STATIC: 0x00ADEEBC
float CMapStaticEntity::s_characterAmbient = 1.0f;

// STATIC: 0x00AEEDF0
C3Vector CMapStaticEntity::s_interiorSunDir = C3Vector(-0.30822f, -0.30822f, -0.9f);

// VERIFY: 4c01a51b manual
// OFFSET: 0x007A0B50
CMapStaticEntity::CMapStaticEntity() {
    this->m_sphere.c = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_sphere.r = 0.0f;
    this->m_scale = 0.0f;
    this->m_staticFlags &= ~MAP_STATIC_FLAGS::MAP_STATIC_FLAG_CONSTRUCTOR_CLEAR_MASK;
    this->m_distanceToCamera = 0.0f;
    this->m_liquidHeight = 0.0f;
    this->model = nullptr;
    this->m_diffuseLightScale = 0.0f;
    this->m_queryTag = 0;
    this->m_shadowStamps[0] = 0;
    this->m_shadowStamps[1] = 0;
    this->m_shadowStamps[2] = 0;
    this->unused_002B = 0;
    this->m_fadeLevel = 0;
    this->visCenter = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_culled = CULL_UNTESTED;
}

// VERIFY: 67998dc9 manual
// OFFSET: 0x007A0CD0
void SMOFog::Fogs::Blend(const Fogs& fogs, float t) {
    for (int32_t i = 0; i < NUM_FOGS; i++) {
        this->m_fog[i].m_end = (fogs.m_fog[i].m_end - this->m_fog[i].m_end) * t + this->m_fog[i].m_end;
        this->m_fog[i].m_startScalar = t * (fogs.m_fog[i].m_startScalar - this->m_fog[i].m_startScalar) + this->m_fog[i].m_startScalar;
        this->m_fog[i].m_color = DayNight::InterpColor(this->m_fog[i].m_color, fogs.m_fog[i].m_color, t);
    }
}

// VERIFY: a41b0f80 manual
// OFFSET: 0x007A0ED0
FogQueue::FogQueue(const uint32_t* params) : CPriorityQ<FogQ>(params) {
}

// VERIFY: 307e1c8c manual
// OFFSET: 0x007A0F50
void FogQueue::Push(float distance, uint32_t fogIndex) {
    this->Grow();
    uint32_t index = this->Used() - 1;

    if (this->Used() != 1 && index != 1) {
        uint32_t parent;

        do {
            parent = index >> 1;

            if (distance < (*this)[parent].m_distance) {
                break;
            }

            (*this)[index] = (*this)[parent];
            index >>= 1;
        } while (parent > 1);
    }
    (*this)[index].m_distance = distance;
    (*this)[index].m_fogIndex = fogIndex;
}

// VERIFY: eeb1ad46 manual
// OFFSET: 0x007A0FE0
CMapEntity::CMapEntity() {
    this->unused_00A8 = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_func = nullptr;
    this->m_funcParam = nullptr;
    this->m_funcParam64 = 0;
    this->m_funcParam32 = 0;
    this->unused_00B4 = 0;
    this->m_groundType = 0;
    this->m_liquidType = 0;
    this->unused_00BE = 0;
    this->m_splitBodyBone = 0xFFFFFFFF;
    this->m_targetAmbientColor.b = 0xFF;
    this->m_targetAmbientColor.g = 0x00;
    this->m_targetAmbientColor.r = 0x00;
    this->m_targetAmbientColor.a = 0x00;
    this->m_targetDiffuseLightScale = 0.0f;
    this->AddType(CMapBaseObj::Type_Entity);
}

// VERIFY: 515c7062 manual
// OFFSET: 0x007A1150
int32_t CMapEntity::QueryCameraFog(SMOFog* fog, CMapObjDef** mapObjDef, uint8_t* found, TSGrowableArray<uint32_t>** groups, float* distance) {
    if (!CWorldScene::s_viewerMapObjDef) {
        return 0;
    }

    CMapObj* owner = CWorldScene::s_viewerMapObjDef->mapObj;

    if (!owner || owner->fogCount == 1) {
        return 0;
    }

    C3Vector localPos = CWorldScene::s_activeWorldView * CWorldScene::s_viewerMapObjDef->m_invMat;
    *found = 0;

    for (uint32_t i = 0; i < CWorldScene::s_viewerMapObjGroups.m_count; i++) {
        CMapObjGroup* group = owner->GetGroup(CWorldScene::s_viewerMapObjGroups.m_data[i], 0);

        if (group && !(group->m_flags & CMapObjGroup::MOGP_FLAGS::F_EXTERIOR_CLASSIFICATION_MASK)) {
            *found = 1;
            float portalDistance = owner->DistFromClosestExtPortal(group, localPos, 1);

            if (portalDistance < *distance) {
                *distance = portalDistance;
            }
        }
    }

    if (*found) {
        *mapObjDef = CWorldScene::s_viewerMapObjDef;
        *groups = &CWorldScene::s_viewerMapObjGroups;
    } else {
        *distance = 0.0f;
    }

    *fog = owner->GetFog(0);

    uint32_t params[2] = { 32, 32 };
    static FogQueue s_fogQueue(params);

    CMapObjGroup* group = owner->GetGroup(CWorldScene::s_viewerMapObjGroups.m_data[0], 0);

    if (group) {
        for (uint32_t i = 0; i < 4; i++) {
            uint8_t fogIndex = group->m_fogs[i];

            if (!fogIndex) {
                continue;
            }

            const SMOFog& groupFog = owner->GetFog(fogIndex);
            float dx = groupFog.m_position.x - localPos.x;
            float dy = groupFog.m_position.y - localPos.y;
            float dz = groupFog.m_position.z - localPos.z;
            float fogDistance = sqrt(dx * dx + dy * dy + dz * dz);

            if (fogDistance < groupFog.m_largerRadius && !(groupFog.m_flags & SMOFog::F_IEBLEND)) {
                s_fogQueue.Push(fogDistance, fogIndex);
            }
        }
    }

    while (!s_fogQueue.Empty()) {
        FogQ entry = s_fogQueue.Pop();
        const SMOFog& groupFog = owner->GetFog(entry.m_fogIndex);
        float clamped = entry.m_distance;

        if (clamped < 0.0f) {
            clamped = 0.0f;
        } else if (groupFog.m_largerRadius <= clamped) {
            clamped = groupFog.m_largerRadius;
        }

        float weight;

        if (clamped >= groupFog.m_smallerRadius) {
            weight = 1.0f - (clamped - groupFog.m_smallerRadius) / (groupFog.m_largerRadius - groupFog.m_smallerRadius);
        } else {
            weight = 1.0f;
        }

        fog->m_fogs.Blend(groupFog.m_fogs, weight);

        if (s_fogQueue.Used() == 1) {
            fog->m_flags = groupFog.m_flags;
        }
    }

    return 1;
}

// VERIFY: 3d0d7fa9 manual
// OFFSET: 0x007C1730
void CMapStaticEntity::SelectLights(CM2Lighting* lighting) {
    if (CWorld::farFog > this->m_distanceToCamera) {
        if (this->m_flags & MAPOBJ_FLAG_UNPLACED) {
            this->m_flags = static_cast<MapObjFlag>(this->m_flags & ~MAPOBJ_FLAG_UNPLACED);
        }

        C3Vector diffuse = { 0.0f, 0.0f, 0.0f };
        C3Vector dir = { 0.0f, 0.0f, 0.0f };
        C3Vector ambient = { 0.0f, 0.0f, 0.0f };

        if (this->m_flags & MAPOBJ_FLAG_EXTERIOR) {
            CM2Light* light = &CMap::s_mapLight->m_light;

            diffuse.x = light->m_dirColor.x * this->m_diffuseLightScale;
            diffuse.y = light->m_dirColor.y * this->m_diffuseLightScale;
            diffuse.z = light->m_dirColor.z * this->m_diffuseLightScale;

            dir = light->m_dir;

            ambient = C3Vector(this->m_m2AmbientColor);
        } else {
            diffuse = C3Vector(this->m_m2DiffuseColor);
            diffuse.x *= this->m_diffuseLightScale;
            diffuse.y *= this->m_diffuseLightScale;
            diffuse.z *= this->m_diffuseLightScale;

            ambient = C3Vector(this->m_m2AmbientColor);

            dir = CMapStaticEntity::s_interiorSunDir;

            if ((this->m_staticFlags & MAP_STATIC_FLAGS::MAP_STATIC_FLAG_BLEND_OUTDOOR_LIGHT) != 0) {
                DayNight::DNInfo* dayNight = DayNight::GetInfo();
                float blend = this->m_m2DiffuseColor.a / 255.0f;

                dir.x += (dayNight->m_light1.m_dir.x - dir.x) * blend;
                dir.y += (dayNight->m_light1.m_dir.y - dir.y) * blend;
                dir.z += (dayNight->m_light1.m_dir.z - dir.z) * blend;

                dir.Normalize();
            }
        }

        if ((this->m_staticFlags & MAP_STATIC_FLAGS::MAP_STATIC_FLAG_USE_CHARACTER_AMBIENT) != 0) {
            ambient.x *= CMapStaticEntity::s_characterAmbient;
            ambient.y *= CMapStaticEntity::s_characterAmbient;
            ambient.z *= CMapStaticEntity::s_characterAmbient;

            if (ambient.x >= 1.0f) {
                ambient.x = 1.0f;
            }

            if (ambient.y >= 1.0f) {
                ambient.y = 1.0f;
            }

            if (ambient.z >= 1.0f) {
                ambient.z = 1.0f;
            }
        }

        lighting->AddAmbient(ambient);
        lighting->AddDiffuse(diffuse, dir);
    }

    DayNight::DNInfo* dayNight = DayNight::GetInfo();

    if ((this->m_flags & MAPOBJ_FLAG_INT_FOG) != 0) {
        lighting->SetFog(C3Vector(dayNight->m_fogInterior.m_color), dayNight->m_fogInterior.m_start, dayNight->m_fogInterior.m_end, dayNight->m_fogInterior.m_density);
    } else {
        lighting->SetFog(C3Vector(dayNight->m_fog.m_color), dayNight->m_fog.m_start, dayNight->m_fog.m_end, dayNight->m_fog.m_density);
    }
}

// VERIFY: feea4d8f manual
// OFFSET: 0x0097E080
FogQ FogQueue::Pop() {
    FogQ top = (*this)[1];
    FogQ last = (*this)[this->Used() - 1];

    if (this->Used()) {
        this->Erase(this->Used() - 1, 1);
    }

    if (this->Used() < 2) {
        return top;
    }

    uint32_t limit = this->Used() - 1;
    uint32_t half = limit >> 1;
    uint32_t index = 1;

    if (half) {
        while (1) {
            uint32_t child = 2 * index;

            if (child < limit && (*this)[child].m_distance <= (*this)[child + 1].m_distance) {
                child++;
            }

            if (last.m_distance >= (*this)[child].m_distance) {
                break;
            }

            (*this)[index] = (*this)[child];
            index = child;

            if (child > half) {
                break;
            }
        }
    }

    (*this)[index] = last;

    return top;
}

// VERIFY: 5f34315e manual
// OFFSET: 0x007A1B10
void CMapEntity::QueryLiquidSounds(TSGrowableArray<World::LiquidQueryResult>& results, uint32_t* distance) {
    if (!(this->m_staticFlags & MAP_STATIC_FLAGS::MAP_STATIC_FLAG_INSIDE)) {
        return;
    }

    for (CMapBaseObjLink* link = this->m_parentLinkList.Head(); link; link = this->m_parentLinkList.Next(link)) {
        CMapBaseObj* ref = link->ref;

        if (!(ref->GetType() & CMapBaseObj::Type_MapObjDefGroup)) {
            continue;
        }

        CMapObjDefGroup* mapObjDefGroup = static_cast<CMapObjDefGroup*>(ref);
        CMapObjDef* mapObjDef = static_cast<CMapObjDef*>(ref->m_parentLinkList.Head()->ref);
        STORM_ASSERT(mapObjDef->GetType() & Type_MapObjDef);
        CMapObj* mapObj = mapObjDef->mapObj;
        STORM_ASSERT(mapObj);
        C3Vector position = this->m_position * mapObjDef->m_invMat;
        mapObj->QueryLiquidSounds(mapObjDefGroup->groupNum, mapObjDefGroup->groupNum, 0, distance, position, results);
    }
}

// VERIFY: 254daf94 manual
// OFFSET: 0x007A1E90
void CMapEntity::Tick() {
    int32_t step = static_cast<int32_t>((FrameTime::s_tickTimeSec + FrameTime::s_tickTimeSec) * 255.0f);

    if (step < 1) {
        step = 1;
    }

    int32_t r = this->m_m2AmbientColor.r;
    int32_t g = this->m_m2AmbientColor.g;
    int32_t b = this->m_m2AmbientColor.b;
    int32_t targetR = this->m_targetAmbientColor.r;
    int32_t targetG = this->m_targetAmbientColor.g;
    int32_t targetB = this->m_targetAmbientColor.b;
    BOOL changed = 0;

    if (targetR - r) {
        changed = 1;

        if (targetR - r <= 0) {
            r -= step;

            if (r < targetR) {
                r = targetR;
            }
        } else {
            r += step;

            if (r > targetR) {
                r = targetR;
            }
        }
    }

    if (targetG - g) {
        changed = 1;

        if (targetG - g <= 0) {
            g -= step;

            if (g < targetG) {
                g = targetG;
            }
        } else {
            g += step;

            if (g > targetG) {
                g = targetG;
            }
        }
    }

    if (targetB - b) {
        changed = 1;

        if (targetB - b <= 0) {
            b -= step;

            if (b < targetB) {
                b = targetB;
            }
        } else {
            b += step;

            if (b > targetB) {
                b = targetB;
            }
        }
    }

    this->m_m2AmbientColor.r = static_cast<uint8_t>(r);
    this->m_m2AmbientColor.g = static_cast<uint8_t>(g);
    this->m_m2AmbientColor.b = static_cast<uint8_t>(b);

    if (!(this->m_staticFlags & MAP_STATIC_FLAGS::MAP_STATIC_FLAG_INSIDE) && !changed) {
        CImVector ambient = DayNight::GetInfo()->m_light1.m_ambient;
        this->m_m2AmbientColor = ambient;
        this->m_targetAmbientColor = ambient;
    }

    float rate = FrameTime::s_tickTimeSec * dirLightScaleRate;
    float delta = this->m_diffuseLightScale - this->m_targetDiffuseLightScale;

    if (0.0f != delta) {
        if (delta >= 0.0f) {
            this->m_diffuseLightScale = this->m_diffuseLightScale - rate;

            if (this->m_diffuseLightScale < this->m_targetDiffuseLightScale) {
                this->m_diffuseLightScale = this->m_targetDiffuseLightScale;
            }
        } else {
            this->m_diffuseLightScale = rate + this->m_diffuseLightScale;

            if (this->m_diffuseLightScale > this->m_targetDiffuseLightScale) {
                this->m_diffuseLightScale = this->m_targetDiffuseLightScale;
            }
        }
    }

    if (this->m_diffuseLightScale > 1.0f) {
        this->m_diffuseLightScale = 1.0f;
    }

    if (!this->m_parentLinkList.Head()) {
        CMap::UpdateEntity(this);
    }
}

// VERIFY: 6acda3f1 manual
// OFFSET: 0x007A13E0
bool CMapStaticEntity::GetMapObjAndGroup(CMapObjDef*& mapObjDef, CMapObj*& mapObj, CMapObjDefGroup*& mapObjDefGroup, CMapObjGroup*& mapObjGroup, int32_t skipHidden) {
    for (CMapBaseObjLink* link = this->m_parentLinkList.Head(); link; link = this->m_parentLinkList.Next(link)) {
        CMapBaseObj* ref = link->ref;

        if (!(ref->GetType() & CMapBaseObj::Type_MapObjDefGroup)) {
            continue;
        }

        mapObjDefGroup = static_cast<CMapObjDefGroup*>(ref);
        mapObjDef = static_cast<CMapObjDef*>(ref->m_parentLinkList.Head()->ref);
        STORM_ASSERT(mapObjDef->GetType() & Type_MapObjDef);

        if (skipHidden && (mapObjDef->m_flags & MAPOBJ_FLAG_NO_DEPTH_SORT)) {
            continue;
        }

        mapObj = mapObjDef->mapObj;
        STORM_ASSERT(mapObj);
        mapObjGroup = mapObj->GetGroup(mapObjDefGroup->groupNum, 0);

        if (mapObjGroup) {
            return true;
        }
    }

    return false;
}

// VERIFY: d211d73c auto
// OFFSET: 0x007A1A30
bool CMapEntity::UpdateMapObjLiquid() {
    this->m_staticFlags &= ~MAP_STATIC_FLAGS::MAP_STATIC_FLAG_LIQUID_STATE_MASK;
    CMapObjDef* mapObjDef;
    CMapObj* mapObj;
    CMapObjDefGroup* mapObjDefGroup;
    CMapObjGroup* mapObjGroup;

    if (!this->GetMapObjAndGroup(mapObjDef, mapObj, mapObjDefGroup, mapObjGroup, 0)) {
        return false;
    }

    C3Vector local = C3Vector(this->m_position.x, this->m_position.y, this->m_bbox.b.z) * mapObjDef->m_invMat;
    int32_t lqWhich;
    float level;

    if (!mapObjGroup->QueryLiquid(local, &lqWhich, &level)) {
        return false;
    }

    this->m_staticFlags |= MAP_STATIC_FLAGS::MAP_STATIC_FLAG_IN_LIQUID;
    C3Vector world = C3Vector(local.x, local.y, level) * mapObjDef->m_mat;
    this->m_liquidHeight = world.z;

    if (world.z <= this->m_bbox.t.z) {
        this->m_staticFlags |= MAP_STATIC_FLAGS::MAP_STATIC_FLAG_LIQUID_SURFACE_CLIP;
    } else {
        this->m_staticFlags &= ~MAP_STATIC_FLAGS::MAP_STATIC_FLAG_LIQUID_SURFACE_CLIP;
    }

    STORM_ASSERT(static_cast<uint32_t>(lqWhich) < 65536); // 12213: "lqWhich < 65536"
    this->m_liquidType = static_cast<uint16_t>(lqWhich);
    return true;
}

// VERIFY: fb6c1e22 manual
// OFFSET: 0x007A0D60
bool CMapEntity::QueryInteriorLighting(CMapObjDef* mapObjDef, int32_t groupNum, uint16_t* faceIndex, C3Vector* pos) {
    CImVector color;
    *reinterpret_cast<uint32_t*>(&color) = 0;
    uint8_t flag;
    bool result;

    if (faceIndex) {
        C3Vector local = *pos * mapObjDef->m_invMat;
        result = mapObjDef->mapObj->QueryLighting(groupNum, local, *faceIndex, &color, &flag);
    } else {
        C3Vector local = this->m_position * mapObjDef->m_invMat;
        C3Segment seg;
        seg.start = C3Vector(local.x, local.y, local.z + 1.0f);
        seg.end = C3Vector(local.x, local.y, local.z - 12.0f);
        result = mapObjDef->mapObj->QueryLighting(groupNum, seg, &color, &flag);
    }

    if (!result) {
        return result;
    }

    CImVector limit;
    *reinterpret_cast<uint32_t*>(&limit) = MIN_INTERIOR_DIRCOLOR;
    this->AdjustLightmap(&color, &this->m_m2DiffuseColor, limit, &this->m_targetAmbientColor, MAX_AMBIENT);

    if (!flag) {
        this->m_staticFlags &= ~MAP_STATIC_FLAGS::MAP_STATIC_FLAG_BLEND_OUTDOOR_LIGHT;
        return result;
    }

    DayNight::DNInfo* info = DayNight::GetInfo();
    uint8_t alpha = color.a;

    if (alpha) {
        this->m_m2DiffuseColor.InterpColor(alpha, &info->m_light1.m_diffuse);
        this->m_targetAmbientColor.InterpColor(alpha, &info->m_light1.m_ambient);
    }

    this->m_staticFlags |= MAP_STATIC_FLAGS::MAP_STATIC_FLAG_BLEND_OUTDOOR_LIGHT;
    this->m_m2DiffuseColor.a = color.a;
    return result;
}

// VERIFY: 8dca5fb5 auto
// OFFSET: 0x007A1500
int32_t CMapEntity::QueryMapObjZoneName(const char** zoneName) {
    CMapObjDef* mapObjDef;
    CMapObj* mapObj;
    CMapObjDefGroup* mapObjDefGroup;
    CMapObjGroup* mapObjGroup;

    if (!this->GetMapObjAndGroup(mapObjDef, mapObj, mapObjDefGroup, mapObjGroup, 1)) {
        return 0;
    }

    *zoneName = "";
    WMOAreaTableRec* rec = GetWMOAreaRec(mapObjDef->mapObj->m_header->m_wmoID, mapObjDef->m_nameSet, -1);

    if (rec) {
        const char* name = rec->m_areaName;

        if (!*name) {
            int32_t areaId = rec->m_areaTableID;

            if (!areaId) {
                areaId = CMap::QueryAreaIdTerrain(&this->m_position);

                if (!areaId) {
                    return 1;
                }
            }

            AreaTableRec* area = g_areaTableDB.GetRecord(areaId);

            if (!area) {
                return 1;
            }

            name = area->m_areaName;
        }

        *zoneName = name;
    }

    return 1;
}

// VERIFY: 384f6f8b auto
// OFFSET: 0x007A15B0
int32_t CMapEntity::QueryMapObjSubzoneName(const char** subzoneName, int32_t* areaId) {
    CMapObjDef* mapObjDef;
    CMapObj* mapObj;
    CMapObjDefGroup* mapObjDefGroup;
    CMapObjGroup* mapObjGroup;

    if (!this->GetMapObjAndGroup(mapObjDef, mapObj, mapObjDefGroup, mapObjGroup, 1)) {
        return 0;
    }

    if (!mapObjDefGroup->m_subzoneName) {
        WMOAreaTableRec* rec = GetWMOAreaRec(mapObj->m_header->m_wmoID, mapObjDef->m_nameSet, mapObjGroup->m_wmoGroupId);
        mapObjDefGroup->m_subzoneName = rec ? rec->m_areaName : "";
    }

    *subzoneName = mapObjDefGroup->m_subzoneName;

    if (mapObjDefGroup->groupNum) {
        *areaId = mapObjDefGroup->groupNum;
    }

    return 1;
}

// VERIFY: feac96db manual
// OFFSET: 0x007A1640
bool CMapEntity::QueryMapObjAreaTable(WMOAreaTableRec** groupRec, WMOAreaTableRec** rootRec, int32_t* wmoGroupId) {
    for (CMapBaseObjLink* link = this->m_parentLinkList.Head(); link; link = this->m_parentLinkList.Next(link)) {
        CMapBaseObj* ref = link->ref;

        if (ref->GetType() & CMapBaseObj::Type_Chunk) {
            break;
        }

        CMapObjDefGroup* mapObjDefGroup = static_cast<CMapObjDefGroup*>(ref);
        CMapObjDef* mapObjDef = static_cast<CMapObjDef*>(ref->m_parentLinkList.Head()->ref);
        STORM_ASSERT(mapObjDef->GetType() & Type_MapObjDef);
        CMapObj* mapObj = mapObjDef->mapObj;
        STORM_ASSERT(mapObj);
        CMapObjGroup* group = mapObj->GetGroup(mapObjDefGroup->groupNum, 0);

        if (group && (!(group->m_flags & CMapObjGroup::MOGP_FLAGS::F_EXTERIOR) || !(mapObjDef->m_flags & MAPOBJ_FLAG_NO_DEPTH_SORT))) {
            *wmoGroupId = group->m_wmoGroupId;
            *groupRec = GetWMOAreaRec(mapObj->m_header->m_wmoID, mapObjDef->m_nameSet, group->m_wmoGroupId);
            WMOAreaTableRec* rec = GetWMOAreaRec(mapObj->m_header->m_wmoID, mapObjDef->m_nameSet, -1);
            *rootRec = rec;
            return *groupRec && rec;
        }
    }

    return false;
}

// VERIFY: 0b2f6b2e manual
// OFFSET: 0x007A18D0
bool CMapEntity::QueryMapObjIDs(int32_t* wmoId, int32_t* wmoHash, int32_t* groupNum) {
    for (CMapBaseObjLink* link = this->m_parentLinkList.Head(); link; link = this->m_parentLinkList.Next(link)) {
        CMapBaseObj* ref = link->ref;

        if (ref->GetType() & CMapBaseObj::Type_Chunk) {
            break;
        }

        CMapObjDefGroup* mapObjDefGroup = static_cast<CMapObjDefGroup*>(ref);
        CMapObjDef* mapObjDef = static_cast<CMapObjDef*>(ref->m_parentLinkList.Head()->ref);
        STORM_ASSERT(mapObjDef->GetType() & Type_MapObjDef);
        CMapObj* mapObj = mapObjDef->mapObj;
        STORM_ASSERT(mapObj);
        uint8_t groupFlags = static_cast<uint8_t>(mapObj->GetGroupFlags(mapObjDefGroup->groupNum));

        if (!(mapObjDef->m_flags & MAPOBJ_FLAG_NO_DEPTH_SORT) || !(groupFlags & CMapObjGroup::MOGP_FLAGS::F_EXTERIOR)) {
            *wmoId = mapObj->m_header->m_wmoID;
            *wmoHash = mapObjDef->mapObj->m_hashval;
            *groupNum = mapObjDefGroup->groupNum;
            return true;
        }
    }

    return false;
}

// VERIFY: 2b02ba04 manual
// OFFSET: 0x007A1480
int32_t CMapEntity::QueryInterior() {
    for (CMapBaseObjLink* link = this->m_parentLinkList.Head(); link; link = this->m_parentLinkList.Next(link)) {
        CMapBaseObj* ref = link->ref;

        if (ref->GetType() & CMapBaseObj::Type_Chunk) {
            break;
        }

        CMapObjDefGroup* mapObjDefGroup = static_cast<CMapObjDefGroup*>(ref);
        CMapObjDef* mapObjDef = static_cast<CMapObjDef*>(ref->m_parentLinkList.Head()->ref);
        STORM_ASSERT(mapObjDef->GetType() & Type_MapObjDef);
        CMapObj* mapObj = mapObjDef->mapObj;
        STORM_ASSERT(mapObj);
        CMapObjGroup* group = mapObj->GetGroup(mapObjDefGroup->groupNum, 0);

        if (group) {
            if (!(group->m_flags & CMapObjGroup::MOGP_FLAGS::F_EXTERIOR)) {
                return 1;
            }

            if (!(mapObjDef->m_flags & MAPOBJ_FLAG_NO_DEPTH_SORT)) {
                break;
            }
        }
    }

    return 0;
}

// VERIFY: 4d2de931 manual
// OFFSET: 0x007A1730
int32_t CMapEntity::QueryMapObjFileName(const char** fileName) {
    for (CMapBaseObjLink* link = this->m_parentLinkList.Head(); link; link = this->m_parentLinkList.Next(link)) {
        CMapBaseObj* ref = link->ref;

        if (ref->GetType() & CMapBaseObj::Type_Chunk) {
            *fileName = nullptr;
            return 0;
        }

        CMapObjDefGroup* mapObjDefGroup = static_cast<CMapObjDefGroup*>(ref);
        CMapObjDef* mapObjDef = static_cast<CMapObjDef*>(ref->m_parentLinkList.Head()->ref);
        STORM_ASSERT(mapObjDef->GetType() & Type_MapObjDef);
        CMapObj* mapObj = mapObjDef->mapObj;
        STORM_ASSERT(mapObj);
        CMapObjGroup* group = mapObj->GetGroup(mapObjDefGroup->groupNum, 0);

        if (group && (!(group->m_flags & CMapObjGroup::MOGP_FLAGS::F_EXTERIOR) || !(mapObjDef->m_flags & MAPOBJ_FLAG_NO_DEPTH_SORT))) {
            *fileName = mapObj->m_wmoName;
            return 1;
        }
    }

    return 0;
}

// VERIFY: c065df6f manual
// OFFSET: 0x007A17E0
bool CMapEntity::QueryMapObjMinimap(CAaBox* box, TSStackArray<MINIMAPQUADINFO>* quads, uint32_t maxQuads, int32_t inside, uint8_t indoorArea) {
    for (CMapBaseObjLink* link = this->m_parentLinkList.Head(); link; link = this->m_parentLinkList.Next(link)) {
        CMapBaseObj* ref = link->ref;

        if (ref->GetType() & CMapBaseObj::Type_Chunk) {
            return false;
        }

        CMapObjDefGroup* mapObjDefGroup = static_cast<CMapObjDefGroup*>(ref);
        CMapObjDef* mapObjDef = static_cast<CMapObjDef*>(ref->m_parentLinkList.Head()->ref);
        STORM_ASSERT(mapObjDef->GetType() & Type_MapObjDef);
        CMapObj* mapObj = mapObjDef->mapObj;
        STORM_ASSERT(mapObj);
        uint8_t groupFlags = static_cast<uint8_t>(mapObj->GetGroupFlags(mapObjDefGroup->groupNum));

        if (!(mapObjDef->m_flags & MAPOBJ_FLAG_NO_DEPTH_SORT) || !(groupFlags & CMapObjGroup::MOGP_FLAGS::F_EXTERIOR)) {
            if (!(groupFlags & CMapObjGroup::MOGP_FLAGS::F_EXTERIOR_CLASSIFICATION_MASK)) {
                indoorArea = 0;
            }

            CAaBox local;
            local.b = C3Vector(0.0f, 0.0f, 0.0f);
            local.t = C3Vector(0.0f, 0.0f, 0.0f);
            CWorldMath::TransformAABox(mapObjDef->m_invMat, *box, local);
            return mapObj->QueryMapObjMinimap(mapObjDefGroup->groupNum, &local, quads, maxQuads, inside, indoorArea);
        }
    }

    return false;
}

// VERIFY: b0f36f16 manual
// OFFSET: 0x007A1980
bool CMapEntity::QueryMapObjMatrix(C44Matrix* matrix, C44Matrix* invMatrix) {
    for (CMapBaseObjLink* link = this->m_parentLinkList.Head(); link; link = this->m_parentLinkList.Next(link)) {
        CMapBaseObj* ref = link->ref;

        if (ref->GetType() & CMapBaseObj::Type_Chunk) {
            return false;
        }

        CMapObjDefGroup* mapObjDefGroup = static_cast<CMapObjDefGroup*>(ref);
        CMapObjDef* mapObjDef = static_cast<CMapObjDef*>(ref->m_parentLinkList.Head()->ref);
        STORM_ASSERT(mapObjDef->GetType() & Type_MapObjDef);
        CMapObj* mapObj = mapObjDef->mapObj;
        STORM_ASSERT(mapObj);

        if (!(mapObjDef->m_flags & MAPOBJ_FLAG_NO_DEPTH_SORT)) {
            if (matrix) {
                *matrix = mapObjDef->m_mat;
            }

            if (invMatrix) {
                *invMatrix = mapObjDef->m_invMat;
            }

            return true;
        }

        CMapObjGroup* group = mapObj->GetGroup(mapObjDefGroup->groupNum, 0);

        if (group && !(group->m_flags & CMapObjGroup::MOGP_FLAGS::F_EXTERIOR)) {
            if (matrix) {
                *matrix = mapObjDef->m_mat;
            }

            if (invMatrix) {
                *invMatrix = mapObjDef->m_invMat;
            }

            return true;
        }
    }

    return false;
}

// VERIFY: 0284b305 manual
// OFFSET: [0x007A10A0, 0x007A1120]
CMapEntity::~CMapEntity() {
    STORM_ASSERT(m_refCount == 0);
}
