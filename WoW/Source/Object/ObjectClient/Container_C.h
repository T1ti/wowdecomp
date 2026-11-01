#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_CONTAINER_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_CONTAINER_C_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/Container.h"
#include "WoW/Source/Object/ObjectClient/Bag_C.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"

class CGContainer_C : public CGItem_C, public CGContainer {
    public:
    static void Initialize();
    /* 03E8 */ TSList<void> m_containerMirrorLists [CGContainerData::NUM_FIELDS];
    /* 0760 */ CGBag_C m_bag;

    ~CGContainer_C() override;
    const CGBag_C* GetBag() const override;
    CGBag_C* GetBag() override;
    void Reenable() override;

    CGContainer_C(uint32_t time, const CClientObjCreate& create);
    int32_t GetFreeSlots() const;
    void SetStorage(uint32_t* storage, uint32_t* saved);
};

#endif
