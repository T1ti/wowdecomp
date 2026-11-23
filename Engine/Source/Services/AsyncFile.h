#ifndef ENGINE_SOURCE_SERVICES_ASYNCFILE_H
#define ENGINE_SOURCE_SERVICES_ASYNCFILE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Services/Types.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Storm/Source/SThread.h"

class CAsyncQueue : public TSLinkedNode<CAsyncQueue> {
    public:
    /* 0008 */ TSExplicitList<CAsyncObject, offsetof(CAsyncObject, link)> readList;
    /* 0014 */ TSExplicitList<CAsyncObject, offsetof(CAsyncObject, link)> m_staleList;
    /* 0020 */ int32_t m_frameLimited;

    ~CAsyncQueue();
};

class CAsyncThread : public TSLinkedNode<CAsyncThread> {
    public:
    /* 0008 */ SThread m_thread;
    /* 000C */ CAsyncQueue* m_queue;
    /* 0010 */ CAsyncObject* currentObject;
};

class AsyncFile {
    public:
    static int32_t Cancel(CAsyncObject* object, void (*callback)(CAsyncObject*));
    static void EnterQueueLock();
    static void LeaveQueueLock();
};

void AsyncFileReadAddPollHandler(void (*handler)());
void AsyncFileReadAddStatusHandler(int32_t (*handler)());
CAsyncObject* AsyncFileReadAllocObject();
void AsyncFileReadDestroy();
void AsyncFileReadDestroyObject(CAsyncObject* object);
void AsyncFileReadIncreasePriority(CAsyncObject* object);
void AsyncFileReadIngameProgressCallback(void (*startCallback)(), void (*progressCallback)(float, int32_t));
void AsyncFileReadInitialize(uint32_t threadSleep, uint32_t handlerTimeout);
bool AsyncFileReadIsReading();
void AsyncFileReadLinkObject(CAsyncObject* object, int32_t urgent);
void AsyncFileReadObject(CAsyncObject* object, int32_t urgent);
int32_t AsyncFileReadPollHandler(const void* data, void* param);
void AsyncFileReadSetProgressCallback(void (*callback)(float, void*), void* param);
void AsyncFileReadWait(CAsyncObject* object);
void AsyncFileReadWaitAll();

#endif
