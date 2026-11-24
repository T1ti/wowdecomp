#ifndef WOWEDIT_SOURCE_APP_WOWEDITMAPSESSION_H
#define WOWEDIT_SOURCE_APP_WOWEDITMAPSESSION_H

#ifdef WOWDECOMP_TOOL_FEATURES

#include <cstdint>

class C3Vector;
class MapRec;
struct WorldStreamingView;

// Names are borrowed from the loaded map database and remain valid until the database is closed.
struct WowEditMapInfo {
    int32_t id;
    const char* name;
    const char* directory;
};

class CWowEditMapSession {
    public:
    static uint32_t GetMapCount();
    static WowEditMapInfo GetMapInfo(uint32_t index);

    // Check the archive before releasing the current map.
    static const MapRec* FindAvailableMap(int32_t mapId);

    // Open a validated map after closing the previous map and setting up the viewport.
    void Open(const MapRec& map, C3Vector& start, const WorldStreamingView& streaming);
    void Close();
    const MapRec* GetMap() const { return this->m_map; }

    private:
    const MapRec* m_map = nullptr;
};

#endif

#endif
