#ifndef WOW_SOURCE_GLUE_REALMLIST_H
#define WOW_SOURCE_GLUE_REALMLIST_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

class Cfg_CategoriesRec;

struct RealmCategory {
    /* 0000 */ Cfg_CategoriesRec* m_category;
    /* 0004 */ TSGrowableArray<uint32_t> m_realms;
    /* 0014 */ uint32_t m_numRealms;
};

// Realm list sort keys; names follow the SortRealms type strings.
enum REALM_SORT_CRITERION : int32_t {
    REALM_SORT_CHARACTERS = 0, // "characters"
    REALM_SORT_LOAD = 1, // "load"
    REALM_SORT_NAME = 2, // "name"
    REALM_SORT_MODE = 3, // "mode"
    NUM_REALM_SORT_CRITERIA = 4,
};

struct RealmSortCriterion {
    /* 0000 */ int32_t m_criterion; // REALM_SORT_CRITERION
    /* 0004 */ int32_t m_reverse;
};

struct REALM_INFO;

class CRealmList {
    public:
    static int32_t m_selectedCategory;
    static int32_t m_preferredCategory;
    static float m_avgLoad;
    static float m_stdDevLoad;
    static RealmSortCriterion m_sortCriteria[NUM_REALM_SORT_CRITERIA];
    static TSFixedArray<RealmCategory*> m_categories;

    static void Initialize();
    static void SetPreferredInfo(int32_t index, int32_t pvp, int32_t roleplaying);
    static void Shutdown();
    static void SortRealms(int32_t criterion);
    static void UpdateList();
};

uint32_t GetCategoryIndex(int32_t visibleIndex);
float GetRealmPopulation(REALM_INFO* realm);
int32_t QSortCategory(const void* a, const void* b);
void RealmListRegisterScriptFunctions();
void RealmListUnregisterScriptFunctions();

#endif
