#ifndef WOW_SOURCE_UI_CURRENCYTYPES_H
#define WOW_SOURCE_UI_CURRENCYTYPES_H

#include <cstdint>
#include <windows.h>

struct lua_State;

struct CurrencyCategoryInfo {
    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_expanded;
    /* 0008 */ const char* m_name;
    /* 000C */ int32_t m_flags;
};

struct CurrencyListEntry {
    /* 0000 */ const char* m_name;
    /* 0004 */ int32_t m_itemID;
    /* 0008 */ int32_t m_categoryID;
    /* 000C */ int32_t m_isHeader;
    /* 0010 */ int32_t m_isExpanded;
    /* 0014 */ int32_t m_isWatched;
};

class CurrencyTypesRec;

class CGCurrencyTypes {
    public:
    static void LeaveWorld();
    static CurrencyTypesRec* GetCurrencyRec(int32_t index);
    static BOOL IsCurrencyShown(int32_t index);
    static CurrencyListEntry* GetDisplayListInfo(int32_t index);
    static int32_t m_listSize;
    static CurrencyListEntry m_list[95];
    static CurrencyCategoryInfo m_categories[32];
    static int32_t m_numCategories;
    static void BuildDisplayList();
    static void SetCategoryExpand(int32_t index, int32_t expand);
    static void SetUnused(int32_t index, int32_t unused);
    static void SetBackpack(int32_t index, int32_t backpack);
    static CurrencyTypesRec* GetBackpackCurrencyInfo(int32_t index);
    static int32_t CategoryHasKnownTypes(int32_t index, uint32_t known1, uint32_t known2, uint32_t unused1, uint32_t unused2);
    static int32_t CurrencyCategorySortCallback(const void* a, const void* b);
    static int32_t DisplayListSortCallback(const void* a, const void* b);
    static void InitializeGame();
    static void ShutdownGame();
    static void EnterWorld();
};

int32_t Script_GetCurrencyListInfo(lua_State* L);
void CurrencyTypesRegisterScriptFunctions();
void CurrencyTypesUnregisterScriptFunctions();

#endif
