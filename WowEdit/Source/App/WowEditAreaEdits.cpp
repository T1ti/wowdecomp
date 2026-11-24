#include "WowEdit/Source/App/WowEditAreaEdits.h"

#ifdef WOWDECOMP_TOOL_FEATURES

#include <windows.h>
#include <algorithm>
#include <fstream>
#include <utility>
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Services/Camera.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapChunk.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/WorldConstants.h"
#include "WowEdit/Source/App/WowEditUndo.h"
#include "WowEdit/Source/App/WowEditWorldView.h"

static uint32_t ChunkKey(int32_t x, int32_t y) {
    return static_cast<uint32_t>(x + y * MAP_CHUNKS_PER_MAP_AXIS);
}

static bool ValidChunk(int32_t x, int32_t y) {
    return x >= 0 && x < MAP_CHUNKS_PER_MAP_AXIS && y >= 0 && y < MAP_CHUNKS_PER_MAP_AXIS;
}

class CWowEditAreaChange : public CWowEditUndoable {
    public:
    CWowEditAreaChange(CWowEditAreaEdits& edits, uint32_t key, int32_t before, int32_t after)
        : m_edits(edits), m_key(key), m_before(before), m_after(after) {}

    const char* GetName() override { return "Paint Area"; }
    void Undo() override { this->m_edits.SetOverride(this->m_key, this->m_before); }
    void Redo() override { this->m_edits.SetOverride(this->m_key, this->m_after); }

    private:
    CWowEditAreaEdits& m_edits;
    uint32_t m_key;
    int32_t m_before;
    int32_t m_after;
};

CWowEditAreaEdits::CWowEditAreaEdits(CWowEditUndoer& undoer) : m_undoer(undoer) {}

bool CWowEditAreaEdits::OpenMap(int32_t mapId, const C3Vector* position, const char* settingsPath) {
    if (this->HasUnsavedChanges() || this->HasPendingActions()) {
        ConsolePrintf("WowEdit: save the current map's area edits before opening another map");
        return false;
    }

    std::string path = std::string(settingsPath) + ".map-" + std::to_string(mapId) + ".areas";
    Overrides overrides;
    std::ifstream input(path);
    if (input) {
        std::string type;
        int32_t version;
        int32_t fileMap;
        if (!(input >> type >> version >> fileMap) || type != "WOWEDIT_AREAS" || version != 1 || fileMap != mapId) {
            ConsolePrintf("WowEdit: invalid area edits header in %s", path.c_str());
            return false;
        }

        while (input >> std::ws && input.peek() != std::char_traits<char>::eof()) {
            int32_t x;
            int32_t y;
            int32_t areaId;
            if (!(input >> x >> y >> areaId) || !ValidChunk(x, y) || areaId <= 0 ||
                !overrides.emplace(ChunkKey(x, y), areaId).second) {
                ConsolePrintf("WowEdit: invalid chunk area override in %s", path.c_str());
                return false;
            }
        }
        if (input.bad()) {
            ConsolePrintf("WowEdit: could not read area edits in %s", path.c_str());
            return false;
        }
    } else if (GetFileAttributesA(path.c_str()) != INVALID_FILE_ATTRIBUTES || GetLastError() != ERROR_FILE_NOT_FOUND) {
        ConsolePrintf("WowEdit: could not read area edits in %s", path.c_str());
        return false;
    }

    if (!WowEditWorldLoadMap(mapId, position)) {
        return false;
    }

    this->m_undoer.Clear();
    this->m_mapId = mapId;
    this->m_path = std::move(path);
    this->m_overrides = std::move(overrides);
    this->m_saved = this->m_overrides;
    this->m_selectedAreaId = 0;
    return true;
}

bool CWowEditAreaEdits::HasUnsavedChanges() const {
    return this->m_overrides != this->m_saved;
}

bool CWowEditAreaEdits::HasPendingActions() const {
    return !this->m_pending.empty();
}

bool CWowEditAreaEdits::Save() {
    if (this->HasPendingActions()) {
        return false;
    }

    if (!this->HasUnsavedChanges()) {
        return true;
    }

    const std::string temp = this->m_path + "." + std::to_string(GetCurrentProcessId()) + ".tmp";
    std::ofstream output(temp);
    output << "WOWEDIT_AREAS 1 " << this->m_mapId << '\n';
    for (const auto& [key, areaId] : this->m_overrides) {
        output << key % MAP_CHUNKS_PER_MAP_AXIS << ' ' << key / MAP_CHUNKS_PER_MAP_AXIS << ' ' << areaId << '\n';
    }

    output.close();
    if (!output || !MoveFileExA(temp.c_str(), this->m_path.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
        DeleteFileA(temp.c_str());
        ConsolePrintf("WowEdit: could not save area edits to %s", this->m_path.c_str());
        return false;
    }

    this->m_saved = this->m_overrides;
    ConsolePrintf("WowEdit: saved area edits to %s", this->m_path.c_str());
    return true;
}

int32_t CWowEditAreaEdits::AreaId(const CMapChunk& chunk) const {
    const auto entry = this->m_overrides.find(ChunkKey(chunk.m_cOffset.x, chunk.m_cOffset.y));
    return entry == this->m_overrides.end() ? chunk.m_areaId : entry->second;
}

bool CWowEditAreaEdits::IsPainting() const {
    return this->m_painting;
}

void CWowEditAreaEdits::SetPainting(bool painting) {
    this->m_painting = painting;
    if (painting) {
        ConsolePrintf("WowEdit: Ctrl+click terrain to sample an area, then click a chunk to paint it");
    }
}

bool CWowEditAreaEdits::SelectArea(int32_t areaId) {
    if (areaId <= 0 || !g_areaTableDB.GetRecord(areaId)) {
        return false;
    }

    this->m_selectedAreaId = areaId;
    return true;
}

int32_t CWowEditAreaEdits::SelectedArea() const {
    return this->m_selectedAreaId;
}

void CWowEditAreaEdits::QueuePick(float x, float y, bool sample) {
    if (this->m_mapId >= 0 && x >= 0.0f && x <= 1.0f && y >= 0.0f && y <= 1.0f) {
        this->m_pending.push_back({ sample ? ActionKind::Sample : ActionKind::Paint, x, y });
    }
}

void CWowEditAreaEdits::QueueUndo() {
    this->m_pending.push_back({ ActionKind::Undo, 0.0f, 0.0f });
}

void CWowEditAreaEdits::QueueRedo() {
    this->m_pending.push_back({ ActionKind::Redo, 0.0f, 0.0f });
}

void CWowEditAreaEdits::CancelPicks() {
    auto firstPick = std::remove_if(this->m_pending.begin(), this->m_pending.end(), [](const PendingAction& action) {
        return action.kind == ActionKind::Paint || action.kind == ActionKind::Sample;
    });
    this->m_pending.erase(firstPick, this->m_pending.end());
}

void CWowEditAreaEdits::ApplyPending(const C3Vector& cameraPosition) {
    for (const PendingAction& action : this->m_pending) {
        switch (action.kind) {
            case ActionKind::Undo:
                this->m_undoer.Undo();
                break;
            case ActionKind::Redo:
                this->m_undoer.Redo();
                break;
            default:
                this->ApplyPick(action, cameraPosition);
                break;
        }
    }

    this->m_pending.clear();
}

void CWowEditAreaEdits::ApplyPick(const PendingAction& action, const C3Vector& cameraPosition) {
    C3Vector start;
    C3Vector end;
    CameraGetLineSegment(action.x, action.y, &start, &end);
    start += cameraPosition;
    end += cameraPosition;

    C3Vector hit;
    float fraction = 1.0f;
    if (!World::Intersect(start, end, &hit, &fraction, WORLD_QUERY_TERRAIN, nullptr) || !World::IsValidPosition(hit, 0.0f)) {
        return;
    }

    const C2Vector xy(hit.x, hit.y);
    const CMapChunk* chunk = CMap::GetChunkAtPosition(&xy);
    if (!chunk || !ValidChunk(chunk->m_cOffset.x, chunk->m_cOffset.y)) {
        return;
    }

    const int32_t currentAreaId = this->AreaId(*chunk);
    if (action.kind == ActionKind::Sample) {
        if (this->SelectArea(currentAreaId)) {
            ConsolePrintf("WowEdit: selected area %d", currentAreaId);
        }
        return;
    }

    if (!this->m_selectedAreaId) {
        ConsolePrintf("WowEdit: Ctrl+click a chunk to select an area before painting");
        return;
    }

    if (this->m_selectedAreaId != currentAreaId) {
        const uint32_t key = ChunkKey(chunk->m_cOffset.x, chunk->m_cOffset.y);
        const auto old = this->m_overrides.find(key);
        const int32_t before = old == this->m_overrides.end() ? 0 : old->second;
        const int32_t after = this->m_selectedAreaId == chunk->m_areaId ? 0 : this->m_selectedAreaId;
        auto* change = new CWowEditAreaChange(*this, key, before, after);
        change->Redo();
        this->m_undoer.Add(change);
    }
}

void CWowEditAreaEdits::SetOverride(uint32_t key, int32_t areaId) {
    if (areaId) {
        this->m_overrides[key] = areaId;
    } else {
        this->m_overrides.erase(key);
    }
}

#endif
