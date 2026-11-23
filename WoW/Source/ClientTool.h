#ifndef WOW_SOURCE_CLIENTTOOL_H
#define WOW_SOURCE_CLIENTTOOL_H

// Tool features: running the client engine as a tool, without the game (port; no client keeps any of it).
//
// Blizzard's tools linked the same engine as the game but not its start-up: no login screen, no realm connection,
// no object manager. A tool executable (WowEdit64, Tools/WowEditApp) describes itself here before CommonMain runs,
// and the client then:
//   - SetPaths: takes the tool's game folder (the one holding Data\) when no -datadir was given, instead of the
//     folder of the executable;
//   - InitializeGlobal: reads and saves the tool's own cvar file in WTF\ instead of Config.wtf;
//   - InitializeEngineCallback: runs the engine start-up as usual (Gx device, textures, models, console), then the
//     databases and the world (ClientDBInitialize, CWorld::Initialize, shadows, lights, sound), and hands over to
//     the tool's initialize callback instead of the glue screens and the network (InitializeToolClient);
//   - DestroyEngineCallback: calls the tool's shutdown and tears down only what the tool start-up made.
// The plain client never sets a tool and runs as before.

#ifdef WOWDECOMP_TOOL_FEATURES

struct CLIENT_TOOL {
    // The game folder, holding Data\ (with a trailing backslash); nullptr or empty for the executable's folder.
    const char* m_basePath;
    // The cvar file in WTF\, instead of the game's Config.wtf (so the tool keeps its own video settings); nullptr
    // for Config.wtf.
    const char* m_configFile;
    // Once the engine, the databases and the world are up: the tool sets up its own screen.
    void (*m_initialize)();
    // Before the world is destroyed at exit.
    void (*m_shutdown)();
};

// From the tool executable, before CommonMain. The structure must outlive the run.
void ClientSetTool(const CLIENT_TOOL* tool);
bool ClientIsTool();

#endif

#endif
