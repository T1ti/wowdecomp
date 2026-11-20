#ifndef WOW_SOURCE_WORLDCLIENT_AREALIST_H
#define WOW_SOURCE_WORLDCLIENT_AREALIST_H

#include <cstdint>

int32_t AreaListGetName(uint32_t areaId, char* buffer, uint32_t size, int32_t includeParent);

void AreaListShutdown();

class AreaTableRec;
class CMapEntity;
class WMOAreaTableRec;

bool AreaListZoneHasBreathParticles(CMapEntity* entity);

void AreaListRegisterLocation(int32_t mapId, CMapEntity* entity);

void AreaListUpdateSounds(const AreaTableRec* zone, const AreaTableRec* area, const WMOAreaTableRec* rootRec, const WMOAreaTableRec* groupRec, int32_t inside);

#endif
