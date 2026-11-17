#include "WoW/Source/DebugFeatures.h"
#include "WoW/Source/WorldClient/World.h"
#include "Engine/Source/ConsoleBase/ConsoleCommand.h"

#ifdef WOWDECOMP_DEBUG_FEATURES

// Debug feature: world console command.
// 0.5.3 reference: CWorld::ConsoleCommand_MapObjLightMode (0x00665B10).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
int32_t CWorld::ConsoleCommand_MapObjLightMode(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: world console command.
// 0.5.3 reference: CWorld::ConsoleCommand_WaterShow (0x00665CA0).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
int32_t CWorld::ConsoleCommand_WaterShow(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: world console command.
// 0.5.3 reference: CWorld::ConsoleCommand_WaterMaxLOD (0x00665CF0).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
int32_t CWorld::ConsoleCommand_WaterMaxLOD(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: world console command.
// 0.5.3 reference: CWorld::ConsoleCommand_WaterWaves (0x00665D40).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
int32_t CWorld::ConsoleCommand_WaterWaves(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: world console command.
// 0.5.3 reference: CWorld::ConsoleCommand_WaterSpecular (0x00665D60).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
int32_t CWorld::ConsoleCommand_WaterSpecular(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: world console command.
// 0.5.3 reference: CWorld::ConsoleCommand_EnumTextures (0x00666150).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
int32_t CWorld::ConsoleCommand_EnumTextures(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: world console command.
// 0.5.3 reference: CWorld::ConsoleCommand_EnumTextureGxCache (0x00666210).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
int32_t CWorld::ConsoleCommand_EnumTextureGxCache(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: world render mode.
// 0.5.3 reference: CWorld::ConsoleCommand_ShowPortals (0x00665C20).
// Already unregistered in 0.5.3 (no console command name survives). Likely toggled CWorld::enables Enable_Portals (inferred from the name).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
int32_t CWorld::ConsoleCommand_ShowPortals(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: world render mode.
// 0.5.3 reference: CWorld::ConsoleCommand_PortalVis (0x00665C60).
// Already unregistered in 0.5.3 (no console command name survives). Likely toggled CWorld::enables Enable_PortalVis (inferred from the name).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
int32_t CWorld::ConsoleCommand_PortalVis(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: world render mode.
// 0.5.3 reference: CWorld::ConsoleCommand_ShowTris (0x00665E20).
// Already unregistered in 0.5.3 (no console command name survives). Likely toggled CWorld::enables Enable_Wireframe (inferred from the name).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
int32_t CWorld::ConsoleCommand_ShowTris(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: world render mode.
// 0.5.3 reference: CWorld::ConsoleCommand_ShowNormals (0x00665E50).
// Already unregistered in 0.5.3 (no console command name survives). Likely toggled CWorld::enables Enable_ShowNormals (inferred from the name).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
int32_t CWorld::ConsoleCommand_ShowNormals(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: world render mode.
// 0.5.3 reference: CWorld::ConsoleCommand_ShowCollision (0x00665930).
// Already unregistered in 0.5.3 (no console command name survives). Likely toggled CWorld::enables Enable_CollisionDebug (inferred from the name).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
int32_t CWorld::ConsoleCommand_ShowCollision(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: world render mode.
// 0.5.3 reference: CWorld::ConsoleCommand_ShowAABoxes (0x006658E0).
// Already unregistered in 0.5.3 (no console command name survives). Likely toggled CWorld::enables Enable_AABoxes (inferred from the name).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
int32_t CWorld::ConsoleCommand_ShowAABoxes(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: world render mode.
// 0.5.3 reference: CWorld::ConsoleCommand_ShowTerrain (0x00665860).
// Already unregistered in 0.5.3 (no console command name survives). Likely toggled CWorld::enables Enable_Chunks (inferred from the name).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
int32_t CWorld::ConsoleCommand_ShowTerrain(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: world render mode.
// 0.5.3 reference: CWorld::ConsoleCommand_ShowDoodads (0x006658A0).
// Already unregistered in 0.5.3 (no console command name survives). Likely toggled CWorld::enables Enable_Doodads (inferred from the name).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
int32_t CWorld::ConsoleCommand_ShowDoodads(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: world render mode.
// 0.5.3 reference: CWorld::ConsoleCommand_ShowMapObjs (0x00665BE0).
// Already unregistered in 0.5.3 (no console command name survives). Likely toggled CWorld::enables Enable_MapObjs (inferred from the name).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
int32_t CWorld::ConsoleCommand_ShowMapObjs(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: world render mode.
// 0.5.3 reference: CWorld::ConsoleCommand_ShowMapObjBSP (0x006657C0).
// Already unregistered in 0.5.3 (no console command name survives). Likely toggled CWorld::enables Enable_MapObjBSP (inferred from the name).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
int32_t CWorld::ConsoleCommand_ShowMapObjBSP(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: world render mode.
// 0.5.3 reference: CWorld::ConsoleCommand_ShowMapObjLight (0x00665AD0).
// Already unregistered in 0.5.3 (no console command name survives). Likely toggled CWorld::enables Enable_MapObjLight (inferred from the name).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
int32_t CWorld::ConsoleCommand_ShowMapObjLight(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: world render mode.
// 0.5.3 reference: CWorld::ConsoleCommand_ShowMapObjTex (0x00665B50).
// Already unregistered in 0.5.3 (no console command name survives). Likely toggled CWorld::enables Enable_MapObjTex (inferred from the name).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
int32_t CWorld::ConsoleCommand_ShowMapObjTex(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: world render mode.
// 0.5.3 reference: CWorld::ConsoleCommand_ShowCrappyBatches (0x00665B90).
// Already unregistered in 0.5.3 (no console command name survives). Likely toggled CWorld::enables Enable_CrappyBatches (inferred from the name).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
int32_t CWorld::ConsoleCommand_ShowCrappyBatches(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: world render mode.
// 0.5.3 reference: CWorld::ConsoleCommand_DebugBSP (0x00665810).
// Already unregistered in 0.5.3 (no console command name survives). Likely toggled CWorld::enables Enable_DebugBSP (inferred from the name).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
int32_t CWorld::ConsoleCommand_DebugBSP(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: world render mode.
// 0.5.3 reference: CWorld::ConsoleCommand_DebugZones (0x00665E80).
// Already unregistered in 0.5.3 (no console command name survives). Likely toggled CWorld::enables Enable_ZoneBounds (inferred from the name).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
int32_t CWorld::ConsoleCommand_DebugZones(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: world render mode.
// 0.5.3 reference: CWorld::ConsoleCommand_ShowQuery (0x00665F60).
// Already unregistered in 0.5.3 (no console command name survives).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
int32_t CWorld::ConsoleCommand_ShowQuery(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: world render mode.
// 0.5.3 reference: CWorld::ConsoleCommand_Proj (0x00665DF0).
// Already unregistered in 0.5.3 (no console command name survives).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
int32_t CWorld::ConsoleCommand_Proj(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: world render mode.
// 0.5.3 reference: CWorld::ConsoleCommand_DetailDoodadTest (0x00665FB0).
// Already unregistered in 0.5.3 (no console command name survives).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
int32_t CWorld::ConsoleCommand_DetailDoodadTest(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: world render mode.
// 0.5.3 reference: CWorld::ConsoleCommand_GroupOnly (0x00666040).
// Already unregistered in 0.5.3 (no console command name survives).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
int32_t CWorld::ConsoleCommand_GroupOnly(const char* command, const char* arguments) {
    return 1;
}

// Debug feature: zone debug.
// 0.5.3 reference: ZoneDebugInitialize (0x0060F1A0).
// 0.5.3 called it from WowClientInit; not called here yet.
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
void ZoneDebugInitialize() {
}

// 0.5.3 reference: ZoneDebugIsInCurrentZone (0x0060F290).
// TODO: not implemented; the 0.5.3 code is not compatible with the 3.3.5 codebase.
bool ZoneDebugIsInCurrentZone(float x, float y) {
    return false;
}

// Debug feature: registers the 0.5.3 CWorld::Initialize console commands missing from 3.3.5.
// The render modes 0.5.3 left unregistered stay unregistered.
void CWorld::DebugRegisterConsoleCommands() {
    ConsoleCommandRegister("mapObjLightMode", CWorld::ConsoleCommand_MapObjLightMode, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("waterShow", CWorld::ConsoleCommand_WaterShow, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("showWater", CWorld::ConsoleCommand_WaterShow, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("waterMaxLOD", CWorld::ConsoleCommand_WaterMaxLOD, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("waterWaves", CWorld::ConsoleCommand_WaterWaves, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("waterSpecular", CWorld::ConsoleCommand_WaterSpecular, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("enumTextures", CWorld::ConsoleCommand_EnumTextures, CATEGORY_GRAPHICS, nullptr);
    ConsoleCommandRegister("enumTextureGxCache", CWorld::ConsoleCommand_EnumTextureGxCache, CATEGORY_GRAPHICS, nullptr);
}

#endif
