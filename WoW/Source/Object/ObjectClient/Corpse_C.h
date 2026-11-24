#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_CORPSE_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_CORPSE_C_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "WoW/Source/Object/Corpse.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"
#include "WoW/Source/Object/Passenger.h"

class CCharacterComponent;
class CClientObjCreate;
class CM2Model;

class CGCorpse_C : public CGObject_C, public CGCorpse {
    public:
    static void Initialize();
    /* 00D8 */ CPassenger m_passenger;
    /* 0108 */ TSList<void> m_corpseMirrorLists [CGCorpseData::NUM_FIELDS];

    ~CGCorpse_C() override;
    BOOL ShouldFadeout() override;
    float GetTrueScale() const override;
    BOOL CanHighlight() const override;
    float GetFacing() const override;
    void ModelLoaded(CM2Model* model) override;
    void Disable() override;
    void Reenable() override;
    void PostReenable() override;
    int32_t GetModelFileName(const char** name) const override;
    void RenderTargetSelection() const override;
    int32_t Animate(float elapsed) override;
    void ShouldRender(uint32_t flags, int32_t& render, int32_t& hidden) override;
    void OnRightClick() override;

    CGCorpse_C(uint32_t time, const CClientObjCreate& create);
    void ApplyGuildColor(int32_t request);
    BOOL CanBeLooted() const;
    void SetStorage(uint32_t* storage, uint32_t* saved);
    float GetDisplayScale() const;
    uint32_t GetFactionTemplate() const;
    const char* GetPortraitTexture() const;
    void PostInit(uint32_t time, const CClientObjCreate& create, bool reenable);
    C3Vector GetGroundNormal();

    private:
    // Local normal-cache/pose word;replicated CORPSE_FIELD_FLAGS is separate.
    enum RUNTIME_FLAGS : uint32_t {
        NONE = 0,
        GROUND_NORMAL_CACHED = 0x1,
        DROWNED_POSE = 0x2,
    };

    /* 0270 */ CCharacterComponent* m_component;
    /* 0274 */ C3Vector m_groundNormal;
    /* 0280 */ uint32_t m_corpseFlags;
    /* 0284 */ float m_selectionRadius;
    /* 0288 */ CM2Model* m_lootEffect;
    /* 028C */ uint8_t unknown_028C [4];

    bool IsClaimed() const {
        return (this->m_corpse->m_corpseFieldFlags & CORPSE_FIELD_FLAGS::CORPSE_FIELD_FLAG_BONES) != 0;
    }

    void ShowLootEffect();
    void OnFlagChanged(uint32_t oldFlags);
    void OnDynamicFlagChanged(uint32_t oldFlags);
    // Private static callback declarations (inferred).
    static int32_t CorpseDynamicFlagUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param);
    static int32_t CorpseFlagUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param);
};

#endif
