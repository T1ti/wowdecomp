#ifndef WOWEDIT_SOURCE_APP_WOWEDITUNDO_H
#define WOWEDIT_SOURCE_APP_WOWEDITUNDO_H

// WowEdit application: undo and redo (port; see App/WowEditApp.h).
//
// After the War3 World Editor's CUndoable / CUndoableGroup: a command that changes map data hands back one undoable
// object holding what it needs to revert and reapply the change; a brush drag collects one per stroke step in a
// group, so the whole drag undoes at once. View state (overlays, the brush size) is not undoable, as in War3.
// The session applies edits and undo/redo in the viewport update before scene preparation.

#ifdef WOWDECOMP_TOOL_FEATURES

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

class CWowEditUndoable {
    public:
    virtual ~CWowEditUndoable() {
    }

    // For the Edit menu ("Undo Raise Terrain").
    virtual const char* GetName() = 0;
    virtual void Undo() = 0;
    virtual void Redo() = 0;
};

// Several undoables as one step: undone last to first, redone first to last. Owns its items.
class CWowEditUndoableGroup : public CWowEditUndoable {
    public:
    explicit CWowEditUndoableGroup(const char* name);
    ~CWowEditUndoableGroup() override;

    void Add(CWowEditUndoable* undoable);
    bool IsEmpty();

    const char* GetName() override;
    void Undo() override;
    void Redo() override;

    private:
    char m_name[64];
    TSGrowableArray<CWowEditUndoable*> m_items;
};

// The undo and redo stacks. Owns every undoable it is given.
class CWowEditUndoer {
    public:
    // Steps kept; the oldest is dropped past it.
    static const uint32_t MAX_DEPTH = 100;

    ~CWowEditUndoer();

    // A new change: clears the redo stack.
    void Add(CWowEditUndoable* undoable);
    bool CanUndo();
    bool CanRedo();
    const char* GetUndoName(); // nullptr when there is nothing to undo
    const char* GetRedoName();
    void Undo();
    void Redo();
    void Clear();

    private:
    static void ClearStack(TSGrowableArray<CWowEditUndoable*>& stack);

    TSGrowableArray<CWowEditUndoable*> m_undo;
    TSGrowableArray<CWowEditUndoable*> m_redo;
};

#endif

#endif
