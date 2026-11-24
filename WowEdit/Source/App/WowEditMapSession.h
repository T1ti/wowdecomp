#ifndef WOWEDIT_SOURCE_APP_WOWEDITMAPSESSION_H
#define WOWEDIT_SOURCE_APP_WOWEDITMAPSESSION_H

#ifdef WOWDECOMP_TOOL_FEATURES

#include <cstdint>

class C3Vector;
class MapRec;

class CWowEditMapSession {
    public:
    // Check the archive before releasing the current map.
    static const MapRec* FindAvailableMap(int32_t mapId);

    // Open a validated map after closing the previous map and setting up the viewport.
    void Open(const MapRec& map, C3Vector& start);
    void Close();
    const MapRec* GetMap() const { return this->m_map; }

    private:
    const MapRec* m_map = nullptr;
};

#endif

#endif
