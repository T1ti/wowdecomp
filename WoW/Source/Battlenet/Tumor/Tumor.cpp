#include "WoW/Source/Battlenet/Tumor/Tumor.h"
#include <cstring>
#include <new>
#include "Engine/Source/Storm/H/storm.h"

// VERIFY: ee519690 manual
// OFFSET: 0x009BFF90
Battlenet::Tumor::Tumor(Application& application) {
    this->m_application = &application;
    this->m_nextTendrilId = 1;

    for (int32_t i = 0; i < NUM_EVENT_BINDINGS; i++) {
        this->m_eventBindings[i].m_subscriber = nullptr;
        this->m_eventBindings[i].m_active = false;
        this->m_eventBindings[i].m_tendrilId = 0;
        this->m_eventBindings[i].m_eventId.m_value = 0;
    }

    for (int32_t i = 0; i < NUM_CONNECTION_BINDINGS; i++) {
        this->m_connectionBindings[i].m_connection = 0;
        this->m_connectionBindings[i].m_client = nullptr;
        this->m_connectionBindings[i].m_active = false;
        this->m_connectionBindings[i].m_tendrilId = 0;
    }

    for (int32_t i = 0; i < NUM_TOKEN_BINDINGS; i++) {
        this->m_tokenBindings[i].m_token = 0;
        this->m_tokenBindings[i].m_request = 0;
        this->m_tokenBindings[i].m_client = nullptr;
        this->m_tokenBindings[i].m_active = false;
    }

    memset(this->m_adapters, 0, sizeof(this->m_adapters));
    memset(this->m_eventAdapters, 0, sizeof(this->m_eventAdapters));
}

// VERIFY: 50fa449a manual
// OFFSET: [0x009C0040, 0x009C10E0]
Battlenet::Tumor::~Tumor() {
    for (int32_t i = 0; i < NUM_ADAPTERS; i++) {
        if (this->m_adapters[i]) {
            Battlenet::Delete(this->m_adapters[i]);
            this->m_adapters[i] = nullptr;
        }
    }

    for (int32_t i = 0; i < NUM_ADAPTERS; i++) {
        if (this->m_eventAdapters[i]) {
            Battlenet::Delete(this->m_eventAdapters[i]);
            this->m_eventAdapters[i] = nullptr;
        }
    }
}

// VERIFY: cfc39af3 auto
// OFFSET: 0x009C0D30
bool Battlenet::Tumor::AllocTendrilId(uint32_t& tendrilId) {
    tendrilId = this->m_nextTendrilId++;
    uint32_t first = tendrilId;

    do {
        uint32_t i = 0;

        while (!this->m_eventAdapters[i] || this->m_eventAdapters[i]->m_tendrilId != tendrilId) {
            if (++i >= NUM_ADAPTERS) {
                return true;
            }
        }

        tendrilId = this->m_nextTendrilId++;
    } while (tendrilId != first);

    return false;
}

// VERIFY: f13c7f2c manual
// OFFSET: 0x009C0DA0
void Battlenet::Tumor::Process(uint32_t time) {
    for (int32_t i = 0; i < NUM_ADAPTERS; i++) {
        if (this->m_adapters[i]) {
            static_cast<SystemClient*>(this->m_adapters[i])->Process(time);
        }
    }
}

// VERIFY: 52d8a946 manual
// OFFSET: 0x009C0DE0
Battlenet::Tumor::ConnectionBinding* Battlenet::Tumor::FindConnectionBinding(uint32_t connection) {
    uint32_t i = 0;

    while (!this->m_connectionBindings[i].m_active || this->m_connectionBindings[i].m_connection != connection) {
        if (++i >= NUM_CONNECTION_BINDINGS) {
            return nullptr;
        }
    }

    return &this->m_connectionBindings[i];
}

// VERIFY: b61bd7a2 manual
// OFFSET: 0x009C0E20
Battlenet::Tumor::TokenBinding* Battlenet::Tumor::FindTokenBinding(uint32_t token) {
    uint32_t i = 0;

    while (!this->m_tokenBindings[i].m_active || this->m_tokenBindings[i].m_token != token) {
        if (++i >= NUM_TOKEN_BINDINGS) {
            return nullptr;
        }
    }

    return &this->m_tokenBindings[i];
}

// VERIFY: 679ecf1c manual
// OFFSET: 0x009C0E60
Battlenet::Tumor::EventBinding* Battlenet::Tumor::AllocEventBinding() {
    uint32_t i = 0;

    while (this->m_eventBindings[i].m_active) {
        if (++i >= NUM_EVENT_BINDINGS) {
            DebugLog("Ran out of room in Tumor::m_eventBindings");
            return nullptr;
        }
    }

    return &this->m_eventBindings[i];
}

// VERIFY: ba73fc1f manual
// OFFSET: 0x009C0EA0
Battlenet::Tumor::ConnectionBinding* Battlenet::Tumor::AllocConnectionBinding() {
    uint32_t i = 0;

    while (this->m_connectionBindings[i].m_active) {
        if (++i >= NUM_CONNECTION_BINDINGS) {
            DebugLog("Ran out of room in Tumor::m_connectionBindings");
            return nullptr;
        }
    }

    return &this->m_connectionBindings[i];
}

// VERIFY: 79a4dbee manual
// OFFSET: 0x009C0EE0
Battlenet::Tumor::TokenBinding* Battlenet::Tumor::AllocTokenBinding() {
    uint32_t i = 0;

    while (this->m_tokenBindings[i].m_active) {
        if (++i >= NUM_TOKEN_BINDINGS) {
            DebugLog("Ran out of room in Tumor::m_tokenBindings");
            return nullptr;
        }
    }

    TokenBinding* binding = &this->m_tokenBindings[i];
    binding->m_token = i;
    return binding;
}

// VERIFY: 629cff03 auto
// OFFSET: 0x009C1190
void Battlenet::Tumor::Subscribe(BSN::FourCC eventId, EventSubscriber& subscriber, const uint32_t& tendrilId) {
    EventBinding* binding = this->AllocEventBinding();

    if (binding) {
        binding->m_eventId = eventId;
        binding->m_subscriber = &subscriber;
        binding->m_active = true;
        binding->m_tendrilId = tendrilId;
    }
}

// VERIFY: 64199477 manual
// OFFSET: 0x009C11C0
Battlenet::Tumor::Adapter* Battlenet::Tumor::AllocAdapter(SystemClient& client, const uint32_t& tendrilId) {
    uint32_t index = 0;

    while (this->m_adapters[index]) {
        if (++index >= NUM_ADAPTERS) {
            DebugLog("No space in Tumor::m_adapters for new Tendril");
            return nullptr;
        }
    }

    void* memory = Battlenet::g_allocator->Alloc(sizeof(Adapter));
    Adapter* adapter = memory ? new (memory) Adapter(client, this, index, tendrilId) : nullptr;
    this->m_adapters[index] = adapter;
    return adapter;
}

// VERIFY: a039aea5 manual
// OFFSET: 0x009C1250
Battlenet::Tumor::EventAdapter* Battlenet::Tumor::AllocEventAdapter(BSN::FourCC eventId, EventSubscriber& subscriber, const uint32_t& tendrilId) {
    uint32_t index = 0;

    while (this->m_eventAdapters[index]) {
        if (++index >= NUM_ADAPTERS) {
            DebugLog("No space in Tumor::m_eventAdapters for new Tendril");
            return nullptr;
        }
    }

    void* memory = Battlenet::g_allocator->Alloc(sizeof(EventAdapter));
    EventAdapter* adapter = memory ? new (memory) EventAdapter(eventId, subscriber, this, index, tendrilId) : nullptr;
    this->m_eventAdapters[index] = adapter;
    return adapter;
}

// VERIFY: a2168cb9 manual
// OFFSET: 0x009C12F0
void Battlenet::Tumor::RemoveTendril(const uint32_t& tendrilId) {
    for (int32_t i = 0; i < NUM_ADAPTERS; i++) {
        if (this->m_eventAdapters[i] && this->m_eventAdapters[i]->m_tendrilId == tendrilId) {
            Battlenet::Delete(this->m_eventAdapters[i]);
            this->m_eventAdapters[i] = nullptr;
        }
    }

    for (int32_t i = 0; i < NUM_ADAPTERS; i++) {
        if (this->m_adapters[i] && this->m_adapters[i]->m_tendrilId == tendrilId) {
            Battlenet::Delete(this->m_adapters[i]);
            this->m_adapters[i] = nullptr;
        }
    }

    for (int32_t i = 0; i < NUM_EVENT_BINDINGS; i++) {
        if (this->m_eventBindings[i].m_active && this->m_eventBindings[i].m_tendrilId == tendrilId) {
            this->m_eventBindings[i].m_active = false;
        }
    }

    for (int32_t i = 0; i < NUM_CONNECTION_BINDINGS; i++) {
        if (this->m_connectionBindings[i].m_active && this->m_connectionBindings[i].m_tendrilId == tendrilId) {
            this->m_connectionBindings[i].m_active = false;
        }
    }
}

// VERIFY: 8e36495e manual
// OFFSET: 0x009C1400
uint32_t Battlenet::Tumor::Connect(uint32_t index, const IP4::AddressPort& address, const uint32_t& tendrilId) {
    ConnectionBinding* binding = this->AllocConnectionBinding();

    if (!binding) {
        DebugLog("Couldn't get a connectionId!");
        return static_cast<uint32_t>(-1);
    }

    uint32_t connection = this->m_application->Connect(address, tendrilId);
    binding->m_connection = connection;
    binding->m_client = this->m_adapters[index];
    binding->m_active = true;
    binding->m_tendrilId = tendrilId;
    return connection;
}

// VERIFY: c1f7add6 manual
// OFFSET: 0x009C1490
uint32_t Battlenet::Tumor::OpenDatagram(uint32_t index, int32_t port, const uint32_t& tendrilId) {
    ConnectionBinding* binding = this->AllocConnectionBinding();

    if (!binding) {
        DebugLog("Couldn't get a connectionId!");
        return static_cast<uint32_t>(-1);
    }

    uint32_t connection = this->m_application->OpenDatagram(port, tendrilId);
    binding->m_connection = connection;
    binding->m_client = this->m_adapters[index];
    binding->m_active = true;
    binding->m_tendrilId = tendrilId;
    return connection;
}

// VERIFY: b017cd8b auto
// OFFSET: 0x009C1520
void Battlenet::Tumor::Resolved(uint32_t request, const Client::DNSInfo& info) {
    TokenBinding* binding = this->FindTokenBinding(request);

    if (binding) {
        binding->m_client->Resolved(binding->m_request, info);
        binding->m_token = 0;
        binding->m_client = nullptr;
        binding->m_active = false;
    }
}

// VERIFY: 697d6e84 auto
// OFFSET: 0x009C1560
void Battlenet::Tumor::Connected(uint32_t connection, const IP4::AddressPort& address) {
    ConnectionBinding* binding = this->FindConnectionBinding(connection);

    if (binding) {
        binding->m_client->Connected(connection, address);
    }
}

// VERIFY: dc021d45 auto
// OFFSET: 0x009C1590
void Battlenet::Tumor::Disconnected(uint32_t connection) {
    ConnectionBinding* binding = this->FindConnectionBinding(connection);

    if (binding) {
        binding->m_client->Disconnected(connection);
        binding->m_connection = 0;
        binding->m_client = nullptr;
        binding->m_active = false;
    }
}

// VERIFY: 6ac0b5ba manual
// OFFSET: 0x009C15D0
bool Battlenet::Tumor::Recv(uint32_t connection, void* data, uint32_t size, uint32_t& used) {
    ConnectionBinding* binding = this->FindConnectionBinding(connection);

    if (binding) {
        return binding->m_client->Recv(connection, data, size, used);
    }

    return false;
}

// VERIFY: 4c7c3bb7 manual
// OFFSET: 0x009C1610
bool Battlenet::Tumor::RecvDatagram(uint32_t connection, void* data, uint32_t size, const IP4::AddressPort& address) {
    ConnectionBinding* binding = this->FindConnectionBinding(connection);

    if (binding) {
        return binding->m_client->RecvDatagram(connection, data, size, address);
    }

    return false;
}

// VERIFY: 4e7878b6 manual
// OFFSET: 0x009C1650
void Battlenet::Tumor::SendEvent(const Event::Header& header, const BSN::EncodedObject* data) {
    if (header.m_target.m_type == Event::Header::Target::TYPE_EVENT_ID) {
        for (int32_t i = 0; i < NUM_EVENT_BINDINGS; i++) {
            if (this->m_eventBindings[i].m_active && this->m_eventBindings[i].m_eventId.m_value == header.m_target.m_value) {
                this->m_eventBindings[i].m_subscriber->RecvEvent(header, data);
            }
        }
    } else if (header.m_target.m_type == Event::Header::Target::TYPE_ADAPTER) {
        EventAdapter* adapter = this->m_eventAdapters[header.m_target.m_value];

        if (adapter) {
            static_cast<EventSubscriber*>(adapter)->RecvEvent(header, data);
        }
    }
}

// INLINED
Battlenet::Tumor::Adapter::Adapter(SystemClient& client, Tumor* tumor, uint32_t index, const uint32_t& tendrilId) {
    this->m_tendrilId = tendrilId;
    this->m_index = index;
    this->m_tumor = tumor;
    this->m_client = &client;
}

// VERIFY: 3658c352 manual
// OFFSET: 0x009C16C0
uint32_t Battlenet::Tumor::Adapter::Resolve(uint32_t token, const char* hostname) {
    Tumor* tumor = this->m_tumor;
    uint32_t index = this->m_index;
    TokenBinding* binding = tumor->AllocTokenBinding();

    if (!binding) {
        return 0;
    }

    binding->m_request = token;
    binding->m_client = tumor->m_adapters[index];
    binding->m_active = true;
    return tumor->m_application->Resolve(binding->m_token, hostname);
}

// VERIFY: 15c88fc4 auto
// OFFSET: 0x009C1710
uint32_t Battlenet::Tumor::Adapter::Connect(const IP4::AddressPort& address, const uint32_t& tendrilId) {
    return this->m_tumor->Connect(this->m_index, address, tendrilId);
}

// VERIFY: 2cff7579 manual
// OFFSET: 0x009C1730
void Battlenet::Tumor::Adapter::Disconnect(uint32_t connection) {
    this->m_tumor->m_application->Disconnect(connection);
}

// VERIFY: 8a9978de auto
// OFFSET: 0x009C1750
bool Battlenet::Tumor::Adapter::Send(uint32_t connection, const void* data, uint32_t size) {
    return this->m_tumor->m_application->Send(connection, data, size);
}

// VERIFY: 5d3bbf14 auto
// OFFSET: 0x009C1770
uint32_t Battlenet::Tumor::Adapter::OpenDatagram(int32_t port, const uint32_t& tendrilId) {
    return this->m_tumor->OpenDatagram(this->m_index, port, tendrilId);
}

// VERIFY: 57407b2a manual
// OFFSET: 0x009C0FD0
void Battlenet::Tumor::Adapter::CloseDatagram(uint32_t connection) {
    this->m_tumor->m_application->CloseDatagram(connection);
}

// VERIFY: 94a86319 auto
// OFFSET: 0x009C0FF0
bool Battlenet::Tumor::Adapter::SendDatagram(uint32_t connection, const void* data, uint32_t size, const IP4::AddressPort& address) {
    return this->m_tumor->m_application->SendDatagram(connection, data, size, address);
}

// VERIFY: e14a1641 auto
// OFFSET: 0x009C1010
uint64_t Battlenet::Tumor::Adapter::GetCurrentTime() const {
    return this->m_tumor->m_application->GetCurrentTime();
}

// VERIFY: b92489b0 auto
// OFFSET: 0x009C1030
uint64_t Battlenet::Tumor::Adapter::GetCurrentTick() const {
    return this->m_tumor->m_application->GetCurrentTick();
}

// VERIFY: 028879f4 manual
// OFFSET: 0x009C1050
void Battlenet::Tumor::Adapter::LogLevel(const char* message, int32_t level) {
    this->m_tumor->m_application->LogLevel(message, level);
}

// VERIFY: aae911a4 manual
// OFFSET: 0x009C1070
Battlenet::ApplicationFile* Battlenet::Tumor::Adapter::OpenApplicationFile(const char* name) {
    return this->m_tumor->m_application->OpenApplicationFile(name);
}

// VERIFY: 4a63c7a1 manual
// OFFSET: 0x009C1090
void Battlenet::Tumor::Adapter::CloseApplicationFile(ApplicationFile* file) {
    this->m_tumor->m_application->CloseApplicationFile(file);
}

// VERIFY: 1cf32261 manual
// OFFSET: [0x009C0F30, 0x009C0F50]
Battlenet::Tumor::Adapter::~Adapter() {
}

// VERIFY: 7d67d9cc auto
// OFFSET: 0x009C10B0
void Battlenet::Tumor::Adapter::Destroy() {
    Battlenet::Delete(this);
}

// VERIFY: 570486f7 manual
// OFFSET: 0x009C0C70
void Battlenet::Tumor::Adapter::Resolved(uint32_t request, const Client::DNSInfo& info) {
    this->m_client->Resolved(request, info);
}

// VERIFY: c288be8e manual
// OFFSET: 0x009C0C80
void Battlenet::Tumor::Adapter::Connected(uint32_t connection, const IP4::AddressPort& address) {
    this->m_client->Connected(connection, address);
}

// VERIFY: edf04550 manual
// OFFSET: 0x009C0C90
void Battlenet::Tumor::Adapter::Disconnected(uint32_t connection) {
    this->m_client->Disconnected(connection);
}

// VERIFY: a3a75e38 auto
// OFFSET: 0x009C0CA0
bool Battlenet::Tumor::Adapter::Recv(uint32_t connection, void* data, uint32_t size, uint32_t& used) {
    return this->m_client->Recv(connection, data, size, used);
}

// VERIFY: 74df147b auto
// OFFSET: 0x009C0CB0
bool Battlenet::Tumor::Adapter::RecvDatagram(uint32_t connection, void* data, uint32_t size, const IP4::AddressPort& address) {
    return this->m_client->RecvDatagram(connection, data, size, address);
}

// VERIFY: 4033000e manual
// OFFSET: 0x009C0CC0
void Battlenet::Tumor::Adapter::Process(uint32_t time) {
    this->m_client->Process(time);
}

// INLINED
Battlenet::Tumor::EventAdapter::EventAdapter(BSN::FourCC eventId, EventSubscriber& subscriber, Tumor* tumor, uint32_t index, const uint32_t& tendrilId) {
    this->m_eventId = eventId;
    this->m_tendrilId = tendrilId;
    this->m_tumor = tumor;
    this->m_index = index;
    this->m_subscriber = &subscriber;
}

// VERIFY: adedbd5a auto
// OFFSET: 0x009C1790
void Battlenet::Tumor::EventAdapter::SendEvent(uint8_t eventType, uint32_t adapter, const BSN::EncodedObject* data) {
    Event::Header header;
    header.m_eventType = eventType;
    header.m_source.m_exists = true;
    header.m_source.m_adapter = this->m_index;
    header.m_source.m_event = this->m_eventId.m_value;
    header.m_target.m_type = Event::Header::Target::TYPE_ADAPTER;
    header.m_target.m_value = adapter;
    this->m_tumor->SendEvent(header, data);
}

// VERIFY: cce6fea3 auto
// OFFSET: 0x009C17E0
void Battlenet::Tumor::EventAdapter::SendEvent(uint8_t eventType, BSN::FourCC eventId, const BSN::EncodedObject* data) {
    Event::Header header;
    header.m_eventType = eventType;
    header.m_source.m_exists = true;
    header.m_source.m_adapter = this->m_index;
    header.m_source.m_event = this->m_eventId.m_value;
    header.m_target.m_type = Event::Header::Target::TYPE_EVENT_ID;
    header.m_target.m_value = eventId.m_value;
    this->m_tumor->SendEvent(header, data);
}

// VERIFY: 5fca2ace auto
// OFFSET: 0x009C0CD0
void Battlenet::Tumor::EventAdapter::Subscribe(BSN::FourCC eventId, const uint32_t& tendrilId) {
    this->m_tumor->Subscribe(eventId, *this, tendrilId);
}

// VERIFY: f9e9eef3 manual
// OFFSET: 0x009C0F90
Battlenet::Tumor::EventAdapter::~EventAdapter() {
}

// VERIFY: f39e6a3a manual
// OFFSET: 0x009C0D00
void Battlenet::Tumor::EventAdapter::RecvEvent(const Event::Header& header, const BSN::EncodedObject* data) {
    this->m_subscriber->RecvEvent(header, data);
}
