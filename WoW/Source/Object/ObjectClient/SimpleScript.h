#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_SIMPLESCRIPT_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_SIMPLESCRIPT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

struct SimpleScriptFunction {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_luaRef;
    /* 0008 */ int32_t m_numArgs;
    /* 000C */ int32_t m_numStringArgs;
    /* 0010 */ int32_t m_numReturns;
    /* 0014 */ int32_t m_numStringReturns;
};

struct SimpleScriptFunctionDesc {
    /* 0000 */ const char* m_name;
    /* 0004 */ const char** m_args;
    /* 0008 */ const char** m_stringArgs;
    /* 000C */ const char* m_body;
    /* 0010 */ const char** m_returns;
    /* 0014 */ const char** m_stringReturns;
};

struct SimpleScriptNumbers {
    /* 0000 */ uint32_t m_alloc;
    /* 0004 */ uint32_t m_count;
    /* 0008 */ double* m_data;
    /* 000C */ uint32_t m_chunk;
};

struct SimpleScriptIO {
    /* 0000 */ SimpleScriptNumbers m_numbers;
    /* 0010 */ uint32_t m_stringSize;
    /* 0014 */ uint32_t m_stringCount;
    /* 0018 */ char* m_strings;
};

class CSimpleScriptManager {
    public:
    static void (*s_callbacks[16])(uint32_t);
    static uint32_t s_callbackCount;
    static CSimpleScriptManager* s_instance;

    static void Create();
    static void Destroy();
    static int32_t RegisterCallback(void (*callback)(uint32_t));
    static void UnregisterCallback(void (*callback)(uint32_t));

    // INLINED
    int32_t IsInited() const {
        return this->m_hashMask != -1;
    }

    /* 0000 */ int32_t m_hashMask;
    /* 0004 */ TSGrowableArray<SimpleScriptFunction>* m_buckets;
    /* 0008 */ int32_t m_envRef;

    CSimpleScriptManager();
    int32_t CallFunction(uint32_t handle, const SimpleScriptIO& in, SimpleScriptIO& out);
    void CopyGlobal(const char* name);
    int32_t Execute(const char* script, const char* name);
    uint32_t FindFunction(int32_t id);
    SimpleScriptFunction* GetFunction(uint32_t handle);
    uint32_t CreateFunction(int32_t id, const SimpleScriptFunctionDesc& desc);
    int32_t Init(uint32_t bits);
    void Uninit();
};

#endif
