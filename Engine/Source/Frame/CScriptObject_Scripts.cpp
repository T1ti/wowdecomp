#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// VERIFY: 2f726e9e auto
// OFFSET: 0x004A81B0
CScriptObject* FrameScript_GetObjectThis(lua_State* L, int32_t type) {
    if (lua_type(L, 1) != LUA_TTABLE) {
        luaL_error(L, "Attempt to find 'this' in non-table object (used '.' instead of ':' ?)");
        return nullptr;
    }

    lua_rawgeti(L, 1, 0);
    CScriptObject* object = static_cast<CScriptObject*>(lua_touserdata(L, -1));
    lua_settop(L, -2);

    if (!object) {
        luaL_error(L, "Attempt to find 'this' in non-framescript object");
        return nullptr;
    }

    if (!object->IsA(type)) {
        luaL_error(L, "Wrong object type for member function");
        return nullptr;
    }

    return object;
}

// VERIFY: d7726c6e manual
// OFFSET: 0x004A8240
static int32_t Script_GetObjectType(lua_State* L) {
    CScriptObject* object = FrameScript_GetObjectThis(L, CScriptObject::GetObjectType());

    lua_pushstring(L, object->GetObjectTypeName());
    return 1;
}

// VERIFY: 06842ad2 auto
// OFFSET: 0x004A8290
static int32_t Script_IsObjectType(lua_State* L) {
    CScriptObject* object = FrameScript_GetObjectThis(L, CScriptObject::GetObjectType());

    if (!lua_isstring(L, 2)) {
        const char* name = object->GetName();
        luaL_error(L, "Usage: %s:IsObjectType(\"type\")", name ? name : "<unnamed>");
        return 0;
    }

    if (object->IsA(lua_tolstring(L, 2, nullptr))) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 90c67768 auto
// OFFSET: 0x004A8340
static int32_t Script_GetName(lua_State* L) {
    CScriptObject* object = FrameScript_GetObjectThis(L, CScriptObject::GetObjectType());
    const char* name = object->GetName();

    if (name && *name) {
        lua_pushstring(L, name);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// VERIFY: 6771f295 auto
// OFFSET: 0x004A83A0
static int32_t Script_GetParent(lua_State* L) {
    CScriptObject* object = FrameScript_GetObjectThis(L, CScriptObject::GetObjectType());
    CScriptObject* parent = object->GetScriptObjectParent();

    if (parent) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, parent->GetLuaObjectRef());
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// STATIC: 0x00AC1B68
static FrameScript_Method s_ScriptMethods[] = {
    { "GetObjectType", &Script_GetObjectType },
    { "IsObjectType", &Script_IsObjectType },
    { "GetName", &Script_GetName },
    { "GetParent", &Script_GetParent },
};

// VERIFY: af190e31 auto
// OFFSET: 0x004A8190
void CScriptObject::RegisterScriptMethods(lua_State* L) {
    FrameScript_Object::FillScriptMethodTable(L, s_ScriptMethods, sizeof(s_ScriptMethods) / sizeof(s_ScriptMethods[0]));
}
