#ifndef WOW_SOURCE_BATTLENET_TUMOR_TUMORMANAGER_H
#define WOW_SOURCE_BATTLENET_TUMOR_TUMORMANAGER_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Battlenet/BattlenetMap.h"
#include "WoW/Source/Battlenet/GeneralFunction.h"
#include "WoW/Source/Battlenet/Tumor/Tumor.h"

namespace Battlenet {
class Tendril;

namespace Authentication {
struct Settings;
}

typedef void (*EnumFunctionsCallback)(void* param, const char* name, GeneralFunction::Function func, void* funcParam);

class TumorManager {
    public:
    static TumorManager* Create(Application& application, const Authentication::Settings& settings);

    /* 0004 */ EventInterface* m_eventAdapter;
    /* 0008 */ Tendril* m_tendrils[20];
    /* 0058 */ Tumor m_tumor;
    /* 0A44 */ uint32_t m_tendrilId;

    virtual void RegisterFunction(const char* name, GeneralFunction::Function func, void* param) = 0;
    virtual void UnregisterFunction(const char* name, GeneralFunction::Function func, void* param) = 0;
    virtual void EnumFunctions(EnumFunctionsCallback callback, void* param) = 0;
    virtual GeneralFunction::Arg CallFunction(const char* name, int32_t argc, GeneralFunction::Arg* args) = 0;
    virtual Tumor* GetTumor();
    virtual void SendEvent(uint8_t eventType, const uint32_t& adapter, const BSN::EncodedObject* data);
    virtual void SendEvent(uint8_t eventType, const BSN::FourCC& eventId, const BSN::EncodedObject* data);
    virtual ~TumorManager();

    TumorManager(Application& application);
    void AddTendril(Tendril* tendril);
    void DestroyTendrils();
    template <class T, class P>
    void SendEvent(const BSN::FourCC& eventId, const T& data);
};

class TumorManagerImplementation : public TumorManager {
    public:
    struct FunctionEntry {
        /* 0000 */ GeneralFunction::Function m_func;
        /* 0004 */ void* m_param;
    };

    /* 0A48 */ Map<FunctionEntry> m_functions;

    void RegisterFunction(const char* name, GeneralFunction::Function func, void* param) override;
    void UnregisterFunction(const char* name, GeneralFunction::Function func, void* param) override;
    void EnumFunctions(EnumFunctionsCallback callback, void* param) override;
    GeneralFunction::Arg CallFunction(const char* name, int32_t argc, GeneralFunction::Arg* args) override;
    ~TumorManagerImplementation() override;

    TumorManagerImplementation(Application& application);
};

GeneralFunction::Arg TumorManagerSendEvent(void* param, int32_t argc, const GeneralFunction::Arg* args);
void TumorManagerRegisterFunction(void* param, const char* name, GeneralFunction::Function func, void* funcParam);

namespace Event {
extern BSN::FourCC CREEP;
extern BSN::FourCC APP;
extern BSN::FourCC DATAGRAM_CONNECTION_ACK;
extern BSN::FourCC S2_LOBBY;
extern BSN::FourCC COMSAT_UPDATE;
extern BSN::FourCC TO_UI;
extern BSN::FourCC COMSAT_STATE;
extern BSN::FourCC FROM_UI;
extern BSN::FourCC DATAGRAM_CONNECTION_INIT;
}
}

// VERIFY: 889ceaaf manual
// OFFSET: 0x008C6F70<Battlenet::Event::Creep::Invoke, Protocol>
template <class T, class P>
void Battlenet::TumorManager::SendEvent(const BSN::FourCC& eventId, const T& data) {
    BSN::BSNEncodedObjectPtr<T, P, 1> object(&data);
    object.IncRef();
    this->SendEvent(Event::EVENT_TYPE_CREEP, eventId, &object);
}

#endif
