#include <cstdint>
#include <cstring>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/DB/DBClient/DBClient.h"

// STATIC: 0x00C5DEA8
static TSFixedArray<const ItemSubClassRec*> s_itemSubClassList[ITEM_CLASS::NUM_ITEM_CLASSES];

// VERIFY: e9c59b21 manual
// OFFSET: 0x00681090
const ItemSubClassRec* SDBItemSubclassGetSubClassRec(ITEM_CLASS classID, uint32_t subClassID) {
    uint32_t classIndex = static_cast<uint32_t>(classID);
    if (classIndex >= sizeof(s_itemSubClassList) / sizeof(s_itemSubClassList[0]) || subClassID >= s_itemSubClassList[classIndex].Count()) {
        return nullptr;
    }

    return s_itemSubClassList[classIndex][subClassID];
}

// VERIFY: 600df192 manual
// OFFSET: 0x006810C0
void SDBItemSubclassInitialize() {
    int32_t counts[sizeof(s_itemSubClassList) / sizeof(s_itemSubClassList[0])];
    memset(counts, 0, sizeof(counts));

    for (int32_t i = 0; i < g_itemSubClassDB.GetNumRecords(); i++) {
        const ItemSubClassRec* rec = i >= 0 && i < g_itemSubClassDB.GetNumRecords() ? &g_itemSubClassDB.m_records[i] : nullptr;

#if defined(WOWDECOMP_ARCH_64)
        if (static_cast<uint32_t>(rec->m_classID) >= sizeof(s_itemSubClassList) / sizeof(s_itemSubClassList[0]) || rec->m_subClassID < 0 || rec->m_subClassID == INT32_MAX) {
            continue;
        }
#endif

        STORM_ASSERT(rec->m_classID < ITEM_CLASS::NUM_ITEM_CLASSES); // 12213: "rec->m_classID < NUM_ITEMCLASSES"

        if (rec->m_classID >= 0 && counts[rec->m_classID] <= rec->m_subClassID) {
            counts[rec->m_classID] = rec->m_subClassID + 1;
        }
    }

    for (uint32_t i = 0; i < sizeof(s_itemSubClassList) / sizeof(s_itemSubClassList[0]); i++) {
        s_itemSubClassList[i].SetCount(counts[i]);
    }

    for (int32_t i = 0; i < g_itemSubClassDB.GetNumRecords(); i++) {
        const ItemSubClassRec* rec = i >= 0 && i < g_itemSubClassDB.GetNumRecords() ? &g_itemSubClassDB.m_records[i] : nullptr;

#if defined(WOWDECOMP_ARCH_64)
        if (static_cast<uint32_t>(rec->m_classID) >= sizeof(s_itemSubClassList) / sizeof(s_itemSubClassList[0]) || static_cast<uint32_t>(rec->m_subClassID) >= s_itemSubClassList[rec->m_classID].Count()) {
            continue;
        }
#endif

        if (rec->m_classID >= 0) {
            STORM_ASSERT(s_itemSubClassList[rec->m_classID].Count() > static_cast<uint32_t>(rec->m_subClassID)); // 12213: "s_itemSubClassList[rec->m_classID].Count() > (uint)rec->m_subClassID"
            s_itemSubClassList[rec->m_classID][rec->m_subClassID] = rec;
        }
    }
}
