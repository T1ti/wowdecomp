#ifndef ENGINE_SOURCE_FRAME_CSIMPLESTATUSBAR_H
#define ENGINE_SOURCE_FRAME_CSIMPLESTATUSBAR_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Frame/Types.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

class CImVector;
class CSimpleRegion;
class CSimpleTexture;
class CStatus;
class XMLNode;
struct lua_State;

class CSimpleStatusBar : public CSimpleFrame {
    public:
    static CDataAllocator s_allocator;

    static void operator delete(void* ptr);
    static void CreateScriptMetaTable();
    static void DestroyScriptMetaTable();
    static int32_t GetObjectType();

    virtual ~CSimpleStatusBar();
    virtual int32_t GetScriptMetaTable();
    virtual FrameScript_Object::ScriptIx* GetScriptByName(const char* name, FrameScript_Object::ScriptData& data);
    virtual bool IsA(int32_t type);
    virtual bool IsA(const char* typeName);
    virtual const char* GetObjectTypeName();
    virtual void GetScriptTime(lua_State* L, double* time, uint32_t* count, int32_t recurse);
    virtual void UnregisterRegion(CSimpleRegion* region);
    virtual void OnLayerUpdate(float elapsedSec);
    virtual void LoadXML(XMLNode* node, CStatus* status);
    virtual void SetValue(float value);
    virtual void SetStatusBarColor(const CImVector& color);
    virtual float GetAnimValue() const;

    CSimpleStatusBar(CSimpleFrame* parent);
    void GetStatusBarColor(CImVector& color);
    int32_t SetBarTexture(const char* fileName, uint32_t drawLayer);
    void SetBarTexture(CSimpleTexture* texture, uint32_t drawLayer);
    void SetMinMaxValues(float min, float max);
    void SetOrientation(ORIENTATION orientation);
    void SetRotatesTexture(int32_t rotates);

    float GetMinValue() const { return this->m_minValue; } // (inferred)
    float GetMaxValue() const { return this->m_maxValue; } // (inferred)
    float GetValue() const { return this->m_value; }
    ORIENTATION GetOrientation() const { return this->m_orientation; }
    CSimpleTexture* GetStatusBarTexture() const { return this->m_barTexture; }
    bool GetRotatesTexture() const { return this->m_rotatesTexture != 0; }

    private:
    static int32_t s_metatable;
    static int32_t s_objectType;
    static const char* s_objectTypeName;
    static void RegisterScriptMethods(lua_State* L);
    void RunOnMinMaxChangedScript();

    /* 029C */ uint32_t m_changed : 1;
    /* 029C */ uint32_t m_rangeSet : 1;
    /* 029C */ uint32_t m_valueSet : 1;
    /* 029C */ uint32_t m_rotatesTexture : 1;
    /* 02A0 */ float m_minValue;
    /* 02A4 */ float m_maxValue;
    /* 02A8 */ float m_value;
    /* 02AC */ CSimpleTexture* m_barTexture;
    /* 02B0 */ ORIENTATION m_orientation;
    /* 02B4 */ FrameScript_Object::ScriptIx m_onValueChanged;
    /* 02BC */ FrameScript_Object::ScriptIx m_onMinMaxChanged;
};

#endif
