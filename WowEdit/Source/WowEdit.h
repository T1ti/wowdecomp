#ifndef WOWEDIT_SOURCE_WOWEDIT_H
#define WOWEDIT_SOURCE_WOWEDIT_H

// WowEdit: the world editor's side of the engine (port; no client keeps any of it), built only with
// WOWDECOMP_TOOL_FEATURES.
//
// Blizzard's world editor ("World Of Warcraft Editor", seen in screenshots running on 3.3.0 data) was its own program
// on the same engine. None of its code survives in any client; what is known comes from screenshots of it, from the
// editor shapes the 4.1 Gxu debug-draw module still builds, and from the pixel shaders of the 8.1 editor. This
// library is this port's reconstruction of the parts the client can host, kept apart from the client sources:
//   - WowEditDraw: markers (spawn points, orientation rings, paths, flying path points) on the Gxu debug-draw module;
//   - WowEditShaders / WowEditTerrain: the 8.1 terrain overlay shaders rewritten for Shader Model 3, drawn as an
//     editor terrain pass;
//   - console commands to turn them on (WowEditRegisterConsoleCommands);
//   - App/: the editor application (WowEdit64.exe), its menus and palettes built with OsGui (App/WowEditApp.h).
// The client only exposes hook points for it (WoW/Source/WorldClient/WorldEditHooks.h) and calls
// WowEditRegisterConsoleCommands; WowEdit depends on the client and engine, never the other way round.
//
// Console commands (category Debug):
//   wowEdit                     status: shader support and which overlays are on
//   wowEditGrid [0-3|off]       terrain grid, an 8.1 woweditgrid permutation (no argument: toggles permutation 3)
//   wowEditSlopes [0|1]         impassable slopes (steeper than 50 degrees) in red
//   wowEditAreas [0|1]          each terrain chunk in a color of its area ID, the area under the cursor named
//   wowEditBrush [radius [inner]|off]  the terrain brush cursor under the mouse: dashed outer and inner circles
//                                      (no argument: toggles a 10-yard brush, inner circle at half the radius)
//   wowEditMarkers [0|1]        a sample of the editor's markers ahead of the player

#ifdef WOWDECOMP_TOOL_FEATURES

// From ClientRegisterConsoleCommands, with the other tool-feature commands.
void WowEditRegisterConsoleCommands();

// After a terrain overlay is turned on: builds its shaders now, so a missing compiler, a compile error or a card
// without Shader Model 3 shows in the console at once rather than as an overlay that never appears.
void WowEditPrepareTerrainOverlays();

// The markers sample (wowEditMarkers).
void WowEditSetMarkers(bool show);
bool WowEditGetMarkers();

#endif

#endif
