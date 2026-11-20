#ifndef ENGINE_SOURCE_FRAME_CSCRIPTREGION_H
#define ENGINE_SOURCE_FRAME_CSCRIPTREGION_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Frame/CLayoutFrame.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CSimpleAnimGroup.h"
#include "Engine/Source/Frame/Types.h"
#include "Engine/Source/Storm/H/stpl.h"

class C2Vector;
class CSimpleFrame;
class CStatus;
class XMLNode;

class CScriptRegion : public CScriptObject, public CLayoutFrame {
    public:
    static int32_t s_objectType;
    static const char* s_objectTypeName;

    static int32_t GetObjectType();
    static void RegisterScriptMethods(lua_State* L);

    /* 0094 */ CSimpleFrame* m_parent;
    /* 0098 */ TSExplicitList<CSimpleAnimGroup, offsetof(CSimpleAnimGroup, m_regionLink)>* m_animGroups;
    /* 009C */ TSExplicitList<CSimpleAnimGroup, offsetof(CSimpleAnimGroup, m_playingLink)>* m_playingAnimGroups;

    virtual ~CScriptRegion();
    virtual bool IsA(int32_t type);
    virtual CScriptObject* GetScriptObjectParent();
    virtual bool IsA(const char* typeName);
    virtual const char* GetObjectTypeName();
    virtual void SetParent(CSimpleFrame* parent);
    virtual bool IsDragging();
    virtual void PreOnAnimUpdate();
    virtual void OnAnimUpdate(float elapsedSec);
    virtual void NotifyAnimBegin(CSimpleAnimGroup* group);
    virtual void NotifyAnimEnd(CSimpleAnimGroup* group);
    virtual void StopAnimating();
    virtual void AnimActivated(CSimpleAnimGroup* group, int32_t first, BOOL hasAlpha);
    virtual void AnimDeactivated(CSimpleAnimGroup* group, int32_t last);
    virtual void AddAnimTranslation(const C2Vector& translation);
    virtual void AddAnimRotation(FRAMEPOINT origin, const C2Vector& offset, float radians);
    virtual void AddAnimScale(FRAMEPOINT origin, const C2Vector& offset, const C2Vector& scale);
    virtual void AddAnimAlpha(int16_t alpha);
    virtual void LoadXML(XMLNode* node, CStatus* status);
    virtual CLayoutFrame* GetLayoutParent();
    virtual CLayoutFrame* GetLayoutFrameByName(const char* name);

    CScriptRegion();
    void GetActiveAnimInfo(int32_t type, int32_t& active, int32_t& activeOfType);
    int32_t HasActiveAnimGroup();
    int32_t HasActiveAnimOfType(int32_t type);
    bool IsMouseOver(float top, float bottom, float left, float right);
    void LoadXML_Animations(XMLNode* node, CStatus* status);
    bool ProtectedFunctionsAllowed();
    void RegisterAnimGroup(CSimpleAnimGroup* group);
};

#endif
