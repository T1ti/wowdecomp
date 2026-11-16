#include "WoW/Source/Battlenet/BattlenetProtocol.h"
#include <cstring>

// STATIC: 0x00D4511C
BSN::Protocol::Protocol Protocol::s_protocol(Protocol::s_bytecode, sizeof(Protocol::s_bytecode), Protocol::s_codeInfo, sizeof(Protocol::s_codeInfo) / sizeof(Protocol::s_codeInfo[0]));

// STATIC: 0x00DCEE80
BSN::AsciiString<63> Battlenet::Event::AuthModule::RequestGameAccount("RequestGameAccount");

// STATIC: 0x00DCEEC8
BSN::AsciiString<63> Battlenet::Event::AuthModule::RequestSurveyInfo("RequestSurveyInfo");

// STATIC: 0x00DCEF10
BSN::AsciiString<63> Battlenet::Event::AuthModule::FinalizeMatrixCard("FinalizeMatrixCard");

// STATIC: 0x00DCEF58
BSN::AsciiString<63> Battlenet::Event::AuthModule::RequestToken("RequestToken");

// STATIC: 0x00DCEFA0
BSN::AsciiString<63> Battlenet::Event::AuthModule::CommitMatrixCard("CommitMatrixCard");

// STATIC: 0x00DCEFE8
BSN::AsciiString<63> Battlenet::Event::AuthModule::SurveyDialogComplete("SurveyDialogComplete");

// STATIC: 0x00DCF030
BSN::AsciiString<63> Battlenet::Event::AuthModule::RequestMatrixCard("RequestMatrixCard");

// STATIC: 0x00DCF078
BSN::AsciiString<63> Battlenet::Event::AuthModule::DisplayAgreement("DisplayAgreement");

// STATIC: 0x00DCF0C0
BSN::AsciiString<63> Battlenet::Event::AuthModule::RequestVirtualKeypadPIN("RequestVirtualKeypadPIN");

// STATIC: 0x00DCF108
BSN::AsciiString<63> Battlenet::Event::AuthModule::EnterMatrixCard("EnterMatrixCard");

// STATIC: 0x00DCF150
BSN::AsciiString<63> Battlenet::Event::AuthModule::ClearGameAccounts("ClearGameAccounts");

// STATIC: 0x00DCF198
BSN::AsciiString<63> Battlenet::Event::AuthModule::RevertMatrixCard("RevertMatrixCard");

// STATIC: 0x00DCF1E0
BSN::AsciiString<63> Battlenet::Event::AuthModule::RequestPassword("RequestPassword");

// STATIC: 0x00DCF228
BSN::AsciiString<63> Battlenet::Event::AuthModule::AddGameAccount("AddGameAccount");

// STATIC: 0x00DCF270
BSN::AsciiString<63> Battlenet::Event::AuthModule::ShowSurveyDialog("ShowSurveyDialog");

// STATIC: 0x00DCF2B8
BSN::AsciiString<63> Battlenet::Event::AuthModule::AcceptAgreement("AcceptAgreement");

// STATIC: 0x00D45154
BSN::FourCC Battlenet::Program::WOW("WoW");

// INLINED
Battlenet::Presence::FieldVal::FieldVal() {
    this->m_tag = Battlenet::Presence::FIELDVAL_NONE;
    memset(this->m_value, 0, sizeof(this->m_value));
}

// VERIFY: cfb3016d manual
// OFFSET: 0x005327B0
Battlenet::Friends::FriendInvite& Battlenet::Friends::FriendInvite::operator=(const FriendInvite& other) {
    this->m_presenceId = other.m_presenceId;
    this->m_message = other.m_message;
    this->unk_0210 = other.unk_0210;
    this->m_time = other.m_time;
    return *this;
}

// INLINED
Battlenet::Authentication::ModuleParam::ModuleParam() {
    this->m_tag = -1;
    memset(this->m_value, 0, sizeof(this->m_value));
}

// INLINED
Battlenet::Authentication::Request::Request() {
    this->m_tag = -1;
    memset(this->m_value, 0, sizeof(this->m_value));
}

// INLINED
Battlenet::Authentication::Response::Response() {
    this->m_tag = -1;
    memset(this->m_value, 0, sizeof(this->m_value));
}

// INLINED
Battlenet::Service::State::State() {
    this->m_tag = -1;
    memset(this->m_value, 0, sizeof(this->m_value));
}

// INLINED
Battlenet::Realm::InfoUpdate::InfoUpdate() {
    this->m_tag = -1;
    memset(this->m_value, 0, sizeof(this->m_value));
}

// INLINED
Battlenet::Realm::Request::Request() {
    this->m_tag = -1;
    memset(this->m_value, 0, sizeof(this->m_value));
}

// INLINED
Battlenet::Realm::List::List() {
    this->m_tag = -1;
    memset(this->m_value, 0, sizeof(this->m_value));
}

// INLINED
Battlenet::Realm::JoinResponse::JoinResponse() {
    this->m_tag = -1;
    memset(this->m_value, 0, sizeof(this->m_value));
}

// INLINED
Battlenet::Realm::Response::Response() {
    this->m_tag = -1;
    memset(this->m_value, 0, sizeof(this->m_value));
}

// INLINED
Battlenet::Event::Creep::Portal::Portal() {
    this->m_tag = -1;
    memset(this->m_value, 0, sizeof(this->m_value));
}

// VERIFY: 37efee87 auto
// OFFSET: 0x008C6A40
Battlenet::Event::Creep::Invoke::Invoke() {
    this->m_tag = -1;
    memset(this->m_value, 0, sizeof(this->m_value));
}

// INLINED
Battlenet::Event::Larva::Invoke::Invoke() {
    this->m_tag = -1;
    memset(this->m_value, 0, sizeof(this->m_value));
}

// VERIFY: d7ede7c5 manual
// OFFSET: 0x008C66A0
Battlenet::Authentication::Settings::Settings() {
    this->m_seedLength = 0;
    this->m_program.m_value = 0;
    this->m_platform.m_value = 0;
    this->m_locale.m_value = 0;
    this->m_systemLocale.m_value = 0;
    this->m_components.m_count = 0;

    for (int32_t i = 0; i < 63; i++) {
        this->m_components.m_data[i].m_program.m_value = 0;
        this->m_components.m_data[i].m_platform.m_value = 0;
    }
}

// VERIFY: 12c6d6a9 auto
// OFFSET: 0x009C5090
int32_t ProtocolHard::TypeInfoOf<Battlenet::IP4::AddressPort>::Encode(const Battlenet::IP4::AddressPort& value, BSN::Hard::Encoder& encoder) {
    int32_t result = encoder.PutAlignedData(4, value.m_address);

    if (result) {
        return result;
    }

    return encoder.PutAlignedData(2, value.m_port);
}

// VERIFY: d7de79e1 auto
// OFFSET: 0x009C50C0
int32_t ProtocolHard::TypeInfoOf<Battlenet::IP4::AddressPort>::Decode(Battlenet::IP4::AddressPort& value, BSN::Hard::Decoder& decoder) {
    int32_t result = decoder.GetAlignedData(4, value.m_address);

    if (result) {
        return result;
    }

    return decoder.GetAlignedData(2, value.m_port);
}

// VERIFY: 93bd1407 manual
// OFFSET: 0x009C50F0
int32_t ProtocolHard::TypeInfoOf<Battlenet::IP4::AddressPort>::Encode(const Battlenet::IP4::AddressPort& value, BSN::Hard::SizeEncoder& encoder) {
    encoder.m_size = ((((encoder.m_size + 7) & ~7u) + 32) & ~7u) + 16;
    return BSN::CODEC_VALID;
}

// VERIFY: f38cf32d manual
// OFFSET: 0x009C5110
int32_t ProtocolHard::TypeInfoOf<Battlenet::Client::DNSInfo>::Encode(const Battlenet::Client::DNSInfo& value, BSN::Hard::Encoder& encoder) {
    uint32_t count = value.m_addresses.m_count;

    if (count > 0x7F) {
        return BSN::CODEC_INVALID;
    }

    int32_t result = encoder.PutBits(7, count);

    if (result) {
        return result;
    }

    for (uint32_t i = 0; i < count; i++) {
        result = encoder.PutAlignedData(4, value.m_addresses.m_data[i].m_value);

        if (result) {
            return result;
        }
    }

    uint32_t length = value.m_name.m_length;

    if (length > 0xFF) {
        return BSN::CODEC_INVALID;
    }

    result = encoder.PutBits(8, length);

    if (result) {
        return result;
    }

    return encoder.PutAlignedData(length, reinterpret_cast<const uint8_t*>(value.m_name.m_data));
}

// VERIFY: 451c706b manual
// OFFSET: 0x009C52C0
int32_t ProtocolHard::TypeInfoOf<Battlenet::Client::DNSInfo>::Encode(const Battlenet::Client::DNSInfo& value, BSN::Hard::SizeEncoder& encoder) {
    uint32_t count = value.m_addresses.m_count;

    if (count > 0x7F) {
        return BSN::CODEC_INVALID;
    }

    encoder.m_size += 7;

    for (; count; count--) {
        encoder.m_size = ((encoder.m_size + 7) & ~7u) + 32;
    }

    uint32_t length = value.m_name.m_length;

    if (length > 0xFF) {
        return BSN::CODEC_INVALID;
    }

    encoder.m_size += 8;
    encoder.m_size = ((encoder.m_size + 7) & ~7u) + 8 * length;
    return BSN::CODEC_VALID;
}

// VERIFY: 2482ba92 manual
// OFFSET: 0x009C5320
int32_t ProtocolHard::TypeInfoOf<Battlenet::Authentication::Component>::Encode(const Battlenet::Authentication::Component& value, BSN::Hard::Encoder& encoder) {
    int32_t result = encoder.PutUInt32(value.m_program.m_value);

    if (result) {
        return result;
    }

    result = encoder.PutUInt32(value.m_platform.m_value);

    if (result) {
        return result;
    }

    return encoder.PutBits(32, value.m_version);
}

// VERIFY: 0d670084 manual
// OFFSET: 0x009C5400
int32_t ProtocolHard::TypeInfoOf<Battlenet::Authentication::Settings>::Encode(const Battlenet::Authentication::Settings& value, BSN::Hard::Encoder& encoder) {
    uint32_t seedLength = value.m_seedLength;

    if (seedLength > 0x3FF) {
        return BSN::CODEC_INVALID;
    }

    int32_t result = encoder.PutBits(10, seedLength);

    if (result) {
        return result;
    }

    result = encoder.PutAlignedData(seedLength, value.m_seed);

    if (result) {
        return result;
    }

    result = encoder.PutUInt32(value.m_program.m_value);

    if (result) {
        return result;
    }

    result = encoder.PutUInt32(value.m_platform.m_value);

    if (result) {
        return result;
    }

    result = encoder.PutUInt32(value.m_locale.m_value);

    if (result) {
        return result;
    }

    result = encoder.PutUInt32(value.m_systemLocale.m_value);

    if (result) {
        return result;
    }

    uint32_t componentCount = value.m_components.m_count;

    if (componentCount > 0x3F) {
        return BSN::CODEC_INVALID;
    }

    result = encoder.PutBits(6, componentCount);

    if (result) {
        return result;
    }

    for (uint32_t i = 0; i < componentCount; i++) {
        result = ProtocolHard::TypeInfoOf<Battlenet::Authentication::Component>::Encode(value.m_components.m_data[i], encoder);

        if (result) {
            return result;
        }
    }

    return BSN::CODEC_VALID;
}

// VERIFY: b7d31bbb manual
// OFFSET: 0x009C5610
int32_t ProtocolHard::TypeInfoOf<Battlenet::Authentication::Settings>::Encode(const Battlenet::Authentication::Settings& value, BSN::Hard::SizeEncoder& encoder) {
    uint32_t seedLength = value.m_seedLength;

    if (seedLength > 0x3FF) {
        return BSN::CODEC_INVALID;
    }

    encoder.m_size += 10;
    encoder.m_size = ((encoder.m_size + 7) & ~7u) + 8 * seedLength + 128;

    uint32_t componentCount = value.m_components.m_count;

    if (componentCount > 0x3F) {
        return BSN::CODEC_INVALID;
    }

    encoder.m_size += 6;

    for (uint32_t i = 0; i < componentCount; i++) {
        encoder.m_size += 64;
        encoder.m_size += 32;
    }

    return BSN::CODEC_VALID;
}
