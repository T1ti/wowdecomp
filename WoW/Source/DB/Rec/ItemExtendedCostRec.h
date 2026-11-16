#ifndef WOW_SOURCE_DB_REC_ITEMEXTENDEDCOSTREC_H
#define WOW_SOURCE_DB_REC_ITEMEXTENDEDCOSTREC_H

#include <cstddef>
#include <cstdint>

class SFileImpl;

// Required-item columns per ItemExtendedCost.dbc record (inferred name).
static const uint32_t MAX_ITEM_EXTENDED_COST_ITEMS = 5;

class ItemExtendedCostRec {
    public:
    static const int32_t s_numColumns = 16;
    static const int32_t s_rowSize = 64;
    static const bool s_generatedID = false;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_honorPoints;
    /* 0008 */ int32_t m_arenaPoints;
    /* 000C */ int32_t m_arenaBracket;
    /* 0010 */ int32_t m_itemID [MAX_ITEM_EXTENDED_COST_ITEMS];
    /* 0024 */ int32_t m_itemCount [MAX_ITEM_EXTENDED_COST_ITEMS];
    /* 0038 */ int32_t m_requiredArenaRating;
    /* 003C */ int32_t m_itemPurchaseGroup;

    ItemExtendedCostRec();
    ~ItemExtendedCostRec();
    static const char* GetFilename();
    bool Read(SFileImpl* file, const char* stringBuffer);
};

#endif
