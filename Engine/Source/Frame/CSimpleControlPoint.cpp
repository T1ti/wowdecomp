#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleControlPoint.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include <new>
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Frame/CSimplePathAnim.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameXML/FrameXML.h"
#include "Engine/Source/FrameXML/XMLTree.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// STATIC: 0x00AC1A70
int32_t CSimpleControlPoint::s_metatable = LUA_NOREF;

// STATIC: 0x00B499E8
int32_t CSimpleControlPoint::s_objectType;

// STATIC: 0x00AC139C
const char* CSimpleControlPoint::s_objectTypeName = "ControlPoint";

// INLINED
int32_t CSimpleControlPoint::GetObjectType() {
    if (!CSimpleControlPoint::s_objectType) {
        CSimpleControlPoint::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleControlPoint::s_objectType;
}

// VERIFY: 387d7ed2 auto
// OFFSET: 0x00498740
bool CSimpleControlPoint::IsA(int32_t type) {
    return type == CSimpleControlPoint::s_objectType || type == CScriptObject::s_objectType;
}

// VERIFY: c865c60a auto
// OFFSET: 0x00498770
bool CSimpleControlPoint::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleControlPoint::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CScriptObject::s_objectTypeName, STORM_MAX_STR);
}

// VERIFY: 70589995 auto
// OFFSET: 0x004987C0
const char* CSimpleControlPoint::GetObjectTypeName() {
    return CSimpleControlPoint::s_objectTypeName;
}

// VERIFY: 70e114c4 auto
// OFFSET: 0x004987D0
int32_t CSimpleControlPoint::GetScriptMetaTable() {
    return CSimpleControlPoint::s_metatable;
}

// VERIFY: 78d7041d manual
// OFFSET: 0x004981F0
void CSimpleControlPoint::SetOffset(const C2Vector& offset) {
    if (offset.y * offset.y + offset.x * offset.x <= 0.00000023841858f) {
        this->m_offset.x = 0.0f;
        this->m_offset.y = 0.0f;
    } else {
        this->m_offset = offset;
    }
}

// VERIFY: 8eedb9e3 auto
// OFFSET: 0x004987E0
void CSimpleControlPoint::LoadXML(XMLNode* node, CStatus* status) {
    const char* inherits = node->GetAttributeByName("inherits");

    if (inherits && *inherits) {
        const char* taint;
        bool locked;
        XMLNode* inheritNode = FrameXML_AcquireHashNode(inherits, taint, locked);

        if (inheritNode) {
            if (locked) {
                status->Add(STATUS_WARNING, "Recursively inherited node: %s", inherits);
            } else {
                this->LoadXML(inheritNode, status);
                FrameXML_ReleaseHashNode(inherits);
            }
        } else {
            status->Add(STATUS_WARNING, "Couldn't find inherited node: %s", inherits);
        }
    }

    const char* parentKey = node->GetAttributeByName("parentKey");

    if (parentKey && *parentKey) {
        lua_State* L = FrameScript_GetContext();
        CScriptObject* parent = this->GetScriptObjectParent();

        if (parent) {
            lua_rawgeti(L, LUA_REGISTRYINDEX, parent->GetLuaObjectRef());
            lua_pushstring(L, parentKey);

            lua_rawgeti(L, LUA_REGISTRYINDEX, this->GetLuaObjectRef());
            lua_settable(L, -3);
            lua_settop(L, -2);
        }
    }

    C2Vector offset;

    const char* offsetX = node->GetAttributeByName("offsetX");

    if (offsetX && *offsetX) {
        float value = SStrToFloat(offsetX);
        offset.x = NDCToDDCWidth(value / (CoordinateGetAspectCompensation() * 1024.0f));
    }

    const char* offsetY = node->GetAttributeByName("offsetY");

    if (offsetY && *offsetY) {
        float value = SStrToFloat(offsetY);
        offset.y = NDCToDDCWidth(value / (CoordinateGetAspectCompensation() * 1024.0f));
    }

    this->SetOffset(offset);
}

// VERIFY: 56324fd9 manual
// OFFSET: 0x00499020
CSimpleControlPoint::~CSimpleControlPoint() {
}

// VERIFY: 5d17df01 manual
// OFFSET: 0x00499840
void CSimpleControlPoint::SetOrder(int32_t order, bool notify, bool wrap) {
    int32_t oldOrder = this->m_order;

    if (wrap) {
        this->m_order = order < 0 || order > 99 ? 0 : order;
    } else {
        this->m_order = order < 0 ? 0 : (order > 99 ? 99 : order);
    }

    if (this->m_parent && notify && oldOrder != this->m_order) {
        this->m_parent->NotifyOrderChanged(this);
    }
}

// VERIFY: b248ed66 manual
// OFFSET: 0x00499D00
CSimpleControlPoint::CSimpleControlPoint(CSimplePathAnim* parent) {
    this->m_position = 0.0f;
    this->m_order = -1;
    this->m_parent = parent;

    if (parent) {
        parent->m_controlPoints.LinkToTail(this);
    }
}

// VERIFY: f39958e7 auto
// OFFSET: 0x0049C710
bool CSimpleControlPoint::ChangeParent(CSimplePathAnim* parent) {
    if (!parent || this->m_parent == parent) {
        return false;
    }

    parent->m_controlPoints.LinkToTail(this);

    if (this->m_parent) {
        this->m_parent->NotifyControlPointRemoved(this);
    }

    this->m_parent = parent;
    parent->NotifyControlPointAdded(this);

    return true;
}

// VERIFY: 4b4593e0 auto
// OFFSET: 0x004383C0
CScriptObject* CSimpleControlPoint::GetScriptObjectParent() {
    return this->m_parent;
}
