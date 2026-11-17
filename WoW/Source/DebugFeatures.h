#ifndef WOW_SOURCE_DEBUGFEATURES_H
#define WOW_SOURCE_DEBUGFEATURES_H

// Debug features: shells for 0.5.3 debug commands, CVars and render modes that have no counterpart in the 3.3.5
// release build. Built only with the WOWDECOMP_DEBUG_FEATURES CMake option; the release build stays unchanged.
// Each shell names its 0.5.3 reference; the bodies are TODO because the 0.5.3 code targets the MDX/HMODEL engine
// and is not compatible with the 3.3.5 codebase.

#ifdef WOWDECOMP_DEBUG_FEATURES

#include <cstdint>
#include "WoW/Source/SoundInterface/Types.h"

class C3Vector;
class CDataStore;
class CFacet;
struct WOWGUID;

template <class T>
class TSGrowableArray;

// Client_Debug.cpp
void ClientDebugRegisterConsoleCommands();
void GameClientCommandsDebugInstall();
void GameClientCommandsDebugUninstall();

// Object/CollisionInfo_Debug.cpp
enum FACET_COLOR {
    FACET_UNTESTED = 0,
    FACET_TESTED_UNTOUCHED = 1,
    FACET_TESTED_TOUCHED = 2,
    FACET_BLOCKING = 3,
    NUM_FACET_COLORS = 4,
};

void CollisionInfoSetWatchGUID(const WOWGUID& guid);
void CollisionInfoReset();
void CollisionInfoSetFaces(const WOWGUID& guid, const TSGrowableArray<CFacet>& facets);
void CollisionInfoColorFace(uint32_t face, FACET_COLOR color);
void CollisionInfoColorSweep(uint32_t hitFace);
void CollisionInfoSetFallBox(const C3Vector& position, float radius, float height);
void CollisionInfoAddBox(const C3Vector& minimum, const C3Vector& maximum);
void CollisionInfoAddVector(const C3Vector& start, const C3Vector& end);
int32_t ToggleCollisionInfo();
void RenderCollisionInfo();

// Object/ObjectClient/Unit_C_Debug.cpp
void UnitDebugRegisterConsoleCommands();
void UnitCombatDebugLogEnable(int32_t enable);
void UnitDebugCombatLogOnEnable(int32_t enable);

// WorldClient/WorldRender_Debug.cpp
void WorldDebugRenderLate();

// WorldClient/World_Debug.cpp
void ZoneDebugInitialize();
bool ZoneDebugIsInCurrentZone(float x, float y);

// WorldClient/DayNight_Debug.cpp
void DayNightDebugRegisterConsoleCommands();

// UIUtil/PortraitButton_Debug.cpp
void PortraitDebugRegisterConsoleCommands();

// UIUtil/SmartScreenRect_Debug.cpp
void SmartScreenRectInitialize();

// Ui/WorldFrame_Debug.cpp
void WorldFrameDebugInitialize();

// SoundInterface/SndDebug.cpp
void SndDebugInitialize();
void SndDebugShutdown();
void SndDebugDungeonTransition(int32_t indoors, uint32_t id);
void SndDebugRegisterContinent(uint32_t continentId);
void SndSetRoomType(SNDROOMTYPE type);

#endif

#endif
