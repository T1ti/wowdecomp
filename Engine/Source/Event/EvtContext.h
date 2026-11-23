#ifndef ENGINE_SOURCE_EVENT_EVTCONTEXT_H
#define ENGINE_SOURCE_EVENT_EVTCONTEXT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/TExtraInstanceRecycler.h"
#include "Engine/Source/Base/TInstanceIdTable.h"
#include "Engine/Source/Base/Types.h"
#include "Engine/Source/Event/CSBasePriorityQueue.h"
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/Event/Types.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/Source/SLock.h"

struct EvtHandler {
    /* 0000 */ TSLink<EvtHandler> m_link;
    /* 0008 */ EVENTHANDLERFUNC m_func;
    /* 000C */ void* m_param;
    /* 0010 */ float m_priority;
    /* 0014 */ int32_t m_marker;
};

struct EvtKeyDown {
    /* 0000 */ TSLink<EvtKeyDown> m_link;
    /* 0008 */ KEY m_key;
};

struct EvtMessage : public TExtraInstanceRecyclable {
    /* 0004 */ TSLink<EvtMessage> m_link;
    /* 000C */ EVENTID m_id;
    /* 0010 */ char m_data[4];
};

struct EvtTimer {
    /* 0000 */ uint32_t m_id;
    /* 0004 */ TSTimerPriority<uint32_t> m_targetTime;
    /* 0014 */ float m_timeout;
    /* 0018 */ EVENTHANDLERFUNC m_handler;
    /* 001C */ void* m_param;
    /* 0020 */ EVENTHANDLERGUIDFUNC m_guidHandler;
    /* 0024 */ uint32_t unused_0024;
    /* 0028 */ uint64_t m_guidParam;
    /* 0030 */ void* m_guidParam2;
    /* 0034 */ uint32_t unused_0034;
};

template <class T>
class EvtIdTable {
    private:
    /* 0000 */ TSGrowableArray<T> m_allocArray;
    /* 0010 */ TSGrowableArray<uint32_t> m_freeArray;

    public:
    uint32_t Alloc();
    void Free(uint32_t id);

    // INLINED
    uint32_t Count() const { // (inferred)
        return this->m_allocArray.Count();
    }

    // INLINED
    T& operator[](uint32_t id) {
        return this->m_allocArray[id];
    }
};

template <class T>
class TInstanceId : public TSLinkedNode<T> {
    public:
    /* 000C */ INSTANCEID m_id;

    TInstanceId();
    virtual ~TInstanceId();
};

template <class T, int bucketCount>
class TSingletonInstanceId : public TInstanceId<T> {
    public:
    static TInstanceIdTable<T, bucketCount> s_idTable;
};

class EvtContext : public TSingletonInstanceId<EvtContext, 8> {
    public:
    // Runtime scheduler state, separate from context-creation debug flags.
    enum : uint32_t {
        SCHED_NONE = 0,
        SCHED_INITIALIZED = 1,
        SCHED_INTERACTIVE = 2,
        SCHED_PAINT_PENDING = 4,
    };

    // Flags consumed only by IEvtSchedulerCreateContext; raw API remains uint32.
    enum : uint32_t {
        DEBUG_CALL_CONTEXT = 1,
        DEBUG_START_WATCHDOG = 2,
    };

    /* 0010 */ mutable SCritSect m_critsect;
    /* 0038 */ uint32_t m_currTime;
    private:
    /* 003C */ SCHEDSTATE m_schedState;

    public:
    /* 0040 */ TSTimerPriority<uint32_t> m_schedNextWakeTime;
    private:
    /* 0050 */ uint32_t m_schedLastIdle;
    /* 0054 */ uint32_t m_schedFlags;
    /* 0058 */ uint32_t m_schedIdleTime;
    /* 005C */ uint32_t m_schedInitialIdleTime;
    public:
    /* 0060 */ uint32_t m_schedWeight;
    /* 0064 */ uint32_t m_schedSmoothWeight;
    /* 0068 */ int32_t m_schedRebalance;
    private:
    /* 006C */ TSExplicitList<EvtHandler, offsetof(EvtHandler, m_link)> m_queueHandlerList[EVENTIDS];
    public:
    /* 021C */ TSExplicitList<EvtMessage, offsetof(EvtMessage, m_link)> m_queueMessageList;
    private:
    /* 0228 */ uint32_t m_queueSyncButtonState;
    /* 022C */ TSExplicitList<EvtKeyDown, offsetof(EvtKeyDown, m_link)> m_queueSyncKeyDownList;
    public:
    /* 0238 */ EvtIdTable<EvtTimer*> m_timerIdTable;
    /* 0258 */ TSPriorityQueue<EvtTimer> m_timerQueue;
    /* 026C */ HPROPCONTEXT m_propContext;
    /* 0270 */ void* m_callContext;
    /* 0274 */ int32_t m_startWatchdog;

    EvtContext(uint32_t flags, uint32_t idleTime, uint32_t schedWeight, void* callContext, int32_t startWatchdog);
    ~EvtContext();

    // INLINED
    bool IsCurrentContext() const {
        return !this->m_id || this->m_id == reinterpret_cast<uintptr_t>(EventGetCurrentContext());
    }

    bool SchedGetClosed() const {
        return this->SchedGetState() == SCHEDSTATE_CLOSED;
    }

    bool SchedGetDestroyed() const { // (inferred)
        return this->SchedGetState() == SCHEDSTATE_DESTROYED;
    }

    void SchedClose() { // (inferred)
        this->m_critsect.Enter();
        if (this->m_schedState == SCHEDSTATE_ACTIVE) {
            this->m_schedState = SCHEDSTATE_CLOSED;
        }
        this->m_critsect.Leave();
    }

    void SchedSetDestroyed() { // (inferred)
        this->m_critsect.Enter();
        this->m_schedState = SCHEDSTATE_DESTROYED;
        this->m_critsect.Leave();
    }

    private:
    SCHEDSTATE SchedGetState() const { // (inferred)
        this->m_critsect.Enter();
        const SCHEDSTATE state = this->m_schedState;
        this->m_critsect.Leave();
        return state;
    }

    public:
    // INLINED
    uint32_t SchedGetLastIdle() const { // (inferred)
        STORM_ASSERT(IsCurrentContext());
        return this->m_schedLastIdle;
    }

    // INLINED
    void SchedSetLastIdle(uint32_t time) { // (inferred)
        STORM_ASSERT(IsCurrentContext());
        this->m_schedLastIdle = time;
    }

    // INLINED
    uint32_t SchedGetFlags() const { // (inferred)
        STORM_ASSERT(IsCurrentContext());
        return this->m_schedFlags;
    }

    // INLINED
    void SchedAddFlags(uint32_t flags) { // (inferred)
        STORM_ASSERT(IsCurrentContext());
        this->m_schedFlags |= flags;
    }

    // INLINED
    void SchedRemoveFlags(uint32_t flags) { // (inferred)
        STORM_ASSERT(IsCurrentContext());
        this->m_schedFlags &= ~flags;
    }

    // INLINED
    uint32_t SchedGetIdleTime() const { // (inferred)
        STORM_ASSERT(IsCurrentContext());
        return this->m_schedIdleTime;
    }

    // INLINED
    uint32_t SchedGetInitialIdleTime() const { // (inferred)
        STORM_ASSERT(IsCurrentContext());
        return this->m_schedInitialIdleTime;
    }

    // INLINED
    TSExplicitList<EvtHandler, offsetof(EvtHandler, m_link)>& QueueGetHandlerList(EVENTID id) { // (inferred)
        STORM_ASSERT(IsCurrentContext());
        STORM_ASSERT(id >= 0 && id < EVENTIDS);
        return this->m_queueHandlerList[id];
    }

    // INLINED
    void QueueLockSyncKeys() { // (inferred)
        STORM_ASSERT(IsCurrentContext());
        this->m_critsect.Enter();
    }

    // INLINED
    void QueueUnlockSyncKeys() { // (inferred)
        STORM_ASSERT(IsCurrentContext());
        this->m_critsect.Leave();
    }

    // INLINED
    TSExplicitList<EvtKeyDown, offsetof(EvtKeyDown, m_link)>& QueueGetSyncKeys() { // (inferred)
        return this->m_queueSyncKeyDownList;
    }

    // INLINED
    bool QueueCheckSyncButton(MOUSEBUTTON button) const { // (inferred)
        STORM_ASSERT(IsCurrentContext());
        return (this->m_queueSyncButtonState & button) != 0;
    }

    // INLINED
    void QueueSetSyncButtons(uint32_t buttons) { // (inferred)
        STORM_ASSERT(IsCurrentContext());
        this->m_queueSyncButtonState |= buttons;
    }

    // INLINED
    void QueueClearSyncButtons(uint32_t buttons) { // (inferred)
        STORM_ASSERT(IsCurrentContext());
        this->m_queueSyncButtonState &= ~buttons;
    }

    // INLINED
    void TimerLock() { // (inferred)
        STORM_ASSERT(IsCurrentContext());
        this->m_critsect.Enter();
    }

    // INLINED
    void TimerUnlock() { // (inferred)
        STORM_ASSERT(IsCurrentContext());
        this->m_critsect.Leave();
    }
};

// INLINED
template <class T>
TInstanceId<T>::TInstanceId() {
    this->m_id = 0;
}

// VERIFY: 3b380357 auto
// OFFSET: [0x0047E510<EvtContext>]
template <class T>
TInstanceId<T>::~TInstanceId() {
}

// STATIC: 0x00B41670
template <class T, int bucketCount>
TInstanceIdTable<T, bucketCount> TSingletonInstanceId<T, bucketCount>::s_idTable;

// INLINED
template <class T>
uint32_t EvtIdTable<T>::Alloc() {
    uint32_t id;

    if (this->m_freeArray.Count()) {
        id = this->m_freeArray.m_data[this->m_freeArray.Count() - 1];
        this->m_freeArray.SetCount(this->m_freeArray.Count() - 1);
    } else {
        id = this->m_allocArray.Count();

        if (id < 1) {
            id = 1;
        }

        this->m_allocArray.GrowToFit(id, 1);
    }

    return id;
}

// INLINED
template <class T>
void EvtIdTable<T>::Free(uint32_t id) {
    STORM_ASSERT(id < m_allocArray.Count());
    this->m_freeArray.New(id);
}

#endif
