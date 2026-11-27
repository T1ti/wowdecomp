#include "ObjectEffect.h"
#include <new>
#include <typeinfo>
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"
#include "WoW/Source/Object/ObjectClient/Types.h"

// STATIC: 0x00C9ECD0
static ObjectEffectAnimState s_objectEffectAnimStateMap[ANIM_COUNT];

// STATIC: 0x00CA0488
static HASHKEY_NONE s_hashKeyNone;

// STATIC: 0x00CA048C
static TSHashTable<OBJ_EFFECT_DEF_LOOKUP, HASHKEY_NONE> s_effectDefLookup;

// STATIC: 0x00CA04B4
static TSHashTable<OBJ_EFFECT_GROUP_DEF_LOOKUP, HASHKEY_NONE> s_groupDefLookup;

// STATIC: 0x00CA04DC
static TSHashTable<OBJ_EFFECT_PACKAGE_DEF_LOOKUP, HASHKEY_NONE> s_packageDefLookup;

// STATIC: 0x00ADA37C
float (*CObjectEffectModifier::s_inputFuncs[NUM_INPUT_TYPES])(CObjectEffectManager* manager) = {
    &CObjectEffectModifier::InputNone,
    &CObjectEffectModifier::InputSpeed,
};

// STATIC: 0x00ADA384
float (*CObjectEffectModifier::s_mapFuncs[NUM_MAP_TYPES])(float input, const float* params) = {
    &CObjectEffectModifier::MapNone,
    &CObjectEffectModifier::MapLinear,
};

// STATIC: 0x00ADA38C
void (*CObjectEffectModifier::s_outputFuncs[NUM_OUTPUT_TYPES])(CObjectEffect* effect, float value) = {
    &CObjectEffectModifier::OutputNone,
    &CObjectEffectModifier::OutputPitch,
};

// INLINED
CObjectEffectModifier::CObjectEffectModifier() {
    this->m_inputType = INPUT_NONE;
    this->m_mapType = MAP_NONE;
    this->m_outputType = OUTPUT_NONE;
    this->m_params = nullptr;
    this->m_nNumParams = 0;
}

// INLINED
CObjectEffectModifier::~CObjectEffectModifier() {
    if (this->m_params) {
        delete[] this->m_params;
        this->m_params = nullptr;
    }
}

// INLINED
void CObjectEffectModifier::Init(const ObjectEffectModifierRec* pModifierRec) {
    STORM_ASSERT(pModifierRec);

    this->m_inputType = static_cast<INPUT_TYPE>(pModifierRec->m_inputType);
    this->m_outputType = static_cast<OUTPUT_TYPE>(pModifierRec->m_outputType);
    this->m_mapType = static_cast<MAP_TYPE>(pModifierRec->m_mapType);
    this->m_id = pModifierRec->m_id;
    this->m_nNumParams = this->m_mapType == MAP_LINEAR ? NUM_LINEAR_PARAMS : 0;

    if (this->m_nNumParams) {
        this->m_params = new (__FILE__, __LINE__) float[this->m_nNumParams];
    }
}

// INLINED
void CObjectEffectModifier::SetParam(int32_t nParamIndex, float value) {
    STORM_ASSERT(nParamIndex >= 0);
    STORM_ASSERT(nParamIndex < m_nNumParams);

    this->m_params[nParamIndex] = value;
}

// INLINED
CObjectEffectDefRef::CObjectEffectDefRef() {
    this->m_effectDef = nullptr;
}

// INLINED
CObjectEffectGroupDefRef::CObjectEffectGroupDefRef() {
    this->m_groupDef = nullptr;
}

// VERIFY: 918fad2d auto
// OFFSET: 0x006F1820
void InitObjectEffectStateMap() {
    for (int32_t i = 0; i < ANIM_COUNT; i++) {
        s_objectEffectAnimStateMap[i].animID = ANIM_COUNT;
        s_objectEffectAnimStateMap[i].stateID = OBJECT_EFFECT_STATE_INVALID;
    }

    s_objectEffectAnimStateMap[ANIM_ATTACK_THROWN].animID = ANIM_ATTACK_THROWN;
    s_objectEffectAnimStateMap[ANIM_ATTACK_THROWN].stateID = OBJECT_EFFECT_STATE_ANIM_ATTACK_THROWN;
    s_objectEffectAnimStateMap[ANIM_ATTACK_THROWN].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_HOLD_THROWN].animID = ANIM_HOLD_THROWN;
    s_objectEffectAnimStateMap[ANIM_HOLD_THROWN].stateID = OBJECT_EFFECT_STATE_ANIM_HOLD_THROWN;
    s_objectEffectAnimStateMap[ANIM_HOLD_THROWN].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_LOAD_THROWN].animID = ANIM_LOAD_THROWN;
    s_objectEffectAnimStateMap[ANIM_LOAD_THROWN].stateID = OBJECT_EFFECT_STATE_ANIM_LOAD_THROWN;
    s_objectEffectAnimStateMap[ANIM_LOAD_THROWN].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_READY_THROWN].animID = ANIM_READY_THROWN;
    s_objectEffectAnimStateMap[ANIM_READY_THROWN].stateID = OBJECT_EFFECT_STATE_ANIM_READY_THROWN;
    s_objectEffectAnimStateMap[ANIM_READY_THROWN].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_RUN].animID = ANIM_RUN;
    s_objectEffectAnimStateMap[ANIM_RUN].stateID = OBJECT_EFFECT_STATE_ANIM_RUN;
    s_objectEffectAnimStateMap[ANIM_RUN].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_WALK].animID = ANIM_WALK;
    s_objectEffectAnimStateMap[ANIM_WALK].stateID = OBJECT_EFFECT_STATE_ANIM_WALK;
    s_objectEffectAnimStateMap[ANIM_WALK].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_COMBAT_WOUND].animID = ANIM_COMBAT_WOUND;
    s_objectEffectAnimStateMap[ANIM_COMBAT_WOUND].stateID = OBJECT_EFFECT_STATE_ANIM_COMBAT_WOUND;
    s_objectEffectAnimStateMap[ANIM_COMBAT_WOUND].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_DEATH].animID = ANIM_DEATH;
    s_objectEffectAnimStateMap[ANIM_DEATH].stateID = OBJECT_EFFECT_STATE_ANIM_DEATH;
    s_objectEffectAnimStateMap[ANIM_DEATH].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_DEAD].animID = ANIM_DEAD;
    s_objectEffectAnimStateMap[ANIM_DEAD].stateID = OBJECT_EFFECT_STATE_ANIM_DEAD;
    s_objectEffectAnimStateMap[ANIM_DEAD].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_SPELL_CAST_DIRECTED].animID = ANIM_SPELL_CAST_DIRECTED;
    s_objectEffectAnimStateMap[ANIM_SPELL_CAST_DIRECTED].stateID = OBJECT_EFFECT_STATE_ANIM_SPELL_CAST_DIRECTED;
    s_objectEffectAnimStateMap[ANIM_SPELL_CAST_DIRECTED].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_STAND_WOUND].animID = ANIM_STAND_WOUND;
    s_objectEffectAnimStateMap[ANIM_STAND_WOUND].stateID = OBJECT_EFFECT_STATE_ANIM_STAND_WOUND;
    s_objectEffectAnimStateMap[ANIM_STAND_WOUND].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_READY_UNARMED].animID = ANIM_READY_UNARMED;
    s_objectEffectAnimStateMap[ANIM_READY_UNARMED].stateID = OBJECT_EFFECT_STATE_ANIM_READY_UNARMED;
    s_objectEffectAnimStateMap[ANIM_READY_UNARMED].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_STAND].animID = ANIM_STAND;
    s_objectEffectAnimStateMap[ANIM_STAND].stateID = OBJECT_EFFECT_STATE_ANIM_STAND;
    s_objectEffectAnimStateMap[ANIM_STAND].altStateID = OBJECT_EFFECT_STATE_ANIM_STAND_VAR1;
    s_objectEffectAnimStateMap[ANIM_SHUFFLE_LEFT].animID = ANIM_SHUFFLE_LEFT;
    s_objectEffectAnimStateMap[ANIM_SHUFFLE_LEFT].stateID = OBJECT_EFFECT_STATE_ANIM_SHUFFLE_LEFT;
    s_objectEffectAnimStateMap[ANIM_SHUFFLE_LEFT].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_SHUFFLE_RIGHT].animID = ANIM_SHUFFLE_RIGHT;
    s_objectEffectAnimStateMap[ANIM_SHUFFLE_RIGHT].stateID = OBJECT_EFFECT_STATE_ANIM_SHUFFLE_RIGHT;
    s_objectEffectAnimStateMap[ANIM_SHUFFLE_RIGHT].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_WALKBACKWARDS].animID = ANIM_WALKBACKWARDS;
    s_objectEffectAnimStateMap[ANIM_WALKBACKWARDS].stateID = OBJECT_EFFECT_STATE_ANIM_WALK_BACKWARDS;
    s_objectEffectAnimStateMap[ANIM_WALKBACKWARDS].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_JUMP_START].animID = ANIM_JUMP_START;
    s_objectEffectAnimStateMap[ANIM_JUMP_START].stateID = OBJECT_EFFECT_STATE_ANIM_JUMP_START;
    s_objectEffectAnimStateMap[ANIM_JUMP_START].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_JUMP_END].animID = ANIM_JUMP_END;
    s_objectEffectAnimStateMap[ANIM_JUMP_END].stateID = OBJECT_EFFECT_STATE_ANIM_JUMP_END;
    s_objectEffectAnimStateMap[ANIM_JUMP_END].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_FALL].animID = ANIM_FALL;
    s_objectEffectAnimStateMap[ANIM_FALL].stateID = OBJECT_EFFECT_STATE_ANIM_FALL;
    s_objectEffectAnimStateMap[ANIM_FALL].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_SWIM_IDLE].animID = ANIM_SWIM_IDLE;
    s_objectEffectAnimStateMap[ANIM_SWIM_IDLE].stateID = OBJECT_EFFECT_STATE_ANIM_SWIM_IDLE;
    s_objectEffectAnimStateMap[ANIM_SWIM_IDLE].altStateID = OBJECT_EFFECT_STATE_ANIM_SWIM_IDLE_VAR1;
    s_objectEffectAnimStateMap[ANIM_SWIM].animID = ANIM_SWIM;
    s_objectEffectAnimStateMap[ANIM_SWIM].stateID = OBJECT_EFFECT_STATE_ANIM_SWIM;
    s_objectEffectAnimStateMap[ANIM_SWIM].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_SWIM_LEFT].animID = ANIM_SWIM_LEFT;
    s_objectEffectAnimStateMap[ANIM_SWIM_LEFT].stateID = OBJECT_EFFECT_STATE_ANIM_SWIM_LEFT;
    s_objectEffectAnimStateMap[ANIM_SWIM_LEFT].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_SWIM_RIGHT].animID = ANIM_SWIM_RIGHT;
    s_objectEffectAnimStateMap[ANIM_SWIM_RIGHT].stateID = OBJECT_EFFECT_STATE_ANIM_SWIM_RIGHT;
    s_objectEffectAnimStateMap[ANIM_SWIM_RIGHT].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_SWIM_BACKWARDS].animID = ANIM_SWIM_BACKWARDS;
    s_objectEffectAnimStateMap[ANIM_SWIM_BACKWARDS].stateID = OBJECT_EFFECT_STATE_ANIM_SWIM_BACKWARDS;
    s_objectEffectAnimStateMap[ANIM_SWIM_BACKWARDS].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_RUN_LEFT].animID = ANIM_RUN_LEFT;
    s_objectEffectAnimStateMap[ANIM_RUN_LEFT].stateID = OBJECT_EFFECT_STATE_ANIM_RUN_LEFT;
    s_objectEffectAnimStateMap[ANIM_RUN_LEFT].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_RUN_RIGHT].animID = ANIM_RUN_RIGHT;
    s_objectEffectAnimStateMap[ANIM_RUN_RIGHT].stateID = OBJECT_EFFECT_STATE_ANIM_RUN_RIGHT;
    s_objectEffectAnimStateMap[ANIM_RUN_RIGHT].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_FLY].animID = ANIM_FLY;
    s_objectEffectAnimStateMap[ANIM_FLY].stateID = OBJECT_EFFECT_STATE_ANIM_FLY;
    s_objectEffectAnimStateMap[ANIM_FLY].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_SPRINT].animID = ANIM_SPRINT;
    s_objectEffectAnimStateMap[ANIM_SPRINT].stateID = OBJECT_EFFECT_STATE_ANIM_SPRINT;
    s_objectEffectAnimStateMap[ANIM_SPRINT].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_JUMP_LAND_RUN].animID = ANIM_JUMP_LAND_RUN;
    s_objectEffectAnimStateMap[ANIM_JUMP_LAND_RUN].stateID = OBJECT_EFFECT_STATE_ANIM_JUMP_LAND_RUN;
    s_objectEffectAnimStateMap[ANIM_JUMP_LAND_RUN].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_JUMP].animID = ANIM_JUMP;
    s_objectEffectAnimStateMap[ANIM_JUMP].stateID = OBJECT_EFFECT_STATE_ANIM_JUMP;
    s_objectEffectAnimStateMap[ANIM_JUMP].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_COMBAT_CRITICAL].animID = ANIM_COMBAT_CRITICAL;
    s_objectEffectAnimStateMap[ANIM_COMBAT_CRITICAL].stateID = OBJECT_EFFECT_STATE_ANIM_COMBAT_CRITICAL;
    s_objectEffectAnimStateMap[ANIM_COMBAT_CRITICAL].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_ATTACK_UNARMED].animID = ANIM_ATTACK_UNARMED;
    s_objectEffectAnimStateMap[ANIM_ATTACK_UNARMED].stateID = OBJECT_EFFECT_STATE_ANIM_ATTACK_UNARMED;
    s_objectEffectAnimStateMap[ANIM_ATTACK_UNARMED].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_SIT_GROUND].animID = ANIM_SIT_GROUND;
    s_objectEffectAnimStateMap[ANIM_SIT_GROUND].stateID = OBJECT_EFFECT_STATE_ANIM_SIT_GROUND;
    s_objectEffectAnimStateMap[ANIM_SIT_GROUND].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_SIT_GROUND_UP].animID = ANIM_SIT_GROUND_UP;
    s_objectEffectAnimStateMap[ANIM_SIT_GROUND_UP].stateID = OBJECT_EFFECT_STATE_ANIM_SIT_GROUND_UP;
    s_objectEffectAnimStateMap[ANIM_SIT_GROUND_UP].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_SIT_GROUND_DOWN].animID = ANIM_SIT_GROUND_DOWN;
    s_objectEffectAnimStateMap[ANIM_SIT_GROUND_DOWN].stateID = OBJECT_EFFECT_STATE_ANIM_SIT_GROUND_DOWN;
    s_objectEffectAnimStateMap[ANIM_SIT_GROUND_DOWN].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_HOVER].animID = ANIM_HOVER;
    s_objectEffectAnimStateMap[ANIM_HOVER].stateID = OBJECT_EFFECT_STATE_ANIM_HOVER;
    s_objectEffectAnimStateMap[ANIM_HOVER].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_GRAB].animID = ANIM_GRAB;
    s_objectEffectAnimStateMap[ANIM_GRAB].stateID = OBJECT_EFFECT_STATE_ANIM_GRAB;
    s_objectEffectAnimStateMap[ANIM_GRAB].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_SHIP_START].animID = ANIM_SHIP_START;
    s_objectEffectAnimStateMap[ANIM_SHIP_START].stateID = OBJECT_EFFECT_STATE_ANIM_SHIP_START;
    s_objectEffectAnimStateMap[ANIM_SHIP_START].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_SHIP_MOVING].animID = ANIM_SHIP_MOVING;
    s_objectEffectAnimStateMap[ANIM_SHIP_MOVING].stateID = OBJECT_EFFECT_STATE_ANIM_SHIP_MOVING;
    s_objectEffectAnimStateMap[ANIM_SHIP_MOVING].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_SHIP_STOP].animID = ANIM_SHIP_STOP;
    s_objectEffectAnimStateMap[ANIM_SHIP_STOP].stateID = OBJECT_EFFECT_STATE_ANIM_SHIP_STOP;
    s_objectEffectAnimStateMap[ANIM_SHIP_STOP].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_OPEN].animID = ANIM_OPEN;
    s_objectEffectAnimStateMap[ANIM_OPEN].stateID = OBJECT_EFFECT_STATE_ANIM_OPEN;
    s_objectEffectAnimStateMap[ANIM_OPEN].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_OPENED].animID = ANIM_OPENED;
    s_objectEffectAnimStateMap[ANIM_OPENED].stateID = OBJECT_EFFECT_STATE_ANIM_OPENED;
    s_objectEffectAnimStateMap[ANIM_OPENED].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_CLOSE].animID = ANIM_CLOSE;
    s_objectEffectAnimStateMap[ANIM_CLOSE].stateID = OBJECT_EFFECT_STATE_ANIM_CLOSE;
    s_objectEffectAnimStateMap[ANIM_CLOSE].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_CLOSED].animID = ANIM_CLOSED;
    s_objectEffectAnimStateMap[ANIM_CLOSED].stateID = OBJECT_EFFECT_STATE_ANIM_CLOSED;
    s_objectEffectAnimStateMap[ANIM_CLOSED].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_DESTROY].animID = ANIM_DESTROY;
    s_objectEffectAnimStateMap[ANIM_DESTROY].stateID = OBJECT_EFFECT_STATE_ANIM_DESTROY;
    s_objectEffectAnimStateMap[ANIM_DESTROY].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_DESTROYED].animID = ANIM_DESTROYED;
    s_objectEffectAnimStateMap[ANIM_DESTROYED].stateID = OBJECT_EFFECT_STATE_ANIM_DESTROYED;
    s_objectEffectAnimStateMap[ANIM_DESTROYED].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_CUSTOM_0].animID = ANIM_CUSTOM_0;
    s_objectEffectAnimStateMap[ANIM_CUSTOM_0].stateID = OBJECT_EFFECT_STATE_ANIM_CUSTOM0;
    s_objectEffectAnimStateMap[ANIM_CUSTOM_0].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_CUSTOM_1].animID = ANIM_CUSTOM_1;
    s_objectEffectAnimStateMap[ANIM_CUSTOM_1].stateID = OBJECT_EFFECT_STATE_ANIM_CUSTOM1;
    s_objectEffectAnimStateMap[ANIM_CUSTOM_1].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_CUSTOM_2].animID = ANIM_CUSTOM_2;
    s_objectEffectAnimStateMap[ANIM_CUSTOM_2].stateID = OBJECT_EFFECT_STATE_ANIM_CUSTOM2;
    s_objectEffectAnimStateMap[ANIM_CUSTOM_2].altStateID = OBJECT_EFFECT_STATE_INVALID;
    s_objectEffectAnimStateMap[ANIM_CUSTOM_3].animID = ANIM_CUSTOM_3;
    s_objectEffectAnimStateMap[ANIM_CUSTOM_3].stateID = OBJECT_EFFECT_STATE_ANIM_CUSTOM3;
    s_objectEffectAnimStateMap[ANIM_CUSTOM_3].altStateID = OBJECT_EFFECT_STATE_INVALID;
}

// OFFSET: 0x006F2020
CObjectEffectDef::~CObjectEffectDef() {
    if (this->m_pModifier) {
        delete this->m_pModifier;
        this->m_pModifier = nullptr;
    }
}

// OFFSET: 0x006F3040
CObjectEffectGroupDef::~CObjectEffectGroupDef() {
    this->m_enterEffects.Clear();
    this->m_loopEffects.Clear();
    this->m_exitEffects.Clear();
    this->m_eventEffects.Clear();
}

// OFFSET: 0x006F5830
CObjectEffectPackageDef::~CObjectEffectPackageDef() {
    for (OBJ_EFFECT_GROUP_DEF_REF_LIST_LOOKUP* lookup = this->m_stateGroupRefs.m_fulllist.Head(); lookup; lookup = this->m_stateGroupRefs.Next(lookup)) {
        lookup->m_groupDefRefs.Clear();
    }

    this->m_stateGroupRefs.Clear();

    for (OBJ_EFFECT_GROUP_DEF_REF_LIST_LOOKUP* lookup = this->m_eventGroupRefs.m_fulllist.Head(); lookup; lookup = this->m_eventGroupRefs.Next(lookup)) {
        lookup->m_groupDefRefs.Clear();
    }

    this->m_eventGroupRefs.Clear();
}

// INLINED
OBJ_EFFECT_GROUP_LOOKUP::~OBJ_EFFECT_GROUP_LOOKUP() {
    this->m_effects.Clear();
}

// VERIFY: 56342ab3 auto
// OFFSET: 0x006F2B40
void ObjectEffectSystem_ShutDown() {
    s_effectDefLookup.Clear();
    s_groupDefLookup.Clear();
    s_packageDefLookup.Clear();
}

// VERIFY: 769d7ae3 manual
// OFFSET: 0x006F66B0
void ObjectEffectSystem_Initialize() {
    InitObjectEffectStateMap();

    int32_t packageCount = g_objectEffectPackageDB.GetNumRecords();

    for (int32_t i = 0; i < packageCount; i++) {
        ObjectEffectPackageRec* rec = (i < 0 || i >= g_objectEffectPackageDB.GetNumRecords()) ? nullptr : &g_objectEffectPackageDB.m_records[i];
        OBJ_EFFECT_PACKAGE_DEF_LOOKUP* packageLookup = s_packageDefLookup.New(rec->m_id, s_hashKeyNone, 0, 0);
        packageLookup->m_objectEffectPackageDef.m_id = rec->m_id;
    }

    int32_t groupCount = g_objectEffectGroupDB.GetNumRecords();

    for (int32_t i = 0; i < groupCount; i++) {
        ObjectEffectGroupRec* rec = (i < 0 || i >= g_objectEffectGroupDB.GetNumRecords()) ? nullptr : &g_objectEffectGroupDB.m_records[i];
        OBJ_EFFECT_GROUP_DEF_LOOKUP* groupLookup = s_groupDefLookup.New(rec->m_id, s_hashKeyNone, 0, 0);
        groupLookup->m_objectEffectGroupDef.m_id = rec->m_id;
    }

    int32_t elemCount = g_objectEffectPackageElemDB.GetNumRecords();

    for (int32_t i = 0; i < elemCount; i++) {
        ObjectEffectPackageElemRec* rec = (i < 0 || i >= g_objectEffectPackageElemDB.GetNumRecords()) ? nullptr : &g_objectEffectPackageElemDB.m_records[i];
        OBJ_EFFECT_PACKAGE_DEF_LOOKUP* packageLookup = s_packageDefLookup.Ptr(rec->m_objectEffectPackageID, s_hashKeyNone);

        if (!packageLookup) {
            continue;
        }

        OBJ_EFFECT_GROUP_DEF_LOOKUP* pGroupDefLookup = s_groupDefLookup.Ptr(rec->m_objectEffectGroupID, s_hashKeyNone);
        STORM_ASSERT(pGroupDefLookup);
        OBJ_EFFECT_GROUP_DEF_REF_LIST_LOOKUP* refList = packageLookup->m_objectEffectPackageDef.m_stateGroupRefs.Ptr(rec->m_stateType, s_hashKeyNone);

        if (!refList) {
            refList = packageLookup->m_objectEffectPackageDef.m_stateGroupRefs.New(rec->m_stateType, s_hashKeyNone, 0, 0);
        }

        CObjectEffectGroupDefRef* pNewGroupDefRef = new (SMemAlloc(sizeof(CObjectEffectGroupDefRef), __FILE__, __LINE__, 0)) CObjectEffectGroupDefRef();
        STORM_ASSERT(pNewGroupDefRef);
        pNewGroupDefRef->m_groupDef = &pGroupDefLookup->m_objectEffectGroupDef;
        refList->m_groupDefRefs.LinkToTail(pNewGroupDefRef);
    }

    int32_t effectCount = g_objectEffectDB.GetNumRecords();

    for (int32_t i = 0; i < effectCount; i++) {
        ObjectEffectRec* pEffectRec = (i < 0 || i >= g_objectEffectDB.GetNumRecords()) ? nullptr : &g_objectEffectDB.m_records[i];
        STORM_ASSERT(pEffectRec);
        OBJ_EFFECT_DEF_LOOKUP* pEffectDefLookup = s_effectDefLookup.New(pEffectRec->m_id, s_hashKeyNone, 0, 0);
        pEffectDefLookup->m_objectEffectDef.m_id = pEffectRec->m_id;
        pEffectDefLookup->m_objectEffectDef.m_effectRecType = pEffectRec->m_effectRecType;
        pEffectDefLookup->m_objectEffectDef.m_effectRecID = pEffectRec->m_effectRecID;
        pEffectDefLookup->m_objectEffectDef.m_attachment = pEffectRec->m_attachment;
        pEffectDefLookup->m_objectEffectDef.m_offset.x = pEffectRec->m_offset[0];
        pEffectDefLookup->m_objectEffectDef.m_offset.y = pEffectRec->m_offset[1];
        pEffectDefLookup->m_objectEffectDef.m_offset.z = pEffectRec->m_offset[2];

        ObjectEffectModifierRec* modifierRec = pEffectRec->m_objectEffectModifierID ? g_objectEffectModifierDB.GetRecord(pEffectRec->m_objectEffectModifierID) : nullptr;

        if (pEffectRec->m_objectEffectModifierID && modifierRec->m_inputType > CObjectEffectModifier::INPUT_NONE && modifierRec->m_mapType > CObjectEffectModifier::MAP_NONE && modifierRec->m_outputType > CObjectEffectModifier::OUTPUT_NONE) {
            pEffectDefLookup->m_objectEffectDef.m_pModifier = new (SMemAlloc(sizeof(CObjectEffectModifier), __FILE__, __LINE__, 0)) CObjectEffectModifier();
            STORM_ASSERT(pEffectDefLookup->m_objectEffectDef.m_pModifier);
            pEffectDefLookup->m_objectEffectDef.m_pModifier->Init(modifierRec);

            for (int32_t param = 0; param < pEffectDefLookup->m_objectEffectDef.m_pModifier->m_nNumParams; param++) {
                pEffectDefLookup->m_objectEffectDef.m_pModifier->SetParam(param, modifierRec->m_param[param]);
            }
        } else {
            pEffectDefLookup->m_objectEffectDef.m_pModifier = nullptr;
        }

        OBJ_EFFECT_GROUP_DEF_LOOKUP* pGroupDefLookup = s_groupDefLookup.Ptr(pEffectRec->m_objectEffectGroupID, s_hashKeyNone);
        STORM_ASSERT(pGroupDefLookup);
        CObjectEffectDefRef* pNewEffectDefRef = new (SMemAlloc(sizeof(CObjectEffectDefRef), __FILE__, __LINE__, 0)) CObjectEffectDefRef();
        STORM_ASSERT(pNewEffectDefRef);
        pNewEffectDefRef->m_effectDef = &pEffectDefLookup->m_objectEffectDef;

        switch (pEffectRec->m_triggerType) {
            case OBJECT_EFFECT_TRIGGER_ENTER:
                pGroupDefLookup->m_objectEffectGroupDef.m_enterEffects.LinkToTail(pNewEffectDefRef);
                break;

            case OBJECT_EFFECT_TRIGGER_LOOP:
                pGroupDefLookup->m_objectEffectGroupDef.m_loopEffects.LinkToTail(pNewEffectDefRef);
                break;

            case OBJECT_EFFECT_TRIGGER_EXIT:
                pGroupDefLookup->m_objectEffectGroupDef.m_exitEffects.LinkToTail(pNewEffectDefRef);
                break;

            case OBJECT_EFFECT_TRIGGER_EVENT:
                pGroupDefLookup->m_objectEffectGroupDef.m_eventEffects.LinkToTail(pNewEffectDefRef);
                break;
        }
    }

    for (OBJ_EFFECT_PACKAGE_DEF_LOOKUP* packageLookup = s_packageDefLookup.m_fulllist.Head(); packageLookup; packageLookup = s_packageDefLookup.Next(packageLookup)) {
        CObjectEffectPackageDef& packageDef = packageLookup->m_objectEffectPackageDef;

        for (OBJ_EFFECT_GROUP_DEF_REF_LIST_LOOKUP* stateRefList = packageDef.m_stateGroupRefs.m_fulllist.Head(); stateRefList; stateRefList = packageDef.m_eventGroupRefs.Next(stateRefList)) {
            for (CObjectEffectGroupDefRef* groupRef = stateRefList->m_groupDefRefs.Head(); groupRef; groupRef = stateRefList->m_groupDefRefs.Next(groupRef)) {
                for (CObjectEffectDefRef* effectRef = groupRef->m_groupDef->m_eventEffects.Head(); effectRef; effectRef = groupRef->m_groupDef->m_eventEffects.Next(effectRef)) {
                    ObjectEffectRec* pEffectRec = g_objectEffectDB.GetRecord(effectRef->m_effectDef->m_id);
                    STORM_ASSERT(pEffectRec);
                    OBJ_EFFECT_GROUP_DEF_REF_LIST_LOOKUP* eventRefList = packageDef.m_eventGroupRefs.Ptr(pEffectRec->m_eventType, s_hashKeyNone);
                    bool found = false;

                    if (eventRefList) {
                        for (CObjectEffectGroupDefRef* eventRef = eventRefList->m_groupDefRefs.Head(); eventRef; eventRef = eventRefList->m_groupDefRefs.Next(eventRef)) {
                            if (eventRef->m_groupDef == groupRef->m_groupDef) {
                                found = true;
                                break;
                            }
                        }
                    }

                    if (found) {
                        continue;
                    }

                    eventRefList = packageDef.m_eventGroupRefs.New(pEffectRec->m_eventType, s_hashKeyNone, 0, 0);

                    CObjectEffectGroupDefRef* eventRef = new (SMemAlloc(sizeof(CObjectEffectGroupDefRef), __FILE__, __LINE__, 0)) CObjectEffectGroupDefRef();
                    eventRef->m_groupDef = groupRef->m_groupDef;
                    eventRefList->m_groupDefRefs.LinkToTail(eventRef);
                }
            }
        }
    }
}

// VERIFY: 41220598 auto
// OFFSET: 0x006F1D70
C3Vector CObjectEffect::GetWorldPosition() {
    STORM_ASSERT(m_pManager);
    CGObject_C* object = this->m_pManager->m_pOwningObject;
    CM2Model* model = object->GetObjectModel();
    C44Matrix transform;

    if (model && model->IsLoaded(0, 0) && (model->m_runtimeFlags & M2_FLAGS::M2_WORLD_TRANSFORM_VALID)) {
        if (model->HasAttachment(this->m_def->m_attachment)) {
            transform = model->GetAttachmentWorldTransform(this->m_def->m_attachment);
        } else {
            transform = model->GetAnimatedWorldTransform();
        }
    } else {
        transform = object->GetMatrix();
    }

    C3Vector position = this->m_def->m_offset;
    position = position * transform;
    return position;
}

// VERIFY: 4fb949cb manual
// OFFSET: 0x006F2AC0
CObjectEffect::~CObjectEffect() {
    if (this->m_def->m_effectRecType == OBJECT_EFFECT_REC_TYPE_SOUND_KIT) {
        if (this->m_soundKitObject.m_sound.HasSound()) {
            SI2::StopSound(&this->m_soundKitObject, 0, -1.0f, 1);
        } else if (this->m_soundKitObject.m_advancedId) {
            SI2::StopAdvancedSound(this->m_soundKitObject.m_advancedId);
        }
    }
}

// VERIFY: e96368c7 auto
// OFFSET: 0x006F1700
float CObjectEffectModifier::InputNone(CObjectEffectManager* manager) {
    return -1.0f;
}

// VERIFY: cd29e853 auto
// OFFSET: 0x006F1D50
float CObjectEffectModifier::InputSpeed(CObjectEffectManager* pManager) {
    STORM_ASSERT(pManager);
    STORM_ASSERT(pManager->m_pOwningObject);  // 12213: "pManager->GetOwningObject()"
    return pManager->m_pOwningObject->GetSpeed();
}

// VERIFY: 924d173e auto
// OFFSET: 0x006F1710
float CObjectEffectModifier::MapNone(float input, const float* params) {
    return input;
}

// VERIFY: e0fb924b manual
// OFFSET: 0x006F1720
float CObjectEffectModifier::MapLinear(float input, const float* params) {
    if (input <= params[0]) {
        return params[2];
    }

    if (input >= params[1]) {
        return params[3];
    }

    return (input - params[0]) / (params[1] - params[0]) * (params[3] - params[2]) + params[2];
}

// VERIFY: 8e9ba7cd auto
// OFFSET: 0x005EEB70
void CObjectEffectModifier::OutputNone(CObjectEffect* effect, float value) {
}

// VERIFY: b1528bea auto
// OFFSET: 0x006F1770
void CObjectEffectModifier::OutputPitch(CObjectEffect* pEffect, float value) {
    STORM_ASSERT(pEffect);
    SI2::SetPitch(&pEffect->m_soundKitObject, value);
}

// VERIFY: 894f2b3c auto
// OFFSET: 0x006F1790
void CObjectEffectModifier::Apply(CObjectEffectManager* manager, CObjectEffect* effect) {
    if (this->m_mapType <= MAP_NONE) {
        return;
    }

    float input = CObjectEffectModifier::s_inputFuncs[this->m_inputType](manager);
    float value = CObjectEffectModifier::s_mapFuncs[this->m_mapType](input, this->m_params);
    CObjectEffectModifier::s_outputFuncs[this->m_outputType](effect, value);
}

// VERIFY: 07e2b193 auto
// OFFSET: 0x006F17F0
OBJECT_EFFECT_STATE FindObjectEffectStateFromAnimID(int32_t animID, int32_t alternate) {
    STORM_ASSERT((s_objectEffectAnimStateMap[animID].animID == animID) || ((s_objectEffectAnimStateMap[animID].animID == ANIM_COUNT) && s_objectEffectAnimStateMap[animID].stateID == OBJECT_EFFECT_STATE_INVALID)); // 12213: "(s_objectEffectAnimStateMap[animID].animID == animID) || ((s_objectEffectAnimStateMap[animID].animID == NUM_ANIMATIONDATAENUM) && s_objectEffectAnimStateMap[animID].stateID == OES_INVALID)"

    if (alternate == 1) {
        return s_objectEffectAnimStateMap[animID].altStateID;
    }

    return s_objectEffectAnimStateMap[animID].stateID;
}

// INLINED
CObjectEffect::CObjectEffect() {
    this->m_def = nullptr;
    this->m_pManager = nullptr;
    this->m_nRefCount = 0;
    this->m_nPlayingRefCount = 0;
}

// INLINED
CObjectEffectRef::CObjectEffectRef() {
    this->m_effect = nullptr;
}

// VERIFY: 2dd44e63 auto
// OFFSET: 0x006F1E70
void CObjectEffect::PlayOneShot() {
    C3Vector position = this->GetWorldPosition();

    if (this->m_def->m_effectRecType == OBJECT_EFFECT_REC_TYPE_SOUND_KIT) {
        SE2SoundKitProperties properties;
        properties.ResetToDefaults();
        properties.m_loopMode = SE2_LOOP_MODE_NONLOOPING;

        if (!SI2::IsPlaying(&this->m_soundKitObject)) {
            SI2::PlaySoundKit(this->m_def->m_effectRecID, &position, &this->m_soundKitObject, &properties, 0, nullptr, 0, 0);
        }
    }

    this->m_nRefCount++;
}

// VERIFY: 0cac3b31 auto
// OFFSET: 0x006F1EF0
void CObjectEffect::TriggerLoopingEffect() {
    if (this->m_nRefCount > 0) {
        return;
    }

    C3Vector position = this->GetWorldPosition();

    if (this->m_def->m_effectRecType == OBJECT_EFFECT_REC_TYPE_SOUND_KIT) {
        if (!SI2::IsPlaying(&this->m_soundKitObject)) {
            SI2_SOUND_USAGE usage = SI2::GetSoundKitUsage(this->m_def->m_effectRecID);

            SE2SoundKitProperties properties;
            properties.ResetToDefaults();
            properties.m_fadeInTime = 0.7f;
            properties.m_fadeOutTime = 0.7f;
            properties.m_loopMode = static_cast<SE2_LOOP_MODE>(usage != SI2_SOUND_USAGE_INTERVAL);

            SI2::PlaySoundKit(this->m_def->m_effectRecID, &position, &this->m_soundKitObject, &properties, 0, nullptr, 0, 0);
            this->m_nRefCount++;
            return;
        }

        this->m_soundKitObject.m_sound.FadeInNow();
    }

    this->m_nRefCount++;
}

// VERIFY: 1fcb379b auto
// OFFSET: 0x006F1FB0
void CObjectEffect::UpdateLoopingEffect(CObjectEffectManager* manager) {
    if (this->m_def->m_effectRecType != OBJECT_EFFECT_REC_TYPE_SOUND_KIT) {
        return;
    }

    C3Vector position = this->GetWorldPosition();

    if (SI2::IsPlaying(&this->m_soundKitObject)) {
        this->m_soundKitObject.m_sound.FadeInNow();
    } else {
        this->TriggerLoopingEffect();
    }

    if (this->m_def->m_pModifier) {
        this->m_def->m_pModifier->Apply(manager, this);
    }

    SI2::Set3DPosition(&this->m_soundKitObject, &position);
}

// VERIFY: 7251efaa manual
// OFFSET: 0x006F3910
void CObjectEffectManager::PropertyChanged(int32_t modifierId) {
    OBJ_EFFECT_REF_LIST_LOOKUP* refList = this->m_refLists.Ptr(modifierId, s_hashKeyNone);

    if (!refList) {
        return;
    }

    for (CObjectEffectRef* ref = refList->m_refs.Head(); ref; ref = refList->m_refs.Next(ref)) {
        CObjectEffectModifier* modifier = ref->m_effect->m_def->m_pModifier;

        if (modifier) {
            modifier->Apply(this, ref->m_effect);
        }
    }
}

// VERIFY: 46ef89f8 manual
// OFFSET: 0x006F39B0
void CObjectEffectManager::UpdateAllPlayingEffects() {
    for (CObjectEffect* pEffect = this->m_loopingEffects.Head(); pEffect; pEffect = this->m_loopingEffects.Next(pEffect)) {
        pEffect->UpdateLoopingEffect(this);
    }

    CObjectEffect* pEffect = this->m_oneShotEffects.Head();

    while (pEffect) {
        if (pEffect->m_def->m_effectRecType == OBJECT_EFFECT_REC_TYPE_SOUND_KIT) {
            C3Vector position = pEffect->GetWorldPosition();

            if (SI2::IsPlaying(&pEffect->m_soundKitObject)) {
                SI2::Set3DPosition(&pEffect->m_soundKitObject, &position);
                pEffect = this->m_oneShotEffects.Next(pEffect);
                continue;
            }
        } else if (pEffect->m_def->m_effectRecType == OBJECT_EFFECT_REC_TYPE_UNKNOWN_2) {
            pEffect = this->m_oneShotEffects.Next(pEffect);
            continue;
        }

        int32_t modifierId = pEffect->m_def->m_pModifier ? pEffect->m_def->m_pModifier->m_id : 0;
        OBJ_EFFECT_REF_LIST_LOOKUP* pEffectListLookup = this->m_refLists.Ptr(modifierId, s_hashKeyNone);
        STORM_ASSERT(pEffectListLookup);

        if (pEffectListLookup) {
            CObjectEffectRef* ref = pEffectListLookup->m_refs.Head();

            while (ref) {
                if (ref->m_effect == pEffect) {
                    ref = pEffectListLookup->m_refs.DeleteNode(ref);
                } else {
                    ref = pEffectListLookup->m_refs.Next(ref);
                }
            }
        }

        pEffect->m_nRefCount--;
        STORM_ASSERT(pEffect->m_nRefCount >= 0);

        CObjectEffect* next = this->m_oneShotEffects.Next(pEffect);
        pEffect->~CObjectEffect();
        SMemFree(pEffect, typeid(CObjectEffect).raw_name(), SLOG_OBJECT, 0);
        pEffect = next;
    }
}

// VERIFY: f71878a1 manual
// OFFSET: 0x006F5900
CObjectEffectManager::CObjectEffectManager() {
    char* pBitArray = static_cast<char*>(SMemAlloc((NUM_OBJECT_EFFECT_STATES + 7) / 8, __FILE__, __LINE__, SMEM_FLAG_ZEROMEMORY));
    STORM_ASSERT(pBitArray);
    this->m_states.SetRawData(pBitArray, NUM_OBJECT_EFFECT_STATES, true);
}

// VERIFY: 79aebd0f manual
// OFFSET: 0x006F60E0
void CObjectEffectManager::PlayOneShotEffect(CObjectEffectDef* pEffectDef) {
    STORM_ASSERT(pEffectDef);

    CObjectEffect* effect = new (__FILE__, __LINE__) CObjectEffect();
    effect->m_pManager = this;
    effect->m_def = pEffectDef;
    this->m_oneShotEffects.LinkToTail(effect);

    int32_t modifierId = pEffectDef->m_pModifier ? pEffectDef->m_pModifier->m_id : 0;
    OBJ_EFFECT_REF_LIST_LOOKUP* refList = this->m_refLists.Ptr(modifierId, s_hashKeyNone);

    if (!refList) {
        refList = this->m_refLists.New(modifierId, s_hashKeyNone, 0, 0);
    }

    CObjectEffectRef* ref = new (__FILE__, __LINE__) CObjectEffectRef();
    ref->m_effect = effect;
    refList->m_refs.LinkToTail(ref);

    effect->PlayOneShot();

    if (pEffectDef->m_pModifier) {
        pEffectDef->m_pModifier->Apply(this, effect);
    }
}

// VERIFY: 4b360386 manual
// OFFSET: 0x006F61D0
void CObjectEffectManager::ExitState(OBJECT_EFFECT_STATE state, int32_t playExitEffects) {
    if (!this->m_states.Get(state)) {
        return;
    }

    this->m_states.Set(state, false);

    OBJ_EFFECT_GROUP_DEF_REF_LIST_LOOKUP* stateRefs = this->m_packageDef->m_stateGroupRefs.Ptr(state, s_hashKeyNone);

    if (!stateRefs) {
        return;
    }

    for (CObjectEffectGroupDefRef* groupRef = stateRefs->m_groupDefRefs.Head(); groupRef; groupRef = stateRefs->m_groupDefRefs.Next(groupRef)) {
        if (playExitEffects) {
            for (CObjectEffectDefRef* effectRef = groupRef->m_groupDef->m_exitEffects.Head(); effectRef; effectRef = groupRef->m_groupDef->m_exitEffects.Next(effectRef)) {
                this->PlayOneShotEffect(effectRef->m_effectDef);
            }
        }

        int32_t clear = 1;
        OBJ_EFFECT_GROUP_LOOKUP* group = this->m_groups.Ptr(groupRef->m_groupDef->m_id, s_hashKeyNone);

        if (!group) {
            continue;
        }

        for (OBJ_EFFECT_REF_LOOKUP* pEffectRefLookup = group->m_effects.m_fulllist.Head(); pEffectRefLookup; pEffectRefLookup = group->m_effects.Next(pEffectRefLookup)) {
            CObjectEffect* effect = pEffectRefLookup->m_pObjectEffect;
            effect->m_nPlayingRefCount--;
            STORM_ASSERT(pEffectRefLookup->m_pObjectEffect->m_nPlayingRefCount >= 0);

            if (effect->m_nPlayingRefCount) {
                clear = 0;
                continue;
            }

            this->m_loopingEffects.UnlinkNode(effect);
            this->m_oneShotEffects.LinkToTail(effect);

            if (effect->m_def->m_effectRecType == OBJECT_EFFECT_REC_TYPE_SOUND_KIT) {
                if (effect->m_soundKitObject.m_advancedId) {
                    SI2::StopAdvancedSound(effect->m_soundKitObject.m_advancedId, 0, 0.7f);
                } else {
                    SI2::StopSound(&effect->m_soundKitObject, 0, -1.0f, 1);
                }
            }
        }

        if (clear) {
            group->m_effects.InternalClear(0);
        }
    }
}

// VERIFY: 3c6099cc manual
// OFFSET: 0x006F6420
void CObjectEffectManager::PlayLoopingEffect(CObjectEffectDef* pEffectDef, int32_t groupId) {
    OBJ_EFFECT_GROUP_LOOKUP* pGroupLookup = this->m_groups.Ptr(groupId, s_hashKeyNone);

    if (!pGroupLookup) {
        pGroupLookup = this->m_groups.New(groupId, s_hashKeyNone, 0, 0);
    }

    STORM_ASSERT(pEffectDef);
    STORM_ASSERT(pGroupLookup);

    for (CObjectEffect* effect = this->m_oneShotEffects.Head(); effect; effect = this->m_oneShotEffects.Next(effect)) {
        if (effect->m_def != pEffectDef) {
            continue;
        }

        OBJ_EFFECT_REF_LOOKUP* lookup = pGroupLookup->m_effects.New(pEffectDef->m_id, s_hashKeyNone, 0, 0);
        lookup->m_pObjectEffect = effect;
        effect->m_link.Unlink();
        this->m_loopingEffects.LinkToTail(effect);

        if (effect->m_def->m_effectRecType == OBJECT_EFFECT_REC_TYPE_SOUND_KIT) {
            effect->m_soundKitObject.m_sound.FadeInNow();
            effect->m_nPlayingRefCount++;
        }

        return;
    }

    OBJ_EFFECT_REF_LOOKUP* lookup = pGroupLookup->m_effects.Ptr(pEffectDef->m_id, s_hashKeyNone);

    if (lookup) {
        lookup->m_pObjectEffect->m_nPlayingRefCount++;
        return;
    }

    lookup = pGroupLookup->m_effects.New(pEffectDef->m_id, s_hashKeyNone, 0, 0);
    lookup->m_pObjectEffect = new (__FILE__, __LINE__) CObjectEffect();
    lookup->m_pObjectEffect->m_pManager = this;
    lookup->m_pObjectEffect->m_def = pEffectDef;
    this->m_loopingEffects.LinkToTail(lookup->m_pObjectEffect);
    lookup->m_pObjectEffect->m_nPlayingRefCount++;

    int32_t modifierId = pEffectDef->m_pModifier ? pEffectDef->m_pModifier->m_id : 0;
    OBJ_EFFECT_REF_LIST_LOOKUP* refList = this->m_refLists.Ptr(modifierId, s_hashKeyNone);

    if (!refList) {
        refList = this->m_refLists.New(modifierId, s_hashKeyNone, 0, 0);
    }

    CObjectEffectRef* ref = new (__FILE__, __LINE__) CObjectEffectRef();
    ref->m_effect = lookup->m_pObjectEffect;
    refList->m_refs.LinkToTail(ref);

    lookup->m_pObjectEffect->TriggerLoopingEffect();

    if (pEffectDef->m_pModifier) {
        pEffectDef->m_pModifier->Apply(this, lookup->m_pObjectEffect);
    }
}

// VERIFY: 7ce181a0 manual
// OFFSET: 0x006F70B0
void CObjectEffectManager::ShutDown() {
    for (int32_t state = 0; state < NUM_OBJECT_EFFECT_STATES; state++) {
        this->ExitState(static_cast<OBJECT_EFFECT_STATE>(state), 0);
    }

    for (OBJ_EFFECT_REF_LIST_LOOKUP* refList = this->m_refLists.m_fulllist.Head(); refList; refList = this->m_refLists.Next(refList)) {
        CObjectEffectRef* ref = refList->m_refs.Head();

        while (ref) {
            CObjectEffectRef* next = refList->m_refs.Next(ref);
            refList->m_refs.UnlinkNode(ref);
            delete ref;
            ref = next;
        }
    }

    this->m_refLists.Clear();
    this->m_groups.Clear();

    while (CObjectEffect* effect = this->m_loopingEffects.Head()) {
        effect->~CObjectEffect();
        SMemFree(effect, typeid(CObjectEffect).raw_name(), SLOG_OBJECT, 0);
    }

    while (CObjectEffect* effect = this->m_oneShotEffects.Head()) {
        effect->~CObjectEffect();
        SMemFree(effect, typeid(CObjectEffect).raw_name(), SLOG_OBJECT, 0);
    }
}

// VERIFY: 6bb11b7f manual
// OFFSET: 0x006F7270
void CObjectEffectManager::EnterState(OBJECT_EFFECT_STATE state, int32_t playEnterEffects, int32_t refresh) {
    if (refresh) {
        if (!this->m_states.Get(state)) {
            return;
        }
    } else {
        if (this->m_states.Get(state)) {
            return;
        }

        this->m_states.Set(state, true);
    }

    OBJ_EFFECT_GROUP_DEF_REF_LIST_LOOKUP* stateRefs = this->m_packageDef->m_stateGroupRefs.Ptr(state, s_hashKeyNone);

    if (!stateRefs) {
        return;
    }

    for (CObjectEffectGroupDefRef* groupRef = stateRefs->m_groupDefRefs.Head(); groupRef; groupRef = stateRefs->m_groupDefRefs.Next(groupRef)) {
        if (playEnterEffects) {
            for (CObjectEffectDefRef* effectRef = groupRef->m_groupDef->m_enterEffects.Head(); effectRef; effectRef = groupRef->m_groupDef->m_enterEffects.Next(effectRef)) {
                this->PlayOneShotEffect(effectRef->m_effectDef);
            }
        }

        for (CObjectEffectDefRef* effectRef = groupRef->m_groupDef->m_loopEffects.Head(); effectRef; effectRef = groupRef->m_groupDef->m_loopEffects.Next(effectRef)) {
            this->PlayLoopingEffect(effectRef->m_effectDef, groupRef->m_groupDef->m_id);
        }
    }
}

// VERIFY: 65d7c3ab manual
// OFFSET: 0x006F7370
CObjectEffectManager::~CObjectEffectManager() {
    this->ShutDown();
}

// VERIFY: 403e887b auto
// OFFSET: 0x006F7420
int32_t CObjectEffectManager::Init(int32_t packageId, CGObject_C* owner) {
    this->m_packageId = packageId;

    OBJ_EFFECT_PACKAGE_DEF_LOOKUP* lookup = s_packageDefLookup.Ptr(packageId, s_hashKeyNone);

    if (!lookup) {
        return 0;
    }

    this->m_packageDef = &lookup->m_objectEffectPackageDef;
    this->m_pOwningObject = owner;
    STORM_ASSERT(m_pOwningObject);
    this->EnterState(OBJECT_EFFECT_STATE_ALWAYS, 1, 0);
    this->EnterState(OBJECT_EFFECT_STATE_MOVEMENT_NOT_MOVING, 1, 0);
    return 1;
}
