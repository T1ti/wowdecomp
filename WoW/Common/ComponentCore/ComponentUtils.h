#ifndef WOW_COMMON_COMPONENTCORE_COMPONENTUTILS_H
#define WOW_COMMON_COMPONENTCORE_COMPONENTUTILS_H

#include <cstddef>
#include <cstdint>
#include "WoW/Common/ComponentCore/Types.h"
#include "WoW/Source/DB/Rec/CharSectionsRec.h"

struct st_race {
    /* 0000 */ CharVariationList sections [NUM_COMPONENT_VARIATIONS];
};

int32_t BuildComponentArray(uint32_t count, st_race** lookup);
bool ComponentFlagsMatch(CharSectionsRec::SECTION_FLAGS flags, COMPONENT_SELECTION selection);
int32_t ComponentGetNumColors(const st_race* lookup, int32_t raceID, int32_t sexID, CHARTEXTUREVARIATIONS variation, int32_t variationIndex);
int32_t ComponentGetNumVariations(const st_race* lookup, int32_t raceID, int32_t sexID, CHARTEXTUREVARIATIONS variation);
CharSectionsRec* ComponentGetSectionsRecord(const st_race* lookup, int32_t raceID, int32_t sexID, COMPONENT_VARIATIONS section, int32_t variationIndex, int32_t colorIndex, bool* found);
bool ComponentValidateBase(const st_race* lookup, int32_t raceID, int32_t sexID, CHARTEXTUREVARIATIONS variation, int32_t variationIndex, int32_t colorIndex);
bool ComponentValidateFace(const st_race* lookup, int32_t raceID, int32_t sexID, int32_t classID, int32_t colorIndex, int32_t variationIndex, COMPONENT_CONTEXT context);
bool ComponentValidateFacialFeature(const st_race* lookup, const uint32_t* facialFeatures, int32_t raceID, int32_t sexID, int32_t classID, int32_t colorIndex, int32_t variationIndex, COMPONENT_CONTEXT context);
bool ComponentValidateHair(const st_race* lookup, int32_t raceID, int32_t sexID, int32_t classID, int32_t colorIndex, int32_t variationIndex, COMPONENT_CONTEXT context);
bool ComponentValidateSkin(const st_race* lookup, int32_t raceID, int32_t sexID, int32_t classID, int32_t colorIndex, COMPONENT_CONTEXT context);
int32_t CountFacialFeatures(uint32_t count, uint32_t** facialFeatures);
COMPONENT_CONTEXT GetContextFromSelection(COMPONENT_SELECTION selection);
COMPONENT_SELECTION GetSelectionFromContext(COMPONENT_CONTEXT context, int32_t classID);

#endif
