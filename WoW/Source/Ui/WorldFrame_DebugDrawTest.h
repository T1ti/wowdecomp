#ifndef WOW_SOURCE_UI_WORLDFRAME_DEBUGDRAWTEST_H
#define WOW_SOURCE_UI_WORLDFRAME_DEBUGDRAWTEST_H

// Tool features: the "debugDrawTest" console command, which exercises the Gxu debug-draw module (port, see
// WorldFrame_DebugDrawTest.cpp).

#ifdef WOWDECOMP_TOOL_FEATURES

// Registers the console command (from ClientRegisterConsoleCommands, next to the debug-feature commands).
void GxuDebugDrawTestRegister();

// Queues the test primitives for this frame when the command is on (from CGWorldFrame::OnWorldRender, before
// GxuDebugDrawRender).
void GxuDebugDrawTestQueue();

#endif

#endif
