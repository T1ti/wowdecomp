#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_SIMPLESCRIPT_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_SIMPLESCRIPT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

struct SimpleScriptFunction {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_luaRef;
    /* 0008 */ int32_t inputNumberCount;
    /* 000C */ int32_t inputStringCount;
    /* 0010 */ int32_t outputNumberCount;
    /* 0014 */ int32_t outputStringCount;
};

struct SimpleScriptFunctionDesc {
    /* 0000 */ const char* m_name;
    /* 0004 */ const char** m_args;
    /* 0008 */ const char** m_stringArgs;
    /* 000C */ const char* m_body;
    /* 0010 */ const char** m_returns;
    /* 0014 */ const char** m_stringReturns;
};

struct SimpleScriptIO {
    /* 0000 */ int32_t numberMaxCount;
    /* 0004 */ int32_t numberCount;
    /* 0008 */ double* numbers; // (inferred)
    /* 000C */ int32_t stringMaxCount;
    /* 0010 */ uint32_t m_stringSize;
    /* 0014 */ int32_t stringCount;
    /* 0018 */ char* m_strings;
};

class CSimpleScriptManager {
    private:
    static void (*s_callbacks[16])(uint32_t);
    static uint32_t s_callbackCount;
    static CSimpleScriptManager* s_instance;

    /* 0000 */ int32_t m_hashMask;
    /* 0004 */ TSGrowableArray<SimpleScriptFunction>* m_buckets;
    /* 0008 */ int32_t m_envRef;

    CSimpleScriptManager();
    void CopyGlobal(const char* name);
    int32_t Execute(const char* script, const char* name);
    SimpleScriptFunction* GetFunction(uint32_t handle);
    int32_t Init(uint32_t hashMaskBitCount);
    void Uninit();

    public:
    static CSimpleScriptManager* GetInstance() { return s_instance; } // (inferred)
    static void Create();
    static void Destroy();
    static int32_t RegisterCallback(void (*callback)(uint32_t));
    static void UnregisterCallback(void (*callback)(uint32_t));

    // INLINED
    int32_t IsInited() const {
        return this->m_hashMask != -1;
    }

    int32_t CallFunction(uint32_t handle, const SimpleScriptIO& input, SimpleScriptIO& output);
    uint32_t FindFunction(int32_t id) const;
    uint32_t CreateFunction(int32_t id, const SimpleScriptFunctionDesc& desc);
};

#endif
