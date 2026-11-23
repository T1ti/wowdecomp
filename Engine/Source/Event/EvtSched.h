#ifndef ENGINE_SOURCE_EVENT_EVTSCHED_H
#define ENGINE_SOURCE_EVENT_EVTSCHED_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Event/EvtThread.h"
#include "Engine/Source/Event/Types.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Storm/Source/SLock.h"
#include "Engine/Source/Storm/Source/SThread.h"

class EvtContext;

HEVENTCONTEXT AttachContextToThread(EvtContext* context);
void CloseContext(EvtContext* context);
void DestroySchedulerThread(uint32_t hThread);
void DetachContextFromThread(uint32_t hThread, EvtContext* context);
void EventForceIdleProcessing();
EvtContext* GetNextContext(uint32_t hThread);
void IEvtDispatchUpdate(EvtContext* context, uint32_t currTime, float elapsedSec);
HEVENTCONTEXT IEvtSchedulerCreateContext(int32_t interactive, EVENTHANDLERFUNC initializeHandler, EVENTHANDLERFUNC destroyHandler, uint32_t idleTime, uint32_t debugFlags);
void IEvtSchedulerDestroy();
void IEvtSchedulerInitialize(uint32_t threadCount, int32_t netServer);
int32_t IEvtSchedulerIsContextInteractive(EvtContext* context);
void IEvtSchedulerProcess();
void IEvtSchedulerShutdown();
uint32_t InitializeSchedulerThread();
void PutContext(uint32_t nextWakeTime, uint32_t newSmoothWeight, EvtContext* context, uint32_t hThread);
uint32_t STORMAPI SchedulerThreadProc(void* mainThread);
int32_t SchedulerThreadProcProcess(uint32_t hThread, int32_t* watchdogStarted);
void SynthesizeDestroy(EvtContext* context);
void SynthesizeIdle(EvtContext* context, uint32_t currTime, float elapsedSec);
int32_t SynthesizeInitialize(EvtContext* context);
void SynthesizePaint(EvtContext* context);
void SynthesizePoll(EvtContext* context);
uint32_t STORMAPI WaitPumpingNetwork(void* param);

#endif
