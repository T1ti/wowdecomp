#ifndef WOW_SOURCE_BATTLENET_TUMOR_CREEPTENDRIL_H
#define WOW_SOURCE_BATTLENET_TUMOR_CREEPTENDRIL_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Battlenet/BattlenetProtocol.h"
#include "WoW/Source/Battlenet/Tumor/Tendril.h"
#include "WoW/Source/Battlenet/Tumor/TumorManager.h"

namespace Battlenet {
namespace CreepTendril {
class Host {
    public:
    virtual ~Host();
    virtual uint32_t Resolve(uint32_t token, const char* hostname) = 0;
    virtual uint32_t Connect(const void* address, uint32_t addressSize) = 0;
    virtual void Disconnect(uint32_t connection) = 0;
    virtual bool Send(uint32_t connection, const void* data, uint32_t size) = 0;
    virtual uint32_t OpenDatagram(int32_t port) = 0;
    virtual void CloseDatagram(uint32_t connection) = 0;
    virtual bool SendDatagram(uint32_t connection, const void* data, uint32_t size, const void* address, uint32_t addressSize) = 0;
    virtual uint64_t GetCurrentTime() const = 0;
    virtual uint64_t GetCurrentTick() const = 0;
    virtual void LogLevel(const char* message, int32_t level) = 0;
    virtual void Log(const char* message);
    virtual ApplicationFile* OpenApplicationFile(const char* name) = 0;
    virtual void CloseApplicationFile(ApplicationFile* file) = 0;
    virtual void SendEvent(const BSN::EncodedObject* header, const BSN::EncodedObject* data) = 0;
};

class Creep {
    public:
    virtual void unk_00() = 0;
    virtual void unk_04() = 0;
    virtual void EnumFunctions(EnumFunctionsCallback callback, void* param) = 0;
    virtual void Resolved(uint32_t request, const void* info, uint32_t size) = 0;
    virtual void Connected(uint32_t connection, const void* address, uint32_t size) = 0;
    virtual void Disconnected(uint32_t connection) = 0;
    virtual bool Recv(uint32_t connection, void* data, uint32_t size, uint32_t& used) = 0;
    virtual bool RecvDatagram(uint32_t connection, void* data, uint32_t size, const void* address, uint32_t addressSize) = 0;
    virtual void RecvEvent(const BSN::EncodedObject* header, const BSN::EncodedObject* data) = 0;
    virtual void Process(uint32_t time) = 0;
};

typedef bool (*CreepInitializeFunc)();
typedef void (*CreepTeardownFunc)();
typedef Creep* (*CreepCreateFunc)(Host* host, const void* settings, uint32_t size);
typedef void (*CreepDestroyFunc)(Creep* creep);

extern uint32_t s_instances;
extern CreepInitializeFunc s_creepInitialize;
extern CreepTeardownFunc s_creepTeardown;
extern CreepCreateFunc s_creepCreate;
extern CreepDestroyFunc s_creepDestroy;
extern Library* s_creepModule;

bool Initialize();

class Instance : public Tendril, public Host {
    public:
    typedef int32_t (Instance::*EventHandler)(const Event::Header& header, const BSN::EncodedObject* data);

    static Instance* Create(const Authentication::Settings& settings, TumorManager& manager);

    /* 0020 */ Creep* m_creep;
    /* 0024 */ Authentication::Settings m_settings;
    /* 0730 */ EventHandler m_eventHandlers[16];

    void RecvEvent(const Event::Header& header, const BSN::EncodedObject* data) override;
    ~Instance() override;
    void Destroy() override;
    void DoSubscriptions() override;
    void Resolved(uint32_t request, const Client::DNSInfo& info) override;
    void Connected(uint32_t connection, const IP4::AddressPort& address) override;
    void Disconnected(uint32_t connection) override;
    bool Recv(uint32_t connection, void* data, uint32_t size, uint32_t& used) override;
    bool RecvDatagram(uint32_t connection, void* data, uint32_t size, const IP4::AddressPort& address) override;
    void Process(uint32_t time) override;
    uint32_t Resolve(uint32_t token, const char* hostname) override;
    uint32_t Connect(const void* address, uint32_t addressSize) override;
    void Disconnect(uint32_t connection) override;
    bool Send(uint32_t connection, const void* data, uint32_t size) override;
    uint32_t OpenDatagram(int32_t port) override;
    void CloseDatagram(uint32_t connection) override;
    bool SendDatagram(uint32_t connection, const void* data, uint32_t size, const void* address, uint32_t addressSize) override;
    uint64_t GetCurrentTime() const override;
    uint64_t GetCurrentTick() const override;
    void LogLevel(const char* message, int32_t level) override;
    ApplicationFile* OpenApplicationFile(const char* name) override;
    void CloseApplicationFile(ApplicationFile* file) override;
    void SendEvent(const BSN::EncodedObject* header, const BSN::EncodedObject* data) override;

    Instance(const Authentication::Settings& settings, TumorManager& manager);
    void EnumFunctions(EnumFunctionsCallback callback, void* param);
    template <uint8_t EVENT_TYPE>
    int32_t ProcessEventData(const Event::Header& header, const BSN::EncodedObject* data);
};
}
}

// VERIFY: 87aa9a8a manual
// OFFSET: [0x009C2040<1>, 0x009C20A0<2>, 0x009C2100<3>, 0x009C2160<10>, 0x009C21C0<11>, 0x009C2220<5>, 0x009C2280<6>, 0x009C22E0<9>, 0x009C2340<8>, 0x009C23A0<14>, 0x009C2400<15>]
template <uint8_t EVENT_TYPE>
int32_t Battlenet::CreepTendril::Instance::ProcessEventData(const Event::Header& header, const BSN::EncodedObject* data) {
    if (header.m_target.m_type == Event::Header::Target::TYPE_EVENT_ID) {
        uint8_t eventType = EVENT_TYPE;
        static_cast<Service::Interface*>(this)->SendEvent(eventType, reinterpret_cast<const BSN::FourCC&>(header.m_target.m_value), data);
    } else if (header.m_target.m_type == Event::Header::Target::TYPE_ADAPTER) {
        uint32_t target = header.m_target.m_value;
        uint8_t eventType = EVENT_TYPE;
        static_cast<Service::Interface*>(this)->SendEvent(eventType, reinterpret_cast<const BSN::FourCC&>(target), data);
    }

    return 0;
}

#endif
