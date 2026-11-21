#ifndef WOW_SOURCE_BATTLENET_TUMOR_TUMOR_H
#define WOW_SOURCE_BATTLENET_TUMOR_TUMOR_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Battlenet/Battlenet.h"

namespace Battlenet {
class Tumor : public SystemClient {
    public:
    static const uint32_t NUM_ADAPTERS = 20;
    static const uint32_t NUM_EVENT_BINDINGS = 20;
    static const uint32_t NUM_CONNECTION_BINDINGS = 64;
    static const uint32_t NUM_TOKEN_BINDINGS = 64;

    class Adapter : public SystemServer, public SystemClient {
        public:
        /* 0008 */ uint32_t m_tendrilId;
        /* 000C */ uint32_t m_index;
        /* 0010 */ Tumor* m_tumor;
        /* 0014 */ SystemClient* m_client;

        uint32_t Resolve(uint32_t token, const char* hostname) override;
        uint32_t Connect(const IP4::AddressPort& address, const uint32_t& tendrilId) override;
        void Disconnect(uint32_t connection) override;
        bool Send(uint32_t connection, const void* data, uint32_t size) override;
        uint32_t OpenDatagram(int32_t port, const uint32_t& tendrilId) override;
        void CloseDatagram(uint32_t connection) override;
        bool SendDatagram(uint32_t connection, const void* data, uint32_t size, const IP4::AddressPort& address) override;
        uint64_t GetCurrentTime() const override;
        uint64_t GetCurrentTick() const override;
        void LogLevel(const char* message, int32_t level) override;
        ApplicationFile* OpenApplicationFile(const char* name) override;
        void CloseApplicationFile(ApplicationFile* file) override;
        ~Adapter() override;
        virtual void Destroy();
        void Resolved(uint32_t request, const Client::DNSInfo& info) override;
        void Connected(uint32_t connection, const IP4::AddressPort& address) override;
        void Disconnected(uint32_t connection) override;
        bool Recv(uint32_t connection, void* data, uint32_t size, uint32_t& used) override;
        bool RecvDatagram(uint32_t connection, void* data, uint32_t size, const IP4::AddressPort& address) override;
        void Process(uint32_t time) override;

        Adapter(SystemClient& client, Tumor* tumor, uint32_t index, const uint32_t& tendrilId);
    };

    class EventAdapter : public EventInterface, public EventSubscriber {
        public:
        /* 000C */ uint32_t m_tendrilId;
        /* 0010 */ Tumor* m_tumor;
        /* 0014 */ uint32_t m_index;
        /* 0018 */ EventSubscriber* m_subscriber;

        void SendEvent(uint8_t eventType, uint32_t adapter, const BSN::EncodedObject* data) override;
        void SendEvent(uint8_t eventType, BSN::FourCC eventId, const BSN::EncodedObject* data) override;
        void Subscribe(BSN::FourCC eventId, const uint32_t& tendrilId) override;
        ~EventAdapter() override;
        void RecvEvent(const Event::Header& header, const BSN::EncodedObject* data) override;

        EventAdapter(BSN::FourCC eventId, EventSubscriber& subscriber, Tumor* tumor, uint32_t index, const uint32_t& tendrilId);
    };

    struct EventBinding {
        /* 0000 */ BSN::FourCC m_eventId;
        /* 0004 */ EventSubscriber* m_subscriber;
        /* 0008 */ bool m_active;
        /* 000C */ uint32_t m_tendrilId;
    };

    struct ConnectionBinding {
        /* 0000 */ uint32_t m_connection;
        /* 0004 */ SystemClient* m_client;
        /* 0008 */ bool m_active;
        /* 000C */ uint32_t m_tendrilId;
    };

    struct TokenBinding {
        /* 0000 */ uint32_t m_token;
        /* 0004 */ uint32_t m_request;
        /* 0008 */ SystemClient* m_client;
        /* 000C */ bool m_active;
    };

    /* 0004 */ Adapter* m_adapters[NUM_ADAPTERS];
    /* 0054 */ EventAdapter* m_eventAdapters[NUM_ADAPTERS];
    /* 00A4 */ uint32_t m_nextTendrilId;
    /* 00A8 */ Application* m_application;
    /* 00AC */ EventBinding m_eventBindings[NUM_EVENT_BINDINGS];
    /* 01EC */ ConnectionBinding m_connectionBindings[NUM_CONNECTION_BINDINGS];
    /* 05EC */ TokenBinding m_tokenBindings[NUM_TOKEN_BINDINGS];

    void Resolved(uint32_t request, const Client::DNSInfo& info) override;
    void Connected(uint32_t connection, const IP4::AddressPort& address) override;
    void Disconnected(uint32_t connection) override;
    bool Recv(uint32_t connection, void* data, uint32_t size, uint32_t& used) override;
    bool RecvDatagram(uint32_t connection, void* data, uint32_t size, const IP4::AddressPort& address) override;
    void Process(uint32_t time) override;
    ~Tumor() override;
    virtual bool AllocTendrilId(uint32_t& tendrilId);
    virtual EventAdapter* AllocEventAdapter(BSN::FourCC eventId, EventSubscriber& subscriber, const uint32_t& tendrilId);
    virtual void RemoveTendril(const uint32_t& tendrilId);
    virtual void Subscribe(BSN::FourCC eventId, EventSubscriber& subscriber, const uint32_t& tendrilId);

    Tumor(Application& application);
    Adapter* AllocAdapter(SystemClient& client, const uint32_t& tendrilId);
    ConnectionBinding* FindConnectionBinding(uint32_t connection);
    TokenBinding* FindTokenBinding(uint32_t token);
    EventBinding* AllocEventBinding();
    ConnectionBinding* AllocConnectionBinding();
    TokenBinding* AllocTokenBinding();
    uint32_t Connect(uint32_t index, const IP4::AddressPort& address, const uint32_t& tendrilId);
    uint32_t OpenDatagram(uint32_t index, int32_t port, const uint32_t& tendrilId);
    void SendEvent(const Event::Header& header, const BSN::EncodedObject* data);
};
}

#endif
