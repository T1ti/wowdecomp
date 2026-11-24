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
| `showhittest` | works | Toggles the geometry object picking tests (half transparent red) on the tracked object's model, or on the player's model if nothing is tracked. From 0.8 and 0.9.1, which registered it (0.5.3 did not). Their models had separate hit-test shapes; the M2 format has none, and 3.3.5 picks an object by its model's skinned render triangles (skipping batches flagged not to be hit-tested), so that is what it shows. The collision mesh has its own display (`showCollision`). |
| `showunselectable [red green blue]` | untested | Draws the geometry cursor picking skips (M2 batches flagged no-hit-test) on the tracked object's model, or the player's, half transparent, in yellow or the given color (0..255 each); again with no color turns it off. 0.5.3 recolored such geosets in `ModelShowUnselectable`, which nothing called; the command name is the port's. |
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
| `showNormals` | works | Also draws the WMO vertex normals (3.3.5 `CMapObj::RenderNormals`, the 0.9.1 `RenderGroupNormals`). |
| `showQuery` | works | Draws the world collision queries (3.3.5 `TestQueryAdd`, the 0.9.1 `TestQueryRender`). |
| `proj` | works | |
| `detailDoodadTest` | works | |
| `showCollision` | works | Draws the collision mesh (translucent triangles and red face normals) of every drawn M2 that has collision data. |
| `showAABoxes` | works | Draws the collision box (translucent cyan) of every drawn M2 that has collision data. 0.5.3 called this "Terrain doodads AA Box visuals". |
| `showMapObjBSP` | untested | Toggles the 0.5.3 / 0.9.1 WMO BSP display (`CMapObj::RenderGroupBsp`): every WMO group draws its faces by kind, lit and untextured. Render faces are green, detail faces blue, and collision faces translucent red over them. Port addition: `showMapObjBSP collision`, `detail`, `render`, `transition` or `collidable` draws the faces one of the 3.3.5 face renderers selects over the normal rendering (the release build has those renderers but never selects them; on their own they draw only the selected faces, which leaves the rest of the WMO invisible), and `showMapObjBSP normal` goes back to normal rendering. Picking a mode turns the BSP display off; turning the BSP display on overrides the mode. The detail and collision passes are drawn with a small depth bias so that faces lying on other faces don't z-fight. |
| `debugBSP` | flag only | No client reads the bit: 0.5.3 sets it and nothing tests it, and by 0.9.1 the bit had become `CMap::enableTriangleStrips`. |
| `showCrappyBatches` | flag only | No client reads the bit: in 0.5.3 only this command tests it, and nothing in 3.3.5 does. |
| `debugZones` | untested | Draws the terrain outside the player's zone plain white, untextured, as 0.9.1 did. 0.9.1 got the zone map from the server (message 11, one byte per world cell), which 3.3.5 does not have. The port compares the top-level zone of each chunk's area (`AreaTable` parent chain) with the player's zone instead. |
| `groupOnly` | untested | Toggles drawing only the WMO group the camera is in: the portal recursion limit (0.5.3 `CMapObj::maxRLevel`, 3.3.5 `s_maxPortalDepth`) between 0 and 10. |
| `mapObjLightMode` | no effect | Prints the 0.5.3 messages (vertex light / lightmaps) and changes nothing: 0.5.3 toggled an enables bit nothing read, which 3.3.5 reuses for footprints, so the port keeps a flag of its own. |
| `waterMaxLOD`, `waterWaves`, `waterSpecular` | no effect | Store a number (`waterMaxLOD` clamped to 0..4), as 0.5.3 did; nothing in 0.5.3 read those values, and 3.3.5 water has nothing like them. |
| `enumTextures`, `enumTextureGxCache` | stub | |

0.5.3 compiled some of these handlers without registering them (`showTerrain` and the commands after it in
`World_Debug.cpp`). Their command names were made up from the handler names.

## Sky (`WoW/Source/WorldClient/DayNight_Debug.cpp`, DEBUG)

| Command | Status |
|---|---|
| `SkyShow` | untested: `SkyShow 1` shows the sky, anything else hides it (sky, sun, moons, stars, glares; 3.3.5 `DNInfo::m_showSky`) |
| `SkyCloudLayers` | untested: 0 or 1 cloud layers; 3.3.5 has a single layer, so this turns the clouds off or on (`DNClouds::m_enabled`) |
| `SkyCloudDensity` | untested: overrides the cloud density, 0..1; 0 gives it back to the sky data |

## Units / combat (`WoW/Source/Object/ObjectClient/Unit_C_Debug.cpp`, DEBUG)

| Command | Status | Notes |
|---|---|---|
| `DebugTargetPath` (CVar) | works | When set to 1, draws the movement path of the locked target and of the player, as 0.5.3 did (no later client has it). Flying (Catmull-Rom) splines, including taxi flights: green spheres at the control points and a white line along the curve. Ground moves: circles projected on the ground, green at the start and red at every spline point, 1 yard across as in 0.5.3 (no spheres or line in 0.5.3). The port centers the circles on the ground below each point and adds small green spheres inside the circles at the path points (half spheres on the ground) and a white line from start to end draped on the ground. The last path stays drawn after the unit stops, as in 0.5.3. Port addition: while the player rides a boat or zeppelin, its path on the current map is drawn like a flight path. |
| `CombatDebugForceActionOn` / `Off` | stub | |
| `CombatDebugForceActionOtherOn` / `Off` | stub | |
| `CombatDebugShowFlags` | stub | |
| `DumpDeathHoldLogs` | stub | |
| `playercombatlogdebug` | stub | |
| `showEffectsStandalone` (CVar) | stub | |
| `animDebug` | works | Makes the locked target the anim debug target (no target: off), from 0.8. Every animation its model starts is printed to the console, with the 0.8 wording: `**** <time> "<unit>" "<anim>" playing [on bone "<key bone>"]`, with the 0.8 key bone names (the wheel bones, which 0.8 lacked, are named after the 3.3.5 enum). When the model lacks the animation, a `doesn't have "<anim>", falling back to "<anim>"` line comes first. The command name is the port's; 0.8 had the setter without a command. Its queueing and interrupting lines are not ported, because they came from the 0.8 unit animation queue, which 3.3.5 does not have. |

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

The standalone editor uses 8.1 terrain overlay and brush shaders rewritten for Shader Model 3. It compiles them at runtime and requires a Direct3D 9 or 9Ex device and d3dcompiler_47.dll. Shader build errors appear in the editor console.

The tool build makes `WowEdit64.exe` (`Tools/WowEditApp`, `WowEdit/Source/App/WowEditApp.h`). The editor owns startup and the event loop and shares the client engine’s archive, graphics, and world services. It renders the world around a free camera, and its window chrome is built with OsGui, the engine's tool interface layer.

- **Settings:** `WowEdit.ini` next to the exe, written with defaults on the first run. `DataPath` is the game's `Data` folder (or the folder holding it; empty means the exe's folder). `MapId` and `StartX/Y/Z` set the map and camera position at start, and are saved on exit. The editor keeps its own cvars in `WTF\WowEdit.wtf` in the game folder (a 1600x900 window by default), apart from the game's `Config.wtf`.
- **Camera:** hold the right mouse button to look (the cursor hides meanwhile), W/A/S/D (or the arrow keys) to move, E/Q up and down, Shift for four times the speed, mouse wheel to change the speed. With View > AZERTY Movement Keys (or `Keyboard=AZERTY` in WowEdit.ini; empty detects the layout) the keys are Z/Q/S/D, E up and A down.
- **Maps:** File > Open Map lists Map.dbc. Map 0 starts at Northshire; other maps start over the middle of their terrain, dropped onto the ground once it loads.
- **View:** the View menu and the floating View palette hold toggles named after WoWEdit's bottom toolbar (wowdev.wiki/WoWEdit): Game View (match in-game view), Grid, Area IDs, Impassable, Terrain, Shadows, Fog, Doodads, Detail Doodads, WMOs, Water, Sky, Particulates, Wireframe, Normals, Collision (debug build) and Markers. Grid draws chunk and tile lines. Wireframe adds cells and diagonals over the textured ground. Area IDs colors terrain chunks by area and labels the area under the cursor. Markers draws sample spawn points and paths near the editor camera. Fog starts off, and the WDL horizon goes with it (without fog it is a flat white band). The View menu also sets the time of day (dawn, noon, dusk, midnight).
- **Render distance:** Window > Render Distance has sliders for the view distance (`farclip`, 183 to 1583 yards, default 1583; the editor sets `farClipOverride 1` so the old world maps get the full range too), the doodad and WMO distance (`environmentDetail`, 0.5 to 1.5, default 1.5), ground clutter (`groundEffectDist`, 0 to 140 yards, default 70) and the WDL horizon (`horizonFarclipScale`, 3 to 6, only with fog). The view distance is also how far the world loads: `World::PrepareAreaOfInterest` loads tiles and chunks 1.25 to 2 times the far clip around the camera. They are saved in WowEdit.ini at exit.
- **Cursor:** the Windows arrow over the 3D view (the Gx window is subclassed for `WM_SETCURSOR`, which the D3D window procedure otherwise answers with the game's hardware cursor).
- **Tools:** Tools > Terrain Brush (Ctrl+B) shows or hides the brush preview. Set its outer and inner radii in the Tool Palette. **Tools > Paint Area IDs** enables single-chunk area painting and the Area IDs overlay. Ctrl-click terrain to sample its area, then click other chunks to apply it. Use Ctrl+Z or Ctrl+Y to undo or redo. **File > Save Area Edits** (Ctrl+S) saves a map-specific file next to WowEdit.ini, and reopening the map reloads those overrides. They affect the editor's area display and leave the original ADTs intact.

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

## 0.8 / 0.9.1 debug features not ported

- `showfootprints`: already a retail 3.3.5 CVar (`CWorldParam::ShowFootprintsCallback`).
- The movement speed and turn rate cheats (0.9.1 `CGUnit_C::OnAllSpeedChangeCheat` / `OnTurnRateChangeCheat`):
  they read the speed from the gamepad axes and send server cheat opcodes, so they need server support.
- The collision toggle (`CMovement::ToggleCollision`): a pending move action the server sends.
