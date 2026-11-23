#ifndef WOWEDIT_SOURCE_APP_WOWEDITWORLDVIEW_H
#define WOWEDIT_SOURCE_APP_WOWEDITWORLDVIEW_H

// WowEdit application: the world view (port; see App/WowEditApp.h).
//
// The editor draws the world itself, without the game: no player, no object manager, no game interface. It is a
// screen layer of its own (ScrnLayerCreate, under the console) that each frame updates the world around a free camera
// and draws it, the world part of CGWorldFrame::OnWorldUpdate / OnWorldRender: CWorld::Update, the day and night
// state, CWorld::Render, the model passes, detail doodads, liquids, weather, glares, and the editor overlays and the
// Gxu debug-draw queue on top.
//
// The world code still asks the game's world frame for its camera (CGWorldFrame::GetActiveCamera: the area of
// interest, the fog padding, the horizon) and for a model hit test (the sun glare occlusion). So a CGWorldFrame is
// created, with a CSimpleTop for it to register with, but neither is shown and the top's input handlers are turned
// off: the frame only owns the camera, which the editor moves itself (CGCamera::UpdateCallback, which follows a
// target, never runs).
//
// Camera: right mouse button held to look around; W/S forward and back, A/D sideways, E/Q up and down (arrow keys
// too); Shift four times faster; the mouse wheel changes the speed. The console key opens the console as usual.

#ifdef WOWDECOMP_TOOL_FEATURES

#include <cstdint>

class C3Vector;

// From the client tool start-up (ClientTool.h): the frames, the camera, the screen layer, the input handlers.
void WowEditWorldInitialize();
void WowEditWorldShutdown();

// Opens a map (Map.dbc ID); position nullptr picks a default (Northshire on map 0, the middle of the map's terrain on
// the others, the camera dropped onto the ground once it has loaded). False when the map is unknown.
bool WowEditWorldLoadMap(int32_t mapId, const C3Vector* position);
int32_t WowEditWorldGetMapId(); // -1 before the first map
void WowEditWorldGetCameraPosition(float* position);

// Time of day for the lighting, in minutes since midnight (the game takes it from the server).
void WowEditWorldSetTimeOfDay(int32_t minutes);
int32_t WowEditWorldGetTimeOfDay();

// Movement keys: QWERTY (W A S D, E up, Q down) or AZERTY (Z Q S D, E up, A down); the arrow keys work in both.
void WowEditWorldSetAzerty(bool azerty);
bool WowEditWorldGetAzerty();

#endif

#endif
