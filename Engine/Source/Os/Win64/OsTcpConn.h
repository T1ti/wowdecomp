#ifndef ENGINE_SOURCE_OS_WIN64_OSTCPCONN_H
#define ENGINE_SOURCE_OS_WIN64_OSTCPCONN_H

#include <cstddef>
#include <cstdint>
#include <winsock2.h>
#include <mswsock.h>
#include <windows.h>
#include "Engine/Source/Os/OsTcp.h"
#include "Engine/Source/Storm/H/SCritSect.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Storm/H/Types.h"

class SEvent;

namespace OsNet {
extern const char* OSNETERR_INTERNAL;
extern const char* OSNETERR_WSASTARTUPFAILED;
extern const char* OSNETERR_WSASTARTUPVERSION;
extern const char* OSNETERR_SENDFAILED;
extern const char* OSNETERR_CREATETHREADFAILED;
extern const char* OSNETERR_CREATEEVENTFAILED;
extern const char* OSNETERR_BINDFAILED;
extern const char* OSNETERR_LISTENFAILED;
extern const char* OSNETERR_SOCKETFAILED;
extern const char* OSNETERR_SELECTFAILED;
extern const char* OSNETERR_ACCEPTFAILED;
extern const char* OSNETERR_PORTFAILED;
extern const char* OSNETERR_OVERLAPTYPE;
extern const char* OSNETERR_LISTENCLOSED;
extern const char* OSNETERR_ACCEPTEXFAILED;

enum SELECTSET {
    SELECTSET_FIRST = 0,
    SELECTSET_R = 0,
    SELECTSET_W = 1,
    SELECTSET_E = 2,
    SELECT_MAX = 3,
};

enum OVERLAPTYPE {
    OVERLAPTYPE_READ = 0,
    OVERLAPTYPE_WRITE = 1,
    OVERLAPTYPE_ACCEPT = 2,
};

enum OUTPUTSTATE {
    OUTPUTSTATE_WAITING = 0,
    OUTPUTSTATE_WRITING = 1,
    OUTPUTSTATE_COMPLETED = 2,
};

enum CONNLIST {
    CONNLIST_LOOP_CONNECTED = 0,
    CONNLIST_TCP_CONNECTED = 1,
    CONNLIST_UDP_CONNECTED = 2,
    CONNLIST_FILE_CONNECTED = 3,
    CONNLISTS = 4,
    CONNLIST_NONE = 4,
};

struct NETSELECTSETS;
struct TCPACCEPT;
struct TCPNET;

typedef int (WSAAPI* WSARECVPROC)(SOCKET s, LPWSABUF buffers, DWORD bufferCount, LPDWORD bytesReceived, LPDWORD flags, LPWSAOVERLAPPED overlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE completionRoutine);
typedef int (WSAAPI* WSASENDPROC)(SOCKET s, LPWSABUF buffers, DWORD bufferCount, LPDWORD bytesSent, DWORD flags, LPWSAOVERLAPPED overlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE completionRoutine);

struct LOCKEDLONG {
    /* 0000 */ volatile LONG m_value;
};

struct NETOVERLAP {
    /* 0000 */ OVERLAPPED m_overlapped;
    /* 0014 */ OVERLAPTYPE m_type;
};

struct OUTPUT : public TSLinkedNode<OUTPUT> {
    /* 0008 */ NETOVERLAP m_overlap;
    /* 0020 */ OUTPUTSTATE m_state;
    /* 0024 */ uint32_t m_time;
    /* 0028 */ uint32_t m_bytes;
    /* 002C */ uint32_t m_dataBytes;
    /* 0030 */ uint8_t* m_data;
    /* 0034 */ SEvent* m_completionEvent;

    OUTPUT();
    ~OUTPUT();
};

struct NETSELSOCK {
    /* 0004 */ SOCKET m_sock;

    virtual void Selected(TCPNET* net, SELECTSET selectSet) = 0;
    virtual BOOL IsClosed();
    virtual void AddToSelectSets(NETSELECTSETS* selectSets) = 0;
};

struct NETSELSOCKPTR : public TSHashObject<NETSELSOCKPTR, HASHKEY_NONE> {
    /* 0018 */ NETSELSOCK* ptr;
};

struct NETSELECTSETS {
    /* 0000 */ TCPNET* m_net;
    /* 0004 */ fd_set m_sets[SELECT_MAX];
    /* 0310 */ TSHashTableReuse<NETSELSOCKPTR, HASHKEY_NONE> m_selsockTable;

    void AddToSet(NETSELSOCK* selsock, SELECTSET selectSet);
};

struct alignas(8) NETCONN : public NETSELSOCK {
    /* 0008 */ TSLink<NETCONN> m_link;
    /* 0010 */ uint8_t m_list;
    /* 0011 */ uint8_t m_listSlot;
    /* 0012 */ uint16_t m_reserved;
    /* 0014 */ LOCKEDLONG m_refCount;
    /* 0018 */ NETCONNADDR m_connAddr;
    /* 0038 */ int32_t m_eventProcUserLock;
    /* 003C */ NETEVENTPROC m_eventProc;
    /* 0040 */ void* m_user;
    /* 0044 */ CCritSect m_lock;
    /* 005C */ uint32_t m_time;
    /* 0060 */ TCPNET* m_net;

    NETCONN(TCPNET* net, SOCKET sock, NETEVENTPROC eventProc, void* user, const NETCONNADDR* connAddr);
    virtual void CloseAndUnlock() = 0;
    virtual ~NETCONN();
    virtual void IncIo();
    virtual void DecIo();
    virtual void CompleteWrite(NETOVERLAP* overlap, uint32_t bytes);
    virtual void CompleteRead(NETOVERLAP* overlap, uint32_t bytes);
    virtual void Close();
    void DecRef();
    void Disconnect(BOOL notify);
    void GetEventProcAndUser(NETEVENTPROC& eventProc, void*& user);
    int32_t NoteConnect();
    int32_t NoteData(void* data, uint32_t bytes, uint32_t* bytesProcessed, const NETCONNADDR* connAddr);
    int32_t NoteDisconnect();
};

struct NETCONNFULL : public NETCONN {
    NETCONNFULL(TCPNET* net, SOCKET sock, NETEVENTPROC eventProc, void* user, const NETCONNADDR* connAddr);
    virtual void Send(const void* data, uint32_t bytes) = 0;
    virtual OS_SEND SendSync(const void* data, uint32_t bytes, uint32_t* bytesSent, uint32_t timeout) = 0;
    virtual void SetNagle(BOOL enable) = 0;
    virtual int32_t SetWindow(uint32_t size) = 0;
    virtual void SetRecvTimeout(uint32_t timeout) = 0;
};

struct TCPCONN : public NETCONNFULL {
    /* 0068 */ TSList<OUTPUT, TSGetLink<OUTPUT> > m_outputList;
    /* 0074 */ uint32_t m_bytes;
    /* 0078 */ uint8_t m_data[1460];

    TCPCONN(TCPNET* net, SOCKET sock, NETEVENTPROC eventProc, void* user, const NETCONNADDR* connAddr);
    virtual void CloseAndUnlock();
    virtual ~TCPCONN();
    virtual void CompleteWrite(NETOVERLAP* overlap, uint32_t bytes);
    virtual void CompleteRead(NETOVERLAP* overlap, uint32_t bytes);
    virtual void Send(const void* data, uint32_t bytes);
    virtual OS_SEND SendSync(const void* data, uint32_t bytes, uint32_t* bytesSent, uint32_t timeout);
    virtual void SetNagle(BOOL enable);
    virtual int32_t SetWindow(uint32_t size);
    virtual void SetRecvTimeout(uint32_t timeout);
    virtual void StartWriteAndLeaveLock(OUTPUT* output) = 0;
    virtual void StartRead() = 0;
    OUTPUT* LockedEnqueue(const void* data, uint32_t bytes);
};

struct IOTCPCONN : public TCPCONN {
    /* 0630 */ LOCKEDLONG m_ioCount;
    /* 0634 */ NETOVERLAP m_readOverlap;

    IOTCPCONN(TCPNET* net, HANDLE port, SOCKET sock, NETEVENTPROC eventProc, void* user, const NETCONNADDR* connAddr, const void* data, uint32_t bytes);
    virtual void Selected(TCPNET* net, SELECTSET selectSet);
    virtual void AddToSelectSets(NETSELECTSETS* selectSets);
    virtual void IncIo();
    virtual void DecIo();
    virtual void StartWriteAndLeaveLock(OUTPUT* output);
    virtual void StartRead();
};

struct SLTCPCONN : public TCPCONN {
    SLTCPCONN(TCPNET* net, SOCKET sock, NETEVENTPROC eventProc, void* user, const NETCONNADDR* connAddr, const void* data, uint32_t bytes);
    virtual void Selected(TCPNET* net, SELECTSET selectSet);
    virtual void AddToSelectSets(NETSELECTSETS* selectSets);
    virtual void StartWriteAndLeaveLock(OUTPUT* output);
    virtual void StartRead();
    void ContinueRead();
    void ContinueWrite();
};

struct TCPLISTEN : public NETSELSOCK {
    /* 0008 */ TSLink<TCPLISTEN> m_link;
    /* 0010 */ uint32_t m_portAddr;
    /* 0014 */ NETEVENTPROC m_eventProc;
    /* 0018 */ void* m_user;
    /* 001C */ BOOL m_enabled;
    /* 0020 */ TSList<TCPACCEPT, TSGetLink<TCPACCEPT> > m_acceptList;
};

struct TCPACCEPT : public TSLinkedNode<TCPACCEPT> {
    /* 0008 */ NETOVERLAP m_overlap;
    /* 0020 */ CCritSect m_lock;
    /* 0038 */ uint8_t m_addr[64];
    /* 0078 */ TCPLISTEN* m_listen;
    /* 007C */ SOCKET m_sock;

    SOCKET Complete(NETCONNADDR* connAddr, int32_t makeSock);
    void Init();
};

struct NETCONNECT : public NETSELSOCK {
    /* 0008 */ TSLink<NETCONNECT> m_link;
    /* 0010 */ void* m_user;
    /* 0014 */ void* m_data;
    /* 0018 */ uint32_t m_bytes;
};

template <class T, int LINKOFFSET, int SLOTS>
struct TSSlottedListEx {
    /* 0004 */ TSExplicitList<T, LINKOFFSET> m_lists[SLOTS];
    /* ---- */ CCritSect m_locks[SLOTS];
    /* ---- */ LOCKEDLONG m_linkSlot;
    /* ---- */ LOCKEDLONG m_count;

    virtual ~TSSlottedListEx();
    uint8_t LinkNode(T* ptr);
    void UnlinkNode(T* ptr, uint8_t slot);
};

struct TCPNET {
    static TCPNET* s_pnet;
    static HSLOG s_log;
    static int32_t s_qpcexists;
    static float s_qpctoms;
    static int32_t s_pumpShutdown;
    static WSARECVPROC s_WSARecv;
    static WSASENDPROC s_WSASend;
    static LPFN_GETACCEPTEXSOCKADDRS s_GetAcceptExSockaddrs;
    static LPFN_ACCEPTEX s_AcceptEx;

    static void LogWrite(const char* format, ...);

    /* 0000 */ LOCKEDLONG m_refCount;
    /* 0004 */ uint32_t m_pumpThreadCount;
    /* 0008 */ TSGrowableArray<void*> m_pumpThreads;
    /* 0018 */ void* m_udpPumpThread;
    /* 001C */ void* m_udpPumpEvent;
    /* 0020 */ CCritSect m_loopLock;
    /* 0038 */ uint8_t m_loopLists[0x24];
    /* 005C */ TSSlottedListEx<NETCONN, 8, 8> m_connList[CONNLISTS];
    /* 050C */ void* m_listenThread;
    /* 0510 */ TSSlottedListEx<TCPLISTEN, 8, 1> m_listenList;
    /* 0540 */ void* m_baseThread;
    /* 0544 */ void* m_baseEvent;
    /* 0548 */ int32_t m_baseTcpShutdown;
    /* 054C */ void* m_baseTcpShutdownEvent;
    /* 0550 */ TSSlottedListEx<NETCONNECT, 8, 1> m_connectList[CONNLISTS];
    /* 0610 */ HANDLE m_port;
    /* 0614 */ LOCKEDLONG m_hostAddrInfoCount;
    /* 0618 */ void* m_hostAddrInfoLock;
    /* 061C */ uint32_t m_hostAddrInfoId;
    /* 0620 */ uint8_t m_hostAddrInfoList[0xC];

    void CompleteAcceptEx(TCPACCEPT* accept, int32_t makeConn);
    void IoPump(uint32_t timeout);
    void LinkConn(NETCONN* conn, CONNLIST toList);
    void Pump(uint32_t timeout);
    void TcpMakeConn(SOCKET sock, NETEVENTPROC eventProc, void* user, const NETCONNADDR* connAddr, const void* data, uint32_t bytes);
};
}

#endif
