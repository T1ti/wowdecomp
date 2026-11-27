#include "MapObjDef.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/LiquidSystem/Liquid.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapLight.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/CWorldMath.h"
#include "WoW/Source/WorldClient/DayNight__DNInfo.h"
#include "WoW/Source/WorldClient/MapObj.h"
#include "WoW/Source/WorldClient/MapObjGroup.h"
#include "WoW/Source/WorldClient/Types.h"
#include "WoW/Source/WorldClient/MapDoodadDef.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Model2/M2Model.h"
#include "WoW/Source/WorldClient/MapLoad.h"
#include "Engine/Source/Services/ParticleSystem2.h"

// VERIFY: 18be6c93 auto
// OFFSET: 0x007B3990
bool CMapObjDef::TestAABox(C3Vector& start, C3Vector& end) {
    return CWorldMath::VectorIntersectAABox2(this->m_bbox, start, end);
}

// VERIFY: 62f4837e auto
// OFFSET: 0x007B3DE0
CMapObjDefGroup::CMapObjDefGroup() {
    this->m_sphere.r = 0.0f;
    this->m_ambientColor = 0;
    this->doodadDefLinkList.SetLinkOffset(offsetof(CMapBaseObjLink, m_refLink));
    this->entityLinkList.SetLinkOffset(offsetof(CMapBaseObjLink, m_refLink));
    this->lightLinkList.SetLinkOffset(offsetof(CMapBaseObjLink, m_refLink));
    this->m_unkLinkList.SetLinkOffset(offsetof(CMapBaseObjLink, m_refLink));
    this->AddType(CMapBaseObj::Type_MapObjDefGroup);
    this->m_flags = static_cast<MapObjFlag>(this->m_flags | MAPOBJ_FLAG_UNPLACED);
    this->groupNum = -1;
    this->m_unkFlags = -1;
    this->unused_0058 = 0;
    this->unused_0064 = 0;
    this->m_subzoneName = nullptr;
    this->m_liquidInstance = nullptr;
}

// VERIFY: 7ed1c1b6 manual
// OFFSET: 0x007B3F30
void CMapObjDefGroup::SelectLights(CM2Lighting* lighting) {
    lighting->AddLight(&CMap::s_mapLight->m_light);

    if (CWorld::farFog > this->m_distanceToCamera) {
        if (this->m_flags & MAPOBJ_FLAG_UNPLACED) {
            this->m_flags = static_cast<MapObjFlag>(this->m_flags & ~MAPOBJ_FLAG_UNPLACED);
        }

        for (CMapBaseObjLink* link = this->lightLinkList.Head(); link; link = this->lightLinkList.Next(link)) {
            CMapLight* light = static_cast<CMapLight*>(link->owner);

            if (light->m_light.m_visible && !light->m_excludeFromMapObjLighting) {
                lighting->AddLight(&light->m_light);
            }
        }
    }

    DayNight::DNInfo* dayNight = DayNight::GetInfo();

    if (this->m_flags & MAPOBJ_FLAG_INT_FOG) {
        lighting->SetFog(C3Vector(dayNight->m_fogInterior.m_color), dayNight->m_fogInterior.m_start, dayNight->m_fogInterior.m_end, dayNight->m_fogInterior.m_density);
    } else {
        lighting->SetFog(C3Vector(dayNight->m_fog.m_color), dayNight->m_fog.m_start, dayNight->m_fog.m_end, dayNight->m_fog.m_density);
    }
}

// VERIFY: 05a4df33 manual
// OFFSET: 0x007B4350
CMapObjDef::CMapObjDef() {
    this->m_sphere.c = C3Vector();
    this->m_sphere.r = 0.0f;
    this->m_argbColor = 0;
    this->m_nameId = 0;
    this->mapObj = nullptr;
    this->unused_00F8 = 0;
    this->AddType(CMapBaseObj::Type_MapObjDef);
    this->m_doodadSet = 0;
    this->m_nameSet = 0;
    this->unused_0108 = 0;
    this->m_objectGuid = WOWGUID();
    this->m_unkFlags = 0xFFFF;
}

// VERIFY: 3855c196 manual
// OFFSET: [0x007B4500, 0x007B47C0]
CMapObjDef::~CMapObjDef() {
    STORM_ASSERT(m_refCount == 0);
    STORM_ASSERT(m_visLink.IsLinked() == 0);
}

// VERIFY: 3113f3c8 manual
// OFFSET: 0x007B39B0
int32_t CMapObjDef::GetGroundType(uint32_t groupNum, uint16_t faceIndex) {
    CMapObjGroup* group = this->mapObj->GetGroup(groupNum, 0);

    if (group && faceIndex < group->m_polyListSize) {
        SMOPoly* poly = &group->m_polyList[faceIndex];

        if (poly) {
            return this->mapObj->GetMaterial(poly->m_materialId)->m_groundType;
        }
    }

    return -1;
}

// VERIFY: 14be0e9a manual
// OFFSET: 0x007B66E0
void CMapObjDef::UpdatePos(CMapObjDef* mapObjDef, const C3Vector* position, float facing, float pitch, float roll) {
    mapObjDef->m_position = *position;
    mapObjDef->m_mat = C44Matrix();
    mapObjDef->m_mat.d0 = position->x;
    mapObjDef->m_mat.d1 = position->y;
    mapObjDef->m_mat.d2 = position->z;
    mapObjDef->m_mat.RotateAroundZ(facing);
    mapObjDef->m_mat.RotateAroundY(pitch);
    mapObjDef->m_mat.RotateAroundX(roll);
    mapObjDef->m_invMat = mapObjDef->m_mat.AffineInverse();
    CMapObjDef::UpdateMoved(mapObjDef);
}

// VERIFY: 70a9844c manual
// OFFSET: 0x007B41F0
void CMapObjDefGroup::SetSequenceFinishedCallback(M2SequenceCallback callback, uint64_t param, uint16_t doodadSet) {
    for (CMapBaseObjLink* link = this->doodadDefLinkList.Head(); link; link = this->doodadDefLinkList.Next(link)) {
        CMapDoodadDef* doodadDef = static_cast<CMapDoodadDef*>(link->owner);

        if (doodadSet && !doodadDef->wmoSpecialDoodadSet) {
            break;
        }

        if (doodadDef->model && doodadDef->wmoSpecialDoodadSet == doodadSet) {
            doodadDef->model->SetSequenceCallback(callback, param);
        }
    }
}

// VERIFY: 5ecf6841 manual
// OFFSET: 0x007B4640
void CMapObjDef::SetSequenceFinishedCallback(M2SequenceCallback callback, uint64_t param, uint16_t doodadSet) {
    for (CMapBaseObjLink* link = this->groupLinkList.Head(); link; link = this->groupLinkList.Next(link)) {
        static_cast<CMapObjDefGroup*>(link->owner)->SetSequenceFinishedCallback(callback, param, doodadSet);
    }
}

// VERIFY: a5601f23 manual
// OFFSET: 0x007B67B0
void CMapObjDef::UpdateMatrix(CMapObjDef* mapObjDef, const C44Matrix& matrix) {
    mapObjDef->m_position.x = matrix.d0;
    mapObjDef->m_position.y = matrix.d1;
    mapObjDef->m_position.z = matrix.d2;
    mapObjDef->m_mat = matrix;
    mapObjDef->m_invMat = matrix.AffineInverse();
    CMapObjDef::UpdateMoved(mapObjDef);
}

// VERIFY: 8b83ad81 manual
// OFFSET: 0x007B4270
void CMapObjDefGroup::SetEventCallback(M2EventCallback callback, uint64_t param, uint16_t doodadSet) {
    for (CMapBaseObjLink* link = this->doodadDefLinkList.Head(); link; link = this->doodadDefLinkList.Next(link)) {
        CMapDoodadDef* doodadDef = static_cast<CMapDoodadDef*>(link->owner);

        if (doodadSet && !doodadDef->wmoSpecialDoodadSet) {
            break;
        }

        if (doodadDef->model && doodadDef->wmoSpecialDoodadSet == doodadSet) {
            doodadDef->model->SetEventCallback(callback, param);
        }
    }
}

// VERIFY: 04ab0ef9 manual
// OFFSET: 0x007B42F0
bool CMapObjDefGroup::IsAllLoaded() {
    if (!(this->m_flags & MAPOBJ_FLAG_REFS_CREATED)) {
        return false;
    }

    for (CMapBaseObjLink* link = this->doodadDefLinkList.Head(); link; link = this->doodadDefLinkList.Next(link)) {
        CMapStaticEntity* doodadDef = static_cast<CMapStaticEntity*>(link->owner);

        if (doodadDef->model && !(doodadDef->m_flags & MAPOBJ_FLAG_PREPARED)) {
            if (SFile::IsStreamingMode()) {
                doodadDef->model->LinkToAsyncHead(1);
            }

            return false;
        }
    }

    return true;
}

// VERIFY: 918f4e3d manual
// OFFSET: 0x007B46A0
void CMapObjDef::SetEventCallback(M2EventCallback callback, uint64_t param, uint16_t doodadSet) {
    for (CMapBaseObjLink* link = this->groupLinkList.Head(); link; link = this->groupLinkList.Next(link)) {
        static_cast<CMapObjDefGroup*>(link->owner)->SetEventCallback(callback, param, doodadSet);
    }
}

// VERIFY: c4f26801 manual
// OFFSET: 0x007B4760
bool CMapObjDef::IsAllLoaded() {
    if (!(this->m_flags & MAPOBJ_FLAG_PREPARED)) {
        return false;
    }

    if (!(this->m_flags & MAPOBJ_FLAG_ALL_LOADED)) {
        for (CMapBaseObjLink* link = this->groupLinkList.Head(); link; link = this->groupLinkList.Next(link)) {
            if (!static_cast<CMapObjDefGroup*>(link->owner)->IsAllLoaded()) {
                return false;
            }
        }

        this->m_flags = static_cast<MapObjFlag>(this->m_flags | MAPOBJ_FLAG_ALL_LOADED);
    }

    return true;
}

// VERIFY: 18569df0 manual
// OFFSET: 0x007B40F0
void CMapObjDefGroup::Update(const C44Matrix& matrix) {
    this->m_flags = static_cast<MapObjFlag>(this->m_flags | MAPOBJ_FLAG_UNPLACED);

    for (CMapBaseObjLink* link = this->doodadDefLinkList.Head(); link; link = this->doodadDefLinkList.Next(link)) {
        static_cast<CMapDoodadDef*>(link->owner)->Update(matrix);
    }

    for (CMapBaseObjLink* link = this->entityLinkList.Head(); link; link = this->entityLinkList.Next(link)) {
        link->owner->m_flags = static_cast<MapObjFlag>(link->owner->m_flags | MAPOBJ_FLAG_UNPLACED);
    }
}

// VERIFY: 5e0dde28 manual
// OFFSET: 0x007B4170
void CMapObjDefGroup::SetSequence(uint32_t sequence, int32_t unknown3, uint16_t doodadSet) {
    for (CMapBaseObjLink* link = this->doodadDefLinkList.Head(); link; link = this->doodadDefLinkList.Next(link)) {
        CMapDoodadDef* doodadDef = static_cast<CMapDoodadDef*>(link->owner);

        if (doodadSet && !doodadDef->wmoSpecialDoodadSet) {
            break;
        }

        if (doodadDef->model && doodadDef->wmoSpecialDoodadSet == doodadSet) {
            doodadDef->model->SetBoneSequence(M2_KEY_BONE_NONE, sequence, M2_RANDOM_VARIATION, 0, 1.0f, 1, 1);
        }
    }
}

// VERIFY: b408ed84 manual
// OFFSET: 0x007B45F0
void CMapObjDef::SetSequence(uint32_t sequence, int32_t unknown3, uint16_t doodadSet) {
    for (CMapBaseObjLink* link = this->groupLinkList.Head(); link; link = this->groupLinkList.Next(link)) {
        static_cast<CMapObjDefGroup*>(link->owner)->SetSequence(sequence, unknown3, doodadSet);
    }
}

// VERIFY: c20df2dc manual
// OFFSET: 0x007B64F0
void CMapObjDef::UpdateMoved(CMapObjDef* mapObjDef) {
    mapObjDef->m_flags = static_cast<MapObjFlag>(mapObjDef->m_flags | MAPOBJ_FLAG_NO_DEPTH_SORT);
    CMapObj* owner = mapObjDef->mapObj;

    if (!owner->m_isGroupLoaded) {
        mapObjDef->m_sphere.r = 0.0f;
        mapObjDef->m_sphere.c = mapObjDef->m_position;
        mapObjDef->m_bbox.t = mapObjDef->m_position;
        mapObjDef->m_bbox.b = mapObjDef->m_position;
        return;
    }

    CAaBox box;
    box.b = C3Vector(0.0f, 0.0f, 0.0f);
    box.t = C3Vector(0.0f, 0.0f, 0.0f);
    owner->GetBounds(mapObjDef->m_sphere);
    mapObjDef->m_sphere.c *= mapObjDef->m_mat;
    owner->GetBounds(&box);
    CWorldMath::TransformAABox(mapObjDef->m_mat, box, mapObjDef->m_bbox);

    for (CMapBaseObjLink* link = mapObjDef->groupLinkList.Head(); link; link = mapObjDef->groupLinkList.Next(link)) {
        CMapObjDefGroup* defGroup = static_cast<CMapObjDefGroup*>(link->owner);
        CMapObjGroup* group = owner->GetGroup(defGroup->groupNum, 0);

        if (group) {
            for (int32_t i = 0; i < group->m_lightRefListCount; i++) {
                uint16_t lightIndex = group->m_lightRefList[i];
                CMapLight* light = mapObjDef->m_lightsArray.m_data[lightIndex];

                if (light) {
                    light->m_light.SetPosition(owner->m_lightList[lightIndex].m_position * mapObjDef->m_mat);

                    if (!(defGroup->m_flags & MAPOBJ_FLAG_INTERIOR)) {
                        CMap::UpdateLight(light);
                    }
                }
            }
        }

        CAaBox groupBox;
        groupBox.b = C3Vector(0.0f, 0.0f, 0.0f);
        groupBox.t = C3Vector(0.0f, 0.0f, 0.0f);
        owner->GetGroupBounds(&defGroup->m_sphere, defGroup->groupNum);
        defGroup->m_sphere.c *= mapObjDef->m_mat;
        owner->GetGroupBounds(&groupBox, defGroup->groupNum);
        CWorldMath::TransformAABox(mapObjDef->m_mat, groupBox, defGroup->aaBox);
        defGroup->Update(mapObjDef->m_mat);
    }
}

// VERIFY: cfdd9f32 manual
// OFFSET: 0x007B6800
void CMapObjDef::LinkDoodadDef(CMapDoodadDef* doodadDef, CMapObjDef* mapObjDef, int32_t testBounds) {
    CMapObj* mapObj = mapObjDef->mapObj;
    STORM_ASSERT(mapObj);
    STORM_ASSERT(doodadDef->wmoSpecialDoodadSet != 0);

    for (CMapBaseObjLink* link = mapObjDef->groupLinkList.Head(); link; link = mapObjDef->groupLinkList.Next(link)) {
        CMapObjDefGroup* group = static_cast<CMapObjDefGroup*>(link->owner);

        if (mapObj->GetGroupFlags(group->groupNum) & (CMapObjGroup::MOGP_FLAGS::F_ALWAYS_DRAW | CMapObjGroup::MOGP_FLAGS::F_UNKNOWN_0x400000 | CMapObjGroup::MOGP_FLAGS::F_UNREACHABLE)) {
            continue;
        }

        if (testBounds && !doodadDef->m_bbox.Intersects(group->aaBox)) {
            continue;
        }

        CMapBaseObjLink* doodadLink = CMap::AllocBaseObjLink(doodadDef);
        doodadLink->ref = group;
        group->doodadDefLinkList.LinkToHead(doodadLink);
        CMapDoodadDef::ExtendBounds(doodadDef, group);
    }
}

// VERIFY: 4eadefc9 manual
// OFFSET: 0x007B68A0
int32_t CMapObjDef::MoveDoodadDefs(CMapObjDef* fromMapObjDef, uint16_t doodadSet, CMapObjDef* toMapObjDef, uint16_t targetDoodadSet) {
    STORM_ASSERT(fromMapObjDef != toMapObjDef);

    if (!(toMapObjDef->m_flags & MAPOBJ_FLAG_PREPARED) || !toMapObjDef->groupLinkList.Head()) {
        return 0;
    }

    for (CMapBaseObjLink* link = fromMapObjDef->groupLinkList.Head(); link; link = fromMapObjDef->groupLinkList.Next(link)) {
        CMapObjDefGroup* group = static_cast<CMapObjDefGroup*>(link->owner);
        CMapBaseObjLink* next;

        for (CMapBaseObjLink* doodadLink = group->doodadDefLinkList.Head(); doodadLink; doodadLink = next) {
            CMapDoodadDef* doodadDef = static_cast<CMapDoodadDef*>(doodadLink->owner);
            next = group->doodadDefLinkList.Next(doodadLink);

            if (!doodadDef->wmoSpecialDoodadSet) {
                break;
            }

            if (doodadDef->wmoSpecialDoodadSet != doodadSet) {
                continue;
            }

            CMapBaseObjLink* parentNext;

            for (CMapBaseObjLink* parentLink = doodadDef->m_parentLinkList.Head(); parentLink; parentLink = parentNext) {
                parentNext = doodadDef->m_parentLinkList.Next(parentLink);
                CMap::FreeBaseObjLink(parentLink);
            }

            CMapObjDef::LinkDoodadDef(doodadDef, toMapObjDef, 0);
            STORM_ASSERT(doodadDef->GetRefCount() > 0);
            doodadDef->wmoSpecialDoodadSet = targetDoodadSet;
        }
    }

    return 0;
}

// VERIFY: 8aef7968 manual
// OFFSET: 0x007B69C0
void CMapObjDef::SetDoodadEmittersIgnoresDistance(CMapObjDef* mapObjDef, BOOL ignore, uint16_t doodadSet) {
    for (CMapBaseObjLink* link = mapObjDef->groupLinkList.Head(); link; link = mapObjDef->groupLinkList.Next(link)) {
        CMapObjDefGroup* group = static_cast<CMapObjDefGroup*>(link->owner);

        for (CMapBaseObjLink* doodadLink = group->doodadDefLinkList.Head(); doodadLink; doodadLink = group->doodadDefLinkList.Next(doodadLink)) {
            CMapDoodadDef* doodadDef = static_cast<CMapDoodadDef*>(doodadLink->owner);

            if (!doodadDef->wmoSpecialDoodadSet) {
                break;
            }

            if (doodadDef->wmoSpecialDoodadSet != doodadSet || !doodadDef->model) {
                continue;
            }

            for (uint32_t i = 0;; i++) {
                CM2Model* model = doodadDef->model;

                model->EnsureInitialized();

                if (i >= model->m_shared->m_data->m_particles.m_count) {
                    break;
                }

                CParticleEmitter2* emitter = doodadDef->model->GetParticleEmitter(i);

                if (ignore) {
                    emitter->m_flags |= CParticleEmitter2::RUNTIME_FLAGS::IGNORE_DISTANCE;
                } else {
                    emitter->m_flags &= ~CParticleEmitter2::RUNTIME_FLAGS::IGNORE_DISTANCE;
                }
            }
        }
    }
}

// VERIFY: 7dcf5e0d auto
// OFFSET: 0x007B6ED0
void CMap::LinkDoodadDefToMapObjDefs(CMapDoodadDef* doodadDef, CMapObjDef* skip) {
    for (CMapObjDef* mapObjDef = CMap::mapObjDefHash.m_fulllist.Head(); mapObjDef; mapObjDef = CMap::mapObjDefHash.m_fulllist.Next(mapObjDef)) {
        if (mapObjDef == skip) {
            continue;
        }

        if (!(mapObjDef->m_flags & MAPOBJ_FLAG_MAPOBJDEF) && (mapObjDef->m_flags & MAPOBJ_FLAG_PREPARED) && mapObjDef->groupLinkList.Head() && doodadDef->m_bbox.Intersects(mapObjDef->m_bbox)) {
            CMapObjDef::LinkDoodadDef(doodadDef, mapObjDef, 1);
        }
    }
}

// VERIFY: 973b4b33 manual
// OFFSET: 0x007B6F60
void CMapObjDef::SetDoodadsEnabled(CMapObjDef* mapObjDef, BOOL enable, uint16_t doodadSet) {
    for (CMapBaseObjLink* link = mapObjDef->groupLinkList.Head(); link; link = mapObjDef->groupLinkList.Next(link)) {
        CMapObjDefGroup* group = static_cast<CMapObjDefGroup*>(link->owner);
        CMapBaseObjLink* next;

        for (CMapBaseObjLink* doodadLink = group->doodadDefLinkList.Head(); doodadLink; doodadLink = next) {
            CMapDoodadDef* doodadDef = static_cast<CMapDoodadDef*>(doodadLink->owner);
            next = group->doodadDefLinkList.Next(doodadLink);

            if (!doodadDef->wmoSpecialDoodadSet) {
                break;
            }

            if (doodadDef->wmoSpecialDoodadSet != doodadSet || !doodadDef->model) {
                continue;
            }

            if (enable) {
                doodadDef->m_flags = static_cast<MapObjFlag>(doodadDef->m_flags & ~(MAPOBJ_FLAG_DISABLED | MAPOBJ_FLAG_NO_HITTEST));
                doodadDef->m_staticFlags &= ~MAP_STATIC_FLAGS::MAP_STATIC_FLAG_LIQUID_CACHE_MASK;
                CMap::LinkDoodadDefToMapObjDefs(doodadDef, mapObjDef);
                continue;
            }

            doodadDef->m_flags = static_cast<MapObjFlag>(doodadDef->m_flags | MAPOBJ_FLAG_DISABLED | MAPOBJ_FLAG_NO_HITTEST);
            CMapBaseObjLink* parentNext;

            for (CMapBaseObjLink* parentLink = doodadDef->m_parentLinkList.Head(); parentLink; parentLink = parentNext) {
                parentNext = doodadDef->m_parentLinkList.Next(parentLink);
                CMapBaseObj* baseObj = parentLink->ref;
                STORM_ASSERT(baseObj->GetType() != CMapBaseObj::Type_Chunk);

                if (baseObj->GetType() & CMapBaseObj::Type_MapObjDefGroup) {
#if defined(WOWDECOMP_ASSERTIONS)
                    const CMapObjDefGroup* mapObjDefGroup = static_cast<CMapObjDefGroup*>(baseObj);
                    STORM_ASSERT(mapObjDefGroup->m_parentLinkList.Head()->ref->GetType() & CMapBaseObj::Type_MapObjDef);
#endif
                    if (baseObj->m_parentLinkList.Head()->ref == mapObjDef) {
                        continue;
                    }
                }

                CMap::FreeBaseObjLink(parentLink);
            }
        }
    }
}

// VERIFY: 3c60fcab manual
// OFFSET: [0x007B3BE0, 0x007B3F00]
CMapObjDefGroup::~CMapObjDefGroup() {
    STORM_ASSERT(entityLinkList.Head() == nullptr); // 12213: "entityLinkList.Head() == __null"
    STORM_ASSERT(tableLink.IsLinked() == 0);
    STORM_ASSERT(visLink.IsLinked() == 0);

    if (this->m_liquidInstance) {
        this->m_liquidInstance->Release();
        this->m_liquidInstance = nullptr;
    }
}

// VERIFY: d684d55b manual
// OFFSET: 0x007B4700
bool CMapObjDef::IsAllDrawable() {
    if (!this->mapObj->IsAllDrawable()) {
        return false;
    }

    for (CMapBaseObjLink* link = this->groupLinkList.Head(); link; link = this->groupLinkList.Next(link)) {
        if (!static_cast<CMapObjDefGroup*>(link->owner)->IsAllLoaded()) {
            return false;
        }
    }

    return true;
}
