#include "WowEdit/Source/App/WowEditUndo.h"

#ifdef WOWDECOMP_TOOL_FEATURES

#include "Engine/Source/Storm/H/storm.h"

// See WowEditUndo.h.

// ---- CWowEditUndoableGroup

CWowEditUndoableGroup::CWowEditUndoableGroup(const char* name) {
    SStrCopy(this->m_name, name ? name : "", sizeof(this->m_name));
}

CWowEditUndoableGroup::~CWowEditUndoableGroup() {
    for (uint32_t i = 0; i < this->m_items.Count(); i++) {
        delete this->m_items[i];
    }
}

void CWowEditUndoableGroup::Add(CWowEditUndoable* undoable) {
    if (undoable) {
        *this->m_items.New() = undoable;
    }
}

bool CWowEditUndoableGroup::IsEmpty() {
    return this->m_items.Count() == 0;
}

const char* CWowEditUndoableGroup::GetName() {
    return this->m_name;
}

void CWowEditUndoableGroup::Undo() {
    for (uint32_t i = this->m_items.Count(); i > 0; i--) {
        this->m_items[i - 1]->Undo();
    }
}

void CWowEditUndoableGroup::Redo() {
    for (uint32_t i = 0; i < this->m_items.Count(); i++) {
        this->m_items[i]->Redo();
    }
}

// ---- CWowEditUndoer

CWowEditUndoer::~CWowEditUndoer() {
    this->Clear();
}

void CWowEditUndoer::ClearStack(TSGrowableArray<CWowEditUndoable*>& stack) {
    for (uint32_t i = 0; i < stack.Count(); i++) {
        delete stack[i];
    }

    stack.SetCount(0);
}

void CWowEditUndoer::Add(CWowEditUndoable* undoable) {
    if (!undoable) {
        return;
    }

    ClearStack(this->m_redo);

    if (this->m_undo.Count() >= MAX_DEPTH) {
        delete this->m_undo[0];

        for (uint32_t i = 1; i < this->m_undo.Count(); i++) {
            this->m_undo[i - 1] = this->m_undo[i];
        }

        this->m_undo.SetCount(this->m_undo.Count() - 1);
    }

    *this->m_undo.New() = undoable;
}

bool CWowEditUndoer::CanUndo() {
    return this->m_undo.Count() != 0;
}

bool CWowEditUndoer::CanRedo() {
    return this->m_redo.Count() != 0;
}

const char* CWowEditUndoer::GetUndoName() {
    return this->CanUndo() ? this->m_undo[this->m_undo.Count() - 1]->GetName() : nullptr;
}

const char* CWowEditUndoer::GetRedoName() {
    return this->CanRedo() ? this->m_redo[this->m_redo.Count() - 1]->GetName() : nullptr;
}

void CWowEditUndoer::Undo() {
    if (!this->CanUndo()) {
        return;
    }

    CWowEditUndoable* undoable = this->m_undo[this->m_undo.Count() - 1];
    this->m_undo.SetCount(this->m_undo.Count() - 1);

    undoable->Undo();
    *this->m_redo.New() = undoable;
}

void CWowEditUndoer::Redo() {
    if (!this->CanRedo()) {
        return;
    }

    CWowEditUndoable* undoable = this->m_redo[this->m_redo.Count() - 1];
    this->m_redo.SetCount(this->m_redo.Count() - 1);

    undoable->Redo();
    *this->m_undo.New() = undoable;
}

void CWowEditUndoer::Clear() {
    ClearStack(this->m_undo);
    ClearStack(this->m_redo);
}

#endif
