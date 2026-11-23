#include <cstddef>
#include <cstdint>
#include "WoW/Common/ComponentCore/ComponentUtils.h"
#include "WoW/Common/Types.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/CharSectionsRec.h"
#include "WoW/Source/DB/Rec/CharacterFacialHairStylesRec.h"

// VERIFY: 764bd294 manual
// OFFSET: 0x004F39A0
bool ComponentFlagsMatch(CharSectionsRec::SECTION_FLAGS flags, COMPONENT_SELECTION selection) {
    switch (selection) {
        case COMPONENT_SELECTION_0:
            return (flags & CharSectionsRec::SECTION_FLAGS::PLAYER) && !(flags & CharSectionsRec::SECTION_FLAGS::DEATH_KNIGHT_OR_NPC_SKIN);

        case COMPONENT_SELECTION_1:
            return (flags & CharSectionsRec::SECTION_FLAGS::PLAYER) && (flags & CharSectionsRec::SECTION_FLAGS::DEATH_KNIGHT_VARIANTS) && !(flags & CharSectionsRec::SECTION_FLAGS::NPC_SKIN);

        case COMPONENT_SELECTION_2:
            return (flags & CharSectionsRec::SECTION_FLAGS::PLAYER_OR_BARBERSHOP) && !(flags & CharSectionsRec::SECTION_FLAGS::DEATH_KNIGHT_OR_NPC_SKIN);

        case COMPONENT_SELECTION_3:
            return (flags & CharSectionsRec::SECTION_FLAGS::PLAYER_OR_BARBERSHOP) && (flags & CharSectionsRec::SECTION_FLAGS::DEATH_KNIGHT_VARIANTS) && !(flags & CharSectionsRec::SECTION_FLAGS::NPC_SKIN);

        case COMPONENT_SELECTION_4:
            return true;

        case COMPONENT_SELECTION_5:
            return !(flags & CharSectionsRec::SECTION_FLAGS::DEATH_KNIGHT_OR_NPC_SKIN);

        case COMPONENT_SELECTION_6:
            return (flags & CharSectionsRec::SECTION_FLAGS::DEATH_KNIGHT_VARIANTS) && !(flags & CharSectionsRec::SECTION_FLAGS::NPC_SKIN);

        default:
            return false;
    }
}

// VERIFY: e1c0c168 manual
// OFFSET: 0x004F3A40
COMPONENT_SELECTION GetSelectionFromContext(COMPONENT_CONTEXT context, int32_t classID) {
    switch (context) {
        case COMPONENT_CONTEXT_1:
            return static_cast<COMPONENT_SELECTION>((classID == CLASS_DEATH_KNIGHT) + COMPONENT_SELECTION_2);

        case COMPONENT_CONTEXT_2:
            return COMPONENT_SELECTION_4;

        case COMPONENT_CONTEXT_3:
            return static_cast<COMPONENT_SELECTION>((classID == CLASS_DEATH_KNIGHT) + COMPONENT_SELECTION_5);

        default:
            return static_cast<COMPONENT_SELECTION>(classID == CLASS_DEATH_KNIGHT);
    }
}

// VERIFY: 8a1f047c manual
// OFFSET: 0x004F3A90
COMPONENT_CONTEXT GetContextFromSelection(COMPONENT_SELECTION selection) {
    switch (selection) {
        case COMPONENT_SELECTION_2:
        case COMPONENT_SELECTION_3:
            return COMPONENT_CONTEXT_1;

        case COMPONENT_SELECTION_4:
            return COMPONENT_CONTEXT_2;

        case COMPONENT_SELECTION_5:
        case COMPONENT_SELECTION_6:
            return COMPONENT_CONTEXT_3;

        default:
            return COMPONENT_CONTEXT_0;
    }
}

// VERIFY: 28ddfb49 manual
// OFFSET: 0x004F3AE0
int32_t ComponentGetNumVariations(const st_race* lookup, int32_t raceID, int32_t sexID, CHARTEXTUREVARIATIONS variation) {
    const CharVariationList& list = lookup[raceID * 2 + sexID].sections[variation];

    if (!list.variationArray) {
        return 0;
    }

    return list.variationCount;
}

// VERIFY: 725a4a25 auto
// OFFSET: 0x004F3B10
int32_t ComponentGetNumColors(const st_race* lookup, int32_t raceID, int32_t sexID, CHARTEXTUREVARIATIONS variation, int32_t variationIndex) {
    const CharVariationList& list = lookup[raceID * 2 + sexID].sections[variation];

    if (!list.variationArray || variationIndex >= list.variationCount || list.variationCount <= 0 || !list.variationArray[variationIndex].m_color) {
        return 0;
    }

    return list.variationArray[variationIndex].colorCount;
}

// VERIFY: 387cd93c manual
// OFFSET: 0x004F3B50
bool ComponentValidateBase(const st_race* lookup, int32_t raceID, int32_t sexID, CHARTEXTUREVARIATIONS variation, int32_t variationIndex, int32_t colorIndex) {
    if (static_cast<uint32_t>(variation) >= CHARTEXTUREVARIATIONS_NUM || variationIndex < 0) {
        return false;
    }

    const CharVariationList& list = lookup[raceID * 2 + sexID].sections[variation];

    if (variationIndex >= list.variationCount || !list.variationCount || colorIndex < 0) {
        return false;
    }

    int32_t numColors = list.variationArray[variationIndex].colorCount;

    return colorIndex < numColors && numColors;
}

// VERIFY: 04a6cba3 manual
// OFFSET: 0x004F3BA0
CharSectionsRec* ComponentGetSectionsRecord(const st_race* lookup, int32_t raceID, int32_t sexID, COMPONENT_VARIATIONS section, int32_t variationIndex, int32_t colorIndex, bool* found) {
    STORM_ASSERT(section >= 0 && section < NUM_COMPONENT_VARIATIONS);

    if (static_cast<uint32_t>(section) < NUM_COMPONENT_VARIATIONS && variationIndex >= 0) {
        const CharVariationList& list = lookup[raceID * 2 + sexID].sections[section];

        if (variationIndex < list.variationCount && list.variationCount && colorIndex >= 0) {
            int32_t numColors = list.variationArray[variationIndex].colorCount;

            if (colorIndex < numColors && numColors) {
                if (found) {
                    *found = true;
                }

                return list.variationArray[variationIndex].m_color[colorIndex];
            }
        }
    }

    if (found) {
        *found = false;
    }

    return nullptr;
}

// VERIFY: e6e73c05 manual
// OFFSET: 0x004F3C10
bool ComponentValidateSkin(const st_race* lookup, int32_t raceID, int32_t sexID, int32_t classID, int32_t colorIndex, COMPONENT_CONTEXT context) {
    const CharVariationList& list = lookup[raceID * 2 + sexID].sections[CHARTEXTUREVAR_SKIN];

    if (list.variationCount <= 0 || colorIndex < 0) {
        return false;
    }

    const CharColorList& colors = list.variationArray[0];

    if (colorIndex >= colors.colorCount || !colors.colorCount || !colors.m_color[colorIndex]) {
        return false;
    }

    return ComponentFlagsMatch(colors.m_color[colorIndex]->m_flags, GetSelectionFromContext(context, classID));
}

// VERIFY: 4e4d2364 manual
// OFFSET: 0x004F3C70
bool ComponentValidateHair(const st_race* lookup, int32_t raceID, int32_t sexID, int32_t classID, int32_t colorIndex, int32_t variationIndex, COMPONENT_CONTEXT context) {
    if (variationIndex < 0) {
        return false;
    }

    const CharVariationList& list = lookup[raceID * 2 + sexID].sections[CHARTEXTUREVAR_HAIR];

    if (variationIndex >= list.variationCount || !list.variationCount || colorIndex < 0) {
        return false;
    }

    const CharColorList& colors = list.variationArray[variationIndex];

    if (colorIndex >= colors.colorCount || !colors.colorCount || !colors.m_color[colorIndex]) {
        return false;
    }

    return ComponentFlagsMatch(colors.m_color[colorIndex]->m_flags, GetSelectionFromContext(context, classID));
}

// VERIFY: e3e5bf4a manual
// OFFSET: 0x004F3CE0
bool ComponentValidateFace(const st_race* lookup, int32_t raceID, int32_t sexID, int32_t classID, int32_t colorIndex, int32_t variationIndex, COMPONENT_CONTEXT context) {
    if (variationIndex < 0) {
        return false;
    }

    const CharVariationList& list = lookup[raceID * 2 + sexID].sections[CHARTEXTUREVAR_FACE];

    if (variationIndex >= list.variationCount || !list.variationCount || colorIndex < 0) {
        return false;
    }

    const CharColorList& colors = list.variationArray[variationIndex];

    if (colorIndex >= colors.colorCount || !colors.colorCount || !colors.m_color[colorIndex]) {
        return false;
    }

    return ComponentFlagsMatch(colors.m_color[colorIndex]->m_flags, GetSelectionFromContext(context, classID));
}

// VERIFY: 9668fbd0 auto
// OFFSET: 0x004F3D50
bool ComponentValidateFacialFeature(const st_race* lookup, const uint32_t* facialFeatures, int32_t raceID, int32_t sexID, int32_t classID, int32_t colorIndex, int32_t variationIndex, COMPONENT_CONTEXT context) {
    if (variationIndex < 0) {
        return false;
    }

    int32_t index = raceID * 2 + sexID;

    if (static_cast<uint32_t>(variationIndex) > facialFeatures[index]) {
        return false;
    }

    const CharVariationList& list = lookup[index].sections[CHARTEXTUREVAR_FACIALHAIR];

    if (variationIndex >= list.variationCount || !list.variationCount || colorIndex < 0) {
        return true;
    }

    const CharColorList& colors = list.variationArray[variationIndex];

    if (colorIndex >= colors.colorCount || !colors.colorCount) {
        return true;
    }

    if (!colors.m_color[colorIndex]) {
        return false;
    }

    return ComponentFlagsMatch(colors.m_color[colorIndex]->m_flags, GetSelectionFromContext(context, classID));
}

static constexpr int32_t UNITSEX_NUM_SEXES = 2;

// VERIFY: 432f1f55 manual
// OFFSET: 0x004F3DD0
int32_t BuildComponentArray(uint32_t count, st_race** lookupOut) {
    if (!lookupOut) {
        return 0;
    }

    st_race* varArray = new (__FILE__, __LINE__) st_race[count];

    int32_t variationIndex = 0;
    CharSectionsRec* first = (0 >= g_charSectionsDB.GetNumRecords()) ? nullptr : &g_charSectionsDB.m_records[0];
    STORM_ASSERT(first); // 12213: "rec"
    int32_t raceID = first->m_raceID;
    int32_t sexID = first->m_sexID;
    COMPONENT_VARIATIONS sectionIndex = first->m_baseSection;

    for (int32_t i = 0; i < g_charSectionsDB.GetNumRecords(); i++) {
        CharSectionsRec* rec = (i < 0 || i >= g_charSectionsDB.GetNumRecords()) ? nullptr : &g_charSectionsDB.m_records[i];
        STORM_ASSERT(rec);

        if (rec->m_baseSection >= NUM_COMPONENT_VARIATIONS) {
            continue;
        }

        if (raceID != rec->m_raceID || sexID != rec->m_sexID || sectionIndex != rec->m_baseSection) {
            CharVariationList& list = varArray[raceID * 2 + sexID].sections[sectionIndex];
            list.variationCount = variationIndex + 1;

            if (list.variationCount > 0) {
                list.variationArray = new (__FILE__, __LINE__) CharColorList[list.variationCount];
            }

            variationIndex = 0;
        }

        if (variationIndex <= rec->m_variationIndex) {
            variationIndex = rec->m_variationIndex;
        }

        raceID = rec->m_raceID;
        sexID = rec->m_sexID;
        sectionIndex = rec->m_baseSection;
        STORM_ASSERT(sectionIndex < NUM_COMPONENT_VARIATIONS);
    }

    CharVariationList& lastList = varArray[raceID * 2 + sexID].sections[sectionIndex];
    lastList.variationCount = variationIndex + 1;

    if (lastList.variationCount > 0) {
        lastList.variationArray = new (__FILE__, __LINE__) CharColorList[lastList.variationCount];
    }

    sexID = 0;
    raceID = RACE_HUMAN;
    sectionIndex = VARIATION_SKIN;
    variationIndex = 0;
    int32_t colorIndex = 0;

    for (int32_t i = 0; i < g_charSectionsDB.GetNumRecords(); i++) {
        CharSectionsRec* rec = (i < 0 || i >= g_charSectionsDB.GetNumRecords()) ? nullptr : &g_charSectionsDB.m_records[i];
        STORM_ASSERT(rec);

        if (rec->m_baseSection >= NUM_COMPONENT_VARIATIONS) {
            continue;
        }

        if (raceID != rec->m_raceID || sexID != rec->m_sexID || sectionIndex != rec->m_baseSection || variationIndex != rec->m_variationIndex) {
            STORM_ASSERT(sectionIndex <= NUM_COMPONENT_VARIATIONS);
            CharVariationList& list = varArray[raceID * 2 + sexID].sections[sectionIndex];

            if (list.variationCount > 0) {
                CharColorList& colors = list.variationArray[variationIndex];
                colors.colorCount = colorIndex + 1;

                if (colors.colorCount > 0) {
                    colors.m_color = new (__FILE__, __LINE__) CharSectionsRec*[colorIndex + 1]();
                }
            }

            colorIndex = 0;
        }

        if (colorIndex <= rec->m_colorIndex) {
            colorIndex = rec->m_colorIndex;
        }

        raceID = rec->m_raceID;
        sexID = rec->m_sexID;
        sectionIndex = rec->m_baseSection;
        variationIndex = rec->m_variationIndex;
    }

    CharVariationList& lastColors = varArray[raceID * 2 + sexID].sections[sectionIndex];

    if (lastColors.variationCount > 0) {
        CharColorList& colors = lastColors.variationArray[variationIndex];
        colors.colorCount = colorIndex + 1;

        if (colors.colorCount > 0) {
            colors.m_color = new (__FILE__, __LINE__) CharSectionsRec*[colorIndex + 1]();
        }
    }

    for (int32_t i = 0; i < g_charSectionsDB.GetNumRecords(); i++) {
        CharSectionsRec* rec = (i < 0 || i >= g_charSectionsDB.GetNumRecords()) ? nullptr : &g_charSectionsDB.m_records[i];

        if (rec->m_baseSection >= NUM_COMPONENT_VARIATIONS) {
            continue;
        }

        sectionIndex = rec->m_baseSection;
        variationIndex = rec->m_variationIndex;
        colorIndex = rec->m_colorIndex;
        CharVariationList& list = varArray[rec->m_raceID * UNITSEX_NUM_SEXES + rec->m_sexID].sections[sectionIndex];
        STORM_ASSERT(variationIndex < varArray[(rec->m_raceID * UNITSEX_NUM_SEXES + rec->m_sexID)].sections[sectionIndex].variationCount);

        if (list.variationCount > 0 && variationIndex < list.variationCount) {
            CharColorList& colors = list.variationArray[variationIndex];
            STORM_ASSERT(colorIndex < varArray[(rec->m_raceID * UNITSEX_NUM_SEXES + rec->m_sexID)].sections[sectionIndex].variationArray[variationIndex].colorCount);

            if (colors.colorCount > 0 && colorIndex < colors.colorCount) {
                colors.m_color[colorIndex] = rec;
            }
        }
    }

    *lookupOut = varArray;

    return 1;
}

// VERIFY: 4f2a06af manual
// OFFSET: 0x004F41B0
int32_t CountFacialFeatures(uint32_t count, uint32_t** facialFeatures) {
    uint32_t* counts = static_cast<uint32_t*>(SMemAlloc(sizeof(uint32_t) * count, __FILE__, __LINE__, SMEM_FLAG_ZEROMEMORY));

    if (!counts) {
        return 0;
    }

    for (int32_t i = 0; i < g_characterFacialHairStylesDB.GetNumRecords(); i++) {
        CharacterFacialHairStylesRec* rec = (i < 0 || i >= g_characterFacialHairStylesDB.GetNumRecords()) ? nullptr : &g_characterFacialHairStylesDB.m_records[i];

        if (!rec) {
            return 0;
        }

        counts[rec->m_raceID * 2 + rec->m_sexID]++;
    }

    *facialFeatures = counts;

    return 1;
}
