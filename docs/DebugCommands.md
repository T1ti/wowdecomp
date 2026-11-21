# Debug and developer commands

This page lists the console commands, CVars and Lua functions that only exist when the client is built with
`WOWDECOMP_DEBUG_FEATURES` or `WOWDECOMP_TOOL_FEATURES`. They come from debug code that was stripped from the 3.3.5
retail build. Most were recovered from 0.5.3, where the code was still compiled in.

Retail console commands (`port`, `worldport`, `perf` and the like) are not listed here. They are always compiled.

Status values:

- **works**: implemented.
- **no-op**: implemented, but does nothing on purpose (same as 0.5.3).
- **stub**: registered, but the handler is still a TODO.
- **flag only**: sets a world flag that nothing reads yet.

Open the console with the `~` key (`²` on AZERTY keyboards).

## Rendering / graphics device (`WoW/Source/Client_Debug.cpp`, DEBUG)

| Command | Status | Notes |
|---|---|---|
| `light` | works | Toggles lighting. |
| `fog` | works | Toggles fog. |
| `DepthTest` | works | Toggles the depth test. |
| `DepthSet` | works | Toggles depth writes. |
| `culling` | works | Toggles back-face culling. |
| `dblbuffer` | works | Toggles double buffering. |
| `resxy`, `resmode`, `bitdepth`, `3dapi` | no-op | Empty in 0.5.3 too. |
| `showbounds [box]` | works | Toggles the bounding sphere, or the box with `box`, on the tracked object's model, or on the player's model if nothing is tracked. Uses the Gxu debug-draw module. |
| `loc`, `dloc`, `tloc` | stub | Prints or sets the player location. |
| `facing`, `dfacing` | stub | Prints or sets the facing. |
| `drawlog`, `animlog` | stub | M2 render/animation logs (0.5.3 wrote RenderLog.txt and AnimLog.txt). |
| `motionBlend`, `mem`, `forceanim`, `resetanim`, `dirwtf` | stub | |
| `debugTargetInfo` (CVar) | stub | No callback yet. |
| `showGUIDs` (CVar) | stub | No callback yet. |
| `ErrorFileLog` (CVar) | stub | |

## World (`WoW/Source/WorldClient/World_Debug.cpp`, DEBUG)

| Command | Status | Notes |
|---|---|---|
| `showTerrain` | works | Toggles terrain. |
| `showDoodads` | works | Toggles doodads. |
| `showMapObjs` | works | Toggles WMOs. |
| `showMapObjLight` | works | Toggles WMO lighting. |
| `showMapObjTex` | works | Toggles WMO textures. |
| `waterShow` / `showWater` | works | Toggles water. |
| `showPortals` | works | Draws WMO portals. |
| `portalVis` | works | Toggles portal culling. |
| `showTris` | works | Wireframe. |
| `showNormals` | works | |
| `showQuery` | works | |
| `proj` | works | |
| `detailDoodadTest` | works | |
| `showCollision` | works | Draws the collision mesh (translucent triangles and red face normals) of every drawn M2 that has collision data. |
| `showAABoxes` | works | Draws the collision box (translucent cyan) of every drawn M2 that has collision data. 0.5.3 called this "Terrain doodads AA Box visuals". |
| `showMapObjBSP`, `debugBSP` | flag only | Should draw the WMO BSP (0.5.3 `RenderGroupBsp`, 0x0069DF60); no reader yet. |
| `showCrappyBatches` | flag only | No reader yet. |
| `debugZones` | flag only | Sets a local bit only. |
| `groupOnly` | stub | |
| `mapObjLightMode` | stub | |
| `waterMaxLOD`, `waterWaves`, `waterSpecular` | stub | |
| `enumTextures`, `enumTextureGxCache` | stub | |

0.5.3 compiled some of these handlers without registering them (`showTerrain` and the commands after it in
`World_Debug.cpp`). Their command names were made up from the handler names.

## Sky (`WoW/Source/WorldClient/DayNight_Debug.cpp`, DEBUG)

| Command | Status |
|---|---|
| `SkyShow` | stub |
| `SkyCloudLayers` | stub |
| `SkyCloudDensity` | stub |

## Units / combat (`WoW/Source/Object/ObjectClient/Unit_C_Debug.cpp`, DEBUG)

| Command | Status | Notes |
|---|---|---|
| `DebugTargetPath` (CVar) | works | When set to 1, draws the movement spline of the locked target and of the player: green spheres at the spline points, white lines between them, and a yellow line to the final destination. |
| `CombatDebugForceActionOn` / `Off` | stub | |
| `CombatDebugForceActionOtherOn` / `Off` | stub | |
| `CombatDebugShowFlags` | stub | |
| `DumpDeathHoldLogs` | stub | |
| `playercombatlogdebug` | stub | |
| `showEffectsStandalone` (CVar) | stub | |

## Sound (`WoW/Source/SoundInterface/SndDebug.cpp`, DEBUG)

| Command | Status |
|---|---|
| `SndDebugPingSound` | stub |
| `SndDebugRoomType` | stub |
| `env` | stub |
| `envlist` | stub |

## UI (DEBUG)

| Command | File | Status |
|---|---|---|
| `PLightEnable`, `PLightInfo`, `PLightOmni`, `PLightAmbColor`, `PLightAmbIntens`, `PLightDirColor`, `PLightDirIntens`, `PLightDirPos` | `UIUtil/PortraitButton_Debug.cpp` | stub. Portrait lighting. |
| `showsmartrects` (CVar) | `UIUtil/SmartScreenRect_Debug.cpp` | stub |
| `SeeIfWorldFrameSucks` | `Ui/WorldFrame_Debug.cpp` | stub |

## Tool build (`WOWDECOMP_TOOL_FEATURES`)

| Command | File | Status | Notes |
|---|---|---|---|
| `debugDrawTest` | `Ui/WorldFrame_DebugDrawTest.cpp` | works | Toggles a test scene in front of the player that exercises the Gxu debug-draw module: every shape and style, sphere LODs, the 4.1 editor shapes (spawn points with orientation rings, early-editor half spheres, a flying path), lines, an oriented box, a texture and text. |

## Lua functions (`WoW/Source/Ui/GameUI_Scripts.cpp`, DEBUG)

In the release build, these functions are bound to the shared return-0 stub.

| Function | Status | Notes |
|---|---|---|
| `TogglePlayerBounds()` | works | Toggles the bounding sphere on the player model. |
| `ToggleTris()` | no-op | Already a return-0 stub in 0.5.3. |
| `TogglePortals()` | no-op | Already a return-0 stub in 0.5.3. |
| `ToggleCollision()` | no-op | Already a return-0 stub in 0.5.3. |
| `ToggleCollisionDisplay()` | no-op | Already a return-0 stub in 0.5.3. |
| `GetDebugStats()` | stub | |
| `TogglePerformanceDisplay()`, `TogglePerformanceValues()`, `TogglePerformancePause()`, `ResetPerformanceValues()` | stub | |
| `GetExtendedItemInfo()` | stub | Debug-only status is inferred from its name; there is no 0.5.3 reference. |

## Debug-draw features with no command yet

- Model hit-test geometry (`CM2Model::ShowHitTestGeometry`, 0.5.3 `ModelShowHitTestGeometry`).
- Unselectable geometry (`CM2Model::ShowUnselectable`).
- WMO BSP display (`showMapObjBSP` / `debugBSP`).
- Zone bounds (`debugZones`).
