#ifndef ENGINE_SOURCE_FRAME_CSIMPLECHECKBOX_H
#define ENGINE_SOURCE_FRAME_CSIMPLECHECKBOX_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Frame/CSimpleButton.h"

class CSimpleFrame;
class CSimpleRegion;
class CSimpleTexture;
class CStatus;
class XMLNode;
struct lua_State;

class CSimpleCheckbox : public CSimpleButton {
    public:
    static CDataAllocator s_allocator;
    static int32_t s_metatable;
    static int32_t s_objectType;
    static const char* s_objectTypeName;

    static void operator delete(void* ptr);
    static void CreateScriptMetaTable();
    static void DestroyScriptMetaTable();
    static int32_t GetObjectType();
    static void RegisterScriptMethods(lua_State* L);

    /* 02F8 */ int32_t m_checked;
    /* 02FC */ CSimpleTexture* m_checkedTexture;
    /* 0300 */ CSimpleTexture* m_disabledCheckedTexture;

    virtual ~CSimpleCheckbox();
    virtual int32_t GetScriptMetaTable();
    virtual bool IsA(int32_t type);
    virtual bool IsA(const char* typeName);
    virtual const char* GetObjectTypeName();
    virtual void UnregisterRegion(CSimpleRegion* region);
    virtual void LoadXML(XMLNode* node, CStatus* status);
    virtual void Enable(BOOL enabled);
    virtual void OnClick(const char* btn, int32_t down);

    CSimpleCheckbox(CSimpleFrame* parent);
    void SetChecked(int32_t checked, BOOL force);
    int32_t SetCheckedTexture(const char* fileName);
    void SetCheckedTexture(CSimpleTexture* texture);
    int32_t SetDisabledCheckedTexture(const char* fileName);
    void SetDisabledCheckedTexture(CSimpleTexture* texture);
};

#endif
