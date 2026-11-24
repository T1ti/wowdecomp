#ifndef WOWEDIT_SOURCE_APP_WOWEDITAREAEDITS_H
#define WOWEDIT_SOURCE_APP_WOWEDITAREAEDITS_H

#ifdef WOWDECOMP_TOOL_FEATURES

#include <cstdint>
#include <map>
#include <string>
#include <vector>

class C3Vector;
class CMapChunk;
class CWowEditAreaChange;
class CWowEditUndoer;

// Chunk area overrides authored by WowEdit. Chunk coordinates remain valid across tile unloads.
class CWowEditAreaEdits {
    public:
    explicit CWowEditAreaEdits(CWowEditUndoer& undoer);

    bool OpenMap(int32_t mapId, const C3Vector* position, const char* settingsPath);
    bool HasUnsavedChanges() const;
    bool HasPendingActions() const;
    bool Save();

    int32_t AreaId(const CMapChunk& chunk) const;
    bool IsPainting() const;
    void SetPainting(bool painting);
    bool SelectArea(int32_t areaId);
    int32_t SelectedArea() const;

    // Called by the input host. Positions are normalized within the viewport.
    void QueuePick(float x, float y, bool sample);
    void QueueUndo();
    void QueueRedo();
    // Called with the editor camera projection set, before the shared scene update.
    void ApplyPending(const C3Vector& cameraPosition);
    void CancelPicks();

    private:
    friend class CWowEditAreaChange;
    enum class ActionKind { Paint, Sample, Undo, Redo };
    struct PendingAction {
        ActionKind kind;
        float x;
        float y;
    };
    using Overrides = std::map<uint32_t, int32_t>;

    void ApplyPick(const PendingAction& action, const C3Vector& cameraPosition);
    void SetOverride(uint32_t key, int32_t areaId);

    CWowEditUndoer& m_undoer;
    int32_t m_mapId = -1;
    bool m_painting = false;
    int32_t m_selectedAreaId = 0;
    std::string m_path;
    Overrides m_overrides;
    Overrides m_saved;
    std::vector<PendingAction> m_pending;
};

#endif

#endif
