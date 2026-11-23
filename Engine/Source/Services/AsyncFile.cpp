#include <cstddef>
#include <cstdint>
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/Base/Prop.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00AC3438
static const char* const s_asyncQueueNames[3] = { "Disk Queue", "Net Geometry Queue", "Net Texture Queue" };

// STATIC: 0x00AC3444
static uint32_t s_handlerTimeout = 100;

// STATIC: 0x00AC3448
static TSList<CAsyncQueue, TSGetLink<CAsyncQueue> > s_asyncQueueList;

// STATIC: 0x00AC3454
static TSList<CAsyncThread, TSGetLink<CAsyncThread> > s_asyncThreadList;

namespace AsyncFileRead {
    // STATIC: 0x00AC3460
    static TSExplicitList<CAsyncObject, offsetof(CAsyncObject, link)> s_asyncFileReadFreeList;
}

// STATIC: 0x00AC346C
static TSExplicitList<CAsyncObject, offsetof(CAsyncObject, link)> s_asyncFileReadPostList;

// STATIC: 0x00B4A1EC
static int32_t s_userQueueLock;

// STATIC: 0x00B4A1F0
static void (*s_ingameStartCallback)();

// STATIC: 0x00B4A1F4
static void (*s_ingameProgressCallback)(float, int32_t);

// STATIC: 0x00B4A1F8
static int32_t s_progressCount;

// STATIC: 0x00B4A1FC
static void* s_asyncProgressCallbackParams;

// STATIC: 0x00B4A200
static void (*s_asyncProgressCallback)(float, void*);

// STATIC: 0x00B4A204
static CAsyncObject* s_asyncWaitObject;

// STATIC: 0x00B4A208
static HPROPCONTEXT s_propContext;

// STATIC: 0x00B4A20C
static CAsyncQueue* s_asyncQueues[3];

// STATIC: 0x00B4A218
static uint32_t s_threadSleep;

namespace AsyncFileRead {
    // STATIC: 0x00B4A21C
    static SEvent s_shutdownEvent(1, 0);
}

// STATIC: 0x00B4A220
static TSGrowableArray<void (*)()> s_asyncPollHandlers;

// STATIC: 0x00B4A230
static TSGrowableArray<int32_t (*)()> s_asyncStatusHandlers;

// STATIC: 0x00B4A240
static SCritSect g_queueLock;

// VERIFY: 5a8ab5a9 auto
// OFFSET: 0x004B50A0
BOOL CAsyncObject::IsPending() const {
    return !this->isCurrent && !this->isRead && !this->isProcessed;
}

// VERIFY: 6305e7e3 auto
// OFFSET: 0x004B9910
void AsyncFileReadSetProgressCallback(void (*callback)(float, void*), void* param) {
    s_asyncProgressCallback = callback;
    s_asyncProgressCallbackParams = param;
}

// VERIFY: fe99c992 auto
// OFFSET: 0x004B9930
void AsyncFileReadIngameProgressCallback(void (*startCallback)(), void (*progressCallback)(float, int32_t)) {
    s_ingameStartCallback = startCallback;
    s_ingameProgressCallback = progressCallback;
}

// VERIFY: 08e4e19e auto
// OFFSET: 0x004B9950
void AsyncFile::EnterQueueLock() {
    s_userQueueLock = 1;
    g_queueLock.Enter();
}

// VERIFY: ee2b2778 auto
// OFFSET: 0x004B9970
void AsyncFile::LeaveQueueLock() {
    g_queueLock.Leave();
    s_userQueueLock = 0;
}

// VERIFY: ca9687a9 manual
// OFFSET: 0x004B9AB0
static void AsyncFileReadUnloadPostList() {
    CAsyncObject* object;

    while ((object = s_asyncFileReadPostList.Head()) != nullptr) {
        object->link.Unlink();

        if (object->userPostUnloadCallback) {
            object->userPostUnloadCallback(object->userArg);
        }
    }
}

// VERIFY: acd49bbd manual
// OFFSET: 0x004B9B20
int32_t AsyncFileReadPollHandler(const void* data, void* param) {
    uint32_t start = OsGetAsyncTimeMs();

    while (true) {
        g_queueLock.Enter();

        CAsyncObject* object = s_asyncFileReadPostList.Head();

        if (!object) {
            g_queueLock.Leave();
            break;
        }

        object->link.Unlink();

        if (object == s_asyncWaitObject) {
            s_asyncWaitObject = nullptr;
        }

        object->isProcessed = 1;
        g_queueLock.Leave();
        STORM_ASSERT(object->userPostloadCallback);
        object->userPostloadCallback(object->userArg);
        s_progressCount--;

        if (OsGetAsyncTimeMs() - start > s_handlerTimeout) {
            break;
        }
    }

    for (uint32_t i = 0; i < s_asyncPollHandlers.Count(); i++) {
        s_asyncPollHandlers[i]();
    }

    return 1;
}

// VERIFY: 132a0c31 manual
// OFFSET: 0x004B9C60
void AsyncFileReadAddPollHandler(void (*handler)()) {
    for (uint32_t i = 0; i < s_asyncPollHandlers.Count(); i++) {
        if (s_asyncPollHandlers[i] == handler) {
            return;
        }
    }

    s_asyncPollHandlers.New(handler);
}

// VERIFY: e6dbc58a manual
// OFFSET: 0x004B9D20
void AsyncFileReadAddStatusHandler(int32_t (*handler)()) {
    for (uint32_t i = 0; i < s_asyncStatusHandlers.Count(); i++) {
        if (s_asyncStatusHandlers[i] == handler) {
            return;
        }
    }

    s_asyncStatusHandlers.New(handler);
}

// VERIFY: 75b60811 manual
// OFFSET: 0x004B9DE0
void AsyncFileReadDestroyObject(CAsyncObject* object) {
    STORM_ASSERT(object);

    g_queueLock.Enter();

    if (object->isCurrent) {
        DebugLog("AsyncFileReadDestroyObject()");
        g_queueLock.Leave();

        while (object->isCurrent) {
            OsSleep(1);
        }

        g_queueLock.Enter();
    }

    object->link.Unlink();

    if (object->file) {
        SFileCloseFile(object->file);
    }

    AsyncFileRead::s_asyncFileReadFreeList.LinkToHead(object);
    g_queueLock.Leave();
}

// VERIFY: 37b4ae03 auto
// OFFSET: 0x004B9E90
static void DefaultAsyncObjectCleanupCallback(CAsyncObject* object) {
    AsyncFileReadDestroyObject(object);
}

// VERIFY: 3f12de16 manual
// OFFSET: 0x004B9EA0
int32_t AsyncFile::Cancel(CAsyncObject* object, void (*callback)(CAsyncObject*)) {
    g_queueLock.Enter();

    if (object->isCurrent) {
        object->userArg = object;
        object->userPostUnloadCallback = reinterpret_cast<CAsyncObject::POST_CALLBACK>(callback ? callback : DefaultAsyncObjectCleanupCallback);
        object->userPostloadCallback = object->userPostUnloadCallback;
        g_queueLock.Leave();

        return 0;
    }

    SFileCloseFile(object->file);
    object->link.Unlink();
    AsyncFileRead::s_asyncFileReadFreeList.LinkToHead(object);
    g_queueLock.Leave();

    return 1;
}

// VERIFY: 8a117547 manual
// OFFSET: 0x004B9F40
static int32_t NetQueuePromoteLocalFiles(const void* data, void* param) {
    // STATIC: 0x00B4A268
    static uint32_t s_lastPromoteTime;

    STORM_ASSERT(SFile::IsStreamingMode());

    uint32_t time = OsGetAsyncTimeMs();

    if (time - s_lastPromoteTime >= 250) {
        s_lastPromoteTime = time;
        g_queueLock.Enter();

        for (int32_t i = 1; i < 3; i++) {
            CAsyncObject* object = s_asyncQueues[i]->readList.Head();

            while (!s_asyncQueues[i]->readList.IsEnd(object)) {
                CAsyncObject* next = s_asyncQueues[i]->readList.RawNext(object);

                if (SFile::FileIsLocal(object->file, 6) == SFILE_LOCAL_YES) {
                    s_asyncQueues[0]->readList.LinkToTail(object);
                    object->queue = s_asyncQueues[0];
                }

                object = next;
            }
        }

        g_queueLock.Leave();
    }

    return 1;
}

// VERIFY: d0d6eee1 manual
// OFFSET: 0x004BA060
void AsyncFileReadWait(CAsyncObject* object) {
    // STATIC: 0x00B4A26C
    static int32_t s_waiting;

    STORM_ASSERT(object);

    if (s_waiting) {
        SErrDisplayAppFatal("AsyncFileReadWait(): s_waiting != FALSE");
    }

    s_waiting++;
    g_queueLock.Enter();

    if (object->isProcessed) {
        g_queueLock.Leave();
        return;
    }

    STORM_ASSERT(s_asyncWaitObject == nullptr); // 12213: "s_asyncWaitObject == __null"
    s_asyncWaitObject = object;

    if (!object->isCurrent && !object->isRead) {
        STORM_ASSERT(object->queue);
        object->link.Unlink();
        object->queue->readList.LinkToHead(object);
    }

    g_queueLock.Leave();

    if (SFile::IsStreamingMode()) {
        SFile::SetFilePriority(object->file, 1, 0);
    }

    char name[256];
    SFile::GetActualFileName(object->file, name, sizeof(name));
    DebugLog("AsyncFileReadWait():%s\n", name);

    if (s_ingameStartCallback) {
        s_ingameStartCallback();
    }

    do {
        if (s_ingameProgressCallback) {
            s_ingameProgressCallback(0.0f, 0);
        }

        AsyncFileReadPollHandler(nullptr, nullptr);

        if (!s_asyncWaitObject) {
            break;
        }

        OsSleep(1);
    } while (true);

    s_waiting--;
}

// VERIFY: 82068864 manual
// OFFSET: 0x004BA170
CAsyncObject* AsyncFileReadAllocObject() {
    g_queueLock.Enter();

    CAsyncObject* object = AsyncFileRead::s_asyncFileReadFreeList.Head();

    if (!object) {
        object = AsyncFileRead::s_asyncFileReadFreeList.NewNode(TSLIST_LINK_AFTER, 0, 0);
    }

    object->link.Unlink();
    g_queueLock.Leave();

    object->file = nullptr;
    object->buffer = nullptr;
    object->size = 0;
    object->userArg = nullptr;
    object->userPostloadCallback = nullptr;
    object->userPostUnloadCallback = nullptr;
    object->queue = nullptr;
    object->isProcessed = 0;
    object->isRead = 0;
    object->isCurrent = 0;
    object->m_char24 = 0;
    object->m_char25 = 0;
    object->queuedFrame = 0;
    object->priority = 126;

    return object;
}

// VERIFY: d372c124 manual
// OFFSET: 0x004BA310
CAsyncQueue::~CAsyncQueue() {
}

// VERIFY: 69785110 manual
// OFFSET: 0x004BA3D0
void AsyncFileReadLinkObject(CAsyncObject* object, int32_t urgent) {
    if (!object->queue) {
        return;
    }

    STORM_ASSERT(!object->isRead);
    STORM_ASSERT(!object->isCurrent);
    STORM_ASSERT(!object->isProcessed);

    object->link.Unlink();

    for (CAsyncObject* other = object->queue->readList.Head(); !object->queue->readList.IsEnd(other); other = object->queue->readList.RawNext(other)) {
        if (object->priority <= other->priority && (urgent || object->priority != other->priority)) {
            object->queue->readList.LinkNode(object, TSLIST_LINK_BEFORE, other);
            object->m_char25 = 0;
            return;
        }
    }

    object->queue->readList.LinkToTail(object);
    object->m_char25 = 0;
}

// VERIFY: c818c689 manual
// OFFSET: 0x004BA530
static void AsyncFileReadLinkStaleObject(CAsyncObject* object, int32_t urgent) {
    object->link.Unlink();

    for (CAsyncObject* other = object->queue->m_staleList.Head(); !object->queue->m_staleList.IsEnd(other); other = object->queue->m_staleList.RawNext(other)) {
        if (object->priority <= other->priority && (urgent || object->priority != other->priority)) {
            object->queue->m_staleList.LinkNode(object, TSLIST_LINK_BEFORE, other);
            object->m_char25 = 1;
            return;
        }
    }

    object->queue->m_staleList.LinkToTail(object);
    object->m_char25 = 1;
}

// VERIFY: 28f905c7 manual
// OFFSET: 0x004BA680
static uint32_t STORMAPI AsyncFileReadThread(void* param) {
    CAsyncThread* thread = static_cast<CAsyncThread*>(param);
    CAsyncQueue* queue = thread->m_queue;
    PropSelectContext(s_propContext);

    while (AsyncFileRead::s_shutdownEvent.Wait(0)) {
        uint32_t count = 0;

        while (true) {
            g_queueLock.Enter();

            CAsyncObject* object = queue->readList.Head();

            if (object) {
                if (queue->m_frameLimited && g_theGxDevicePtr->m_frameCount - object->queuedFrame > 3) {
                    AsyncFileReadLinkStaleObject(object, 1);
                    g_queueLock.Leave();
                    continue;
                }
            } else {
                object = queue->m_staleList.Head();

                if (!object) {
                    break;
                }
            }

            object->link.Unlink();
            STORM_ASSERT(!thread->currentObject);
            STORM_ASSERT(!object->isCurrent);
            STORM_ASSERT(object->queue == queue);
            object->queue = nullptr;
            object->isCurrent = 1;
            thread->currentObject = object;
            g_queueLock.Leave();

            STORM_ASSERT(!object->isRead);
            STORM_ASSERT(!object->isProcessed);
            STORM_ASSERT(object->file);
            STORM_ASSERT(object->buffer);
            STORM_ASSERT(object->size);
            STORM_ASSERT(object->userPostloadCallback);

            for (int32_t retries = 10; retries; ) {
                if (SFile::IsStreamingMode() && object->file->m_type) {
                    SFile::MakeFileLocal(object->file, (object->priority > 127) + 1, true);
                }

                if (SFileReadFile(object->file, object->buffer, object->size, nullptr, nullptr, 0)) {
                    break;
                }

                if (!--retries) {
                    char name[512];
                    SFile::GetActualFileName(object->file, name, sizeof(name));
                    char error[512];
                    SErrGetErrorStr(SErrGetLastError(), error, sizeof(error));
                    DebugLog("AsyncFileReadThread() failed reading %s: %s", name, error);
                }
            }

            g_queueLock.Enter();
            s_asyncFileReadPostList.LinkToTail(object);
            thread->currentObject = nullptr;
            object->isCurrent = 0;
            object->isRead = 1;
            g_queueLock.Leave();

            if (s_threadSleep && ++count == s_threadSleep) {
                OsSleep(1);
                count = 0;
            }
        }

        g_queueLock.Leave();
        OsSleep(1);
    }

    return 0;
}

// VERIFY: 1d3b9267 manual
// OFFSET: 0x004BA8E0
static CAsyncQueue* AsyncFileReadCreateQueue() {
    return s_asyncQueueList.NewNode(TSLIST_LINK_BEFORE, 0, 0);
}

// VERIFY: 3de6d82d auto
// OFFSET: 0x004BA980
static void AsyncFileReadCreateThread(CAsyncQueue* queue, const char* name) {
    CAsyncThread* thread = s_asyncThreadList.NewNode(TSLIST_LINK_BEFORE, 0, 0);
    thread->m_queue = queue;
    thread->currentObject = nullptr;
    SThread::Create(AsyncFileReadThread, thread, thread->m_thread, const_cast<char*>(name), 0);
}

// VERIFY: e94485f6 auto
// OFFSET: 0x004BAA40
void AsyncFileReadInitialize(uint32_t threadSleep, uint32_t handlerTimeout) {
    s_threadSleep = threadSleep;
    s_handlerTimeout = handlerTimeout;

    if (threadSleep > 100) {
        s_threadSleep = 100;
    }

    if (handlerTimeout < 20) {
        s_handlerTimeout = 20;
    }

    EventRegisterEx(EVENT_ID_POLL, AsyncFileReadPollHandler, nullptr, EVENT_PRIORITY_NORMAL);

    if (SFile::IsStreamingMode()) {
        EventRegisterEx(EVENT_ID_IDLE, NetQueuePromoteLocalFiles, nullptr, EVENT_PRIORITY_BELOW_NORMAL);
    }

    s_asyncWaitObject = nullptr;
    s_asyncProgressCallback = nullptr;
    s_asyncProgressCallbackParams = nullptr;
    s_ingameProgressCallback = nullptr;
    s_ingameStartCallback = nullptr;
    s_propContext = PropGetSelectedContext();
    AsyncFileRead::s_shutdownEvent.Reset();

    int32_t queues = SFile::IsStreamingMode() ? 3 : 1;

    for (int32_t i = 0; i < queues; i++) {
        s_asyncQueues[i] = AsyncFileReadCreateQueue();
        AsyncFileReadCreateThread(s_asyncQueues[i], s_asyncQueueNames[i]);
    }

    if (SFile::IsStreamingMode()) {
        s_asyncQueues[2]->m_frameLimited = 1;
    }

    s_userQueueLock = 0;
}

// VERIFY: 35423985 auto
// OFFSET: 0x004BAB50
void AsyncFileReadObject(CAsyncObject* object, int32_t urgent) {
    STORM_ASSERT(object);
    STORM_ASSERT(!object->isRead);
    STORM_ASSERT(!object->queue);
    STORM_ASSERT(!object->isCurrent);
    STORM_ASSERT(!object->isProcessed);
    STORM_ASSERT(object->file);
    STORM_ASSERT(object->buffer);
    STORM_ASSERT(object->size);
    STORM_ASSERT(object->userPostloadCallback);

    CAsyncQueue* queue = s_asyncQueues[0];

    if (SFile::IsStreamingMode()) {
        int32_t local = SFile::FileIsLocal(object->file, 6);

        if (local == SFILE_LOCAL_NO || local == SFILE_LOCAL_PARTIAL) {
            object->m_char24 = 1;
            queue = object->priority > 127 ? s_asyncQueues[2] : s_asyncQueues[1];
        }
    }

    STORM_ASSERT(queue);
    g_queueLock.Enter();
    object->queue = queue;

    if (object == s_asyncWaitObject) {
        object->priority = object->priority > 127 ? 128 : 0;
        object->queuedFrame = g_theGxDevicePtr->m_frameCount;
        queue->readList.LinkToHead(object);
        object->m_char25 = 0;
    } else if (queue->m_frameLimited) {
        AsyncFileReadLinkStaleObject(object, urgent);
    } else {
        AsyncFileReadLinkObject(object, urgent);
    }

    g_queueLock.Leave();

    if (SFile::IsStreamingMode()) {
        SFile::SetFilePriority(object->file, 0, 0);
    }
}

// VERIFY: 2f496060 auto
// OFFSET: 0x004BAC20
void AsyncFileReadIncreasePriority(CAsyncObject* object) {
    STORM_ASSERT(s_userQueueLock);
    object->queuedFrame = g_theGxDevicePtr->m_frameCount;

    if (object->m_char25) {
        AsyncFileReadLinkObject(object, 1);
    }
}

// VERIFY: 3aa538d7 manual
// OFFSET: 0x004BAC50
void AsyncFileReadDestroy() {
    for (CAsyncThread* thread = s_asyncThreadList.Head(); !s_asyncThreadList.IsEnd(thread); thread = s_asyncThreadList.RawNext(thread)) {
        AsyncFileRead::s_shutdownEvent.Set();
        thread->m_thread.Wait(0xFFFFFFFF);
    }

    AsyncFileReadUnloadPostList();
    AsyncFileRead::s_asyncFileReadFreeList.Clear();

#if defined(WOWDECOMP_ASSERTIONS)
    for (CAsyncQueue* queue = s_asyncQueueList.Head(); !s_asyncQueueList.IsEnd(queue); queue = s_asyncQueueList.RawNext(queue)) {
        STORM_ASSERT(queue->readList.Head() == nullptr); // 12213: "queue->readList.Head() == __null"
    }
#endif

    s_asyncQueueList.Clear();
    s_asyncQueues[0] = nullptr;
    s_asyncQueues[1] = nullptr;
    s_asyncQueues[2] = nullptr;

#if defined(WOWDECOMP_ASSERTIONS)
    for (CAsyncThread* thread = s_asyncThreadList.Head(); !s_asyncThreadList.IsEnd(thread); thread = s_asyncThreadList.RawNext(thread)) {
        STORM_ASSERT(thread->currentObject == nullptr); // 12213: "thread->currentObject == __null"
    }
#endif

    s_asyncThreadList.Clear();
    s_asyncPollHandlers.SetCount(0);
    s_asyncStatusHandlers.SetCount(0);
    s_asyncProgressCallback = nullptr;
    s_asyncProgressCallbackParams = nullptr;
    s_ingameProgressCallback = nullptr;
    s_ingameStartCallback = nullptr;

    if (SFile::IsStreamingMode()) {
        EventUnregisterEx(EVENT_ID_IDLE, NetQueuePromoteLocalFiles, nullptr, 2);
    }

    EventUnregisterEx(EVENT_ID_POLL, AsyncFileReadPollHandler, nullptr, 0xFFFFFFFF);
}

// VERIFY: e7566958 auto
// OFFSET: 0x004BAD80
bool AsyncFileReadIsReading() {
    g_queueLock.Enter();

    bool reading = false;

    for (CAsyncThread* thread = s_asyncThreadList.Head(); !s_asyncThreadList.IsEnd(thread); thread = s_asyncThreadList.RawNext(thread)) {
        if (thread->currentObject) {
            reading = true;
        }
    }

    for (CAsyncQueue* queue = s_asyncQueueList.Head(); !s_asyncQueueList.IsEnd(queue); queue = s_asyncQueueList.RawNext(queue)) {
        if (queue->readList.Head()) {
            reading = true;
        }
    }

    if (s_asyncFileReadPostList.Head()) {
        reading = true;
    }

    g_queueLock.Leave();

    return reading;
}

// VERIFY: 3e612f72 auto
// OFFSET: 0x004BAE10
void AsyncFileReadWaitAll() {
    DebugLog("AsyncFileReadWaitAll() Enter\n");

    int32_t total = 0;

    for (uint32_t i = 0; i < s_asyncStatusHandlers.Count(); i++) {
        total += s_asyncStatusHandlers[i]();
    }

    s_progressCount = total;

    while (AsyncFileReadIsReading()) {
        AsyncFileReadPollHandler(nullptr, nullptr);

        if (s_asyncProgressCallback) {
            float progress = 1.0f;

            if (total) {
                progress = static_cast<float>(total - s_progressCount) / total;

                if (progress < 0.0f) {
                    progress = 0.0f;
                } else if (progress > 1.0f) {
                    progress = 1.0f;
                }
            }

            s_asyncProgressCallback(progress, s_asyncProgressCallbackParams);
        }

        OsSleep(1);
    }

    s_asyncProgressCallback = nullptr;
    DebugLog("AsyncFileReadWaitAll() Leave\n");
}
