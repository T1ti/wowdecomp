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
| `DebugTargetPath` (CVar) | works | When set to 1, draws the movement path of the locked target and of the player, as 0.5.3 did. For flying (Catmull-Rom) splines, including taxi flights, it draws green spheres at the control points and a white line along the curve. Ground moves get circles projected on the ground: green at the start, red at each path point. |
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
| `debugDrawTest` | `Ui/WorldFrame_DebugDrawTest.cpp` | works | Toggles a test scene in front of the player that exercises the Gxu debug-draw module: every shape and style, sphere LODs, lines, an oriented box, a texture and text. |

## World editor (`WowEdit/`, tool build)

The editor library at the repository root (`WowEdit/Source/WowEdit.h`). The terrain overlays are the 8.1 editor pixel shaders rewritten for Shader Model 3 and compiled at run time. They need the Direct3D device (`gxApi d3d` or `d3d9ex`) and `d3dcompiler_47.dll`.

| Command | Status | Notes |
|---|---|---|
| `wowEdit` | works | Prints whether the terrain shaders can run and which overlays are on. |
| `wowEditGrid [0-3\|off]` | works | Terrain grid, one of the four 8.1 `woweditgrid` permutations. With no argument it toggles permutation 3. 0 shows nothing on its own: it outputs transparent black on chunk borders. 1 draws cells with diagonals. 2 draws chunk and tile lines. 3 draws all three. |
| `wowEditSlopes [0\|1]` | works | Paints terrain steeper than 50 degrees (the client's walkable limit) red at 25%. |
| `wowEditMarkers [0\|1]` | works | Sample editor markers ahead of the player, placed on the ground: spawn points with orientation rings, early-editor half spheres and path, and a flying path with direction diamonds. |

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
