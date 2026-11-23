#include <cstddef>
#include <cstdint>
#include <new>
#include "WoW/Source/SoundInterface/SoundInterface2DSP.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/SoundEngine/SoundEngine.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/SoundInterface/SoundInterface2.h"

// STATIC: 0x00B4ADB4
static TSHashTable<DSP_CHAIN_HEAD_LOOKUP, HASHKEY_STRI> s_dspChains;

// VERIFY: 4b244940 auto
// OFFSET: 0x004CD9D0
static FMOD_DSP_TYPE TranslateDspType(SI2_DSP_TYPE type) {
    switch (type) {
        case SI2_DSP_TYPE_PITCHSHIFT:
            return FMOD_DSP_TYPE_PITCHSHIFT;

        case SI2_DSP_TYPE_HIGHPASS:
            return FMOD_DSP_TYPE_HIGHPASS;

        case SI2_DSP_TYPE_LOWPASS:
            return FMOD_DSP_TYPE_LOWPASS;

        case SI2_DSP_TYPE_CHORUS:
            return FMOD_DSP_TYPE_CHORUS;

        case SI2_DSP_TYPE_ECHO:
            return FMOD_DSP_TYPE_ECHO;

        default:
            return FMOD_DSP_TYPE_UNKNOWN;
    }
}

// INLINED
SI2DspNode::SI2DspNode() {
    this->m_dsp = nullptr;
    this->m_type = SI2_DSP_TYPE_UNINITIALIZED;
}

// INLINED
SI2DspChainHead::SI2DspChainHead() {
    this->m_expireTime = 0.1f;
}

// VERIFY: 3a7e7c8c manual
// OFFSET: 0x004CDC80
void SI2::DSP_MakeDSPsExpire(float elapsed) {
    for (DSP_CHAIN_HEAD_LOOKUP* head = s_dspChains.m_fulllist.Head(); head; head = s_dspChains.m_fulllist.Next(head)) {
        if (SE2::GetChannelGroupNumChannels(head->m_chain.m_channelGroup)) {
            continue;
        }

        if (head->m_chain.m_expireTime > 0.0f && head->m_chain.m_expireTime - elapsed <= 0.0f) {
            for (SI2DspNode* node = head->m_chain.m_nodes.Head(); node; node = head->m_chain.m_nodes.Next(node)) {
                SE2::EnableDSP(node->m_dsp, false);
            }
        }

        head->m_chain.m_expireTime -= elapsed;

        if (head->m_chain.m_expireTime < 0.0f) {
            head->m_chain.m_expireTime = 0.0f;
        }
    }
}

// VERIFY: 427289d5 manual
// OFFSET: 0x004CDD40
void SI2::DSP_ShutDown() {
    for (DSP_CHAIN_HEAD_LOOKUP* head = s_dspChains.m_fulllist.Head(); head; head = s_dspChains.m_fulllist.Next(head)) {
        for (SI2DspNode* node = head->m_chain.m_nodes.Head(); node; node = head->m_chain.m_nodes.Next(node)) {
            SE2::DspDestroy(node->m_dsp);
            node->m_dsp = nullptr;
        }

        head->m_chain.m_nodes.Clear();

        if (head->m_chain.m_channelGroup) {
            SE2::DestroyChannelGroup(head->m_chain.m_channelGroup);
            head->m_chain.m_channelGroup = nullptr;
        }
    }

    s_dspChains.Clear();
}

// VERIFY: 015e69f4 manual
// OFFSET: 0x004CDF70
void SI2::DSP_CreateDsp(const char* pszDspChainName, SI2_DSP_TYPE type, const float* afParams) {
    STORM_ASSERT(pszDspChainName);
    STORM_ASSERT(afParams);

    static const CVar* const s_enableDSPEffects = CVar::Lookup("Sound_EnableDSPEffects");

    if (!s_enableDSPEffects->m_intValue) {
        return;
    }

    SI2DspNode* node = new (SMemAlloc(sizeof(SI2DspNode), __FILE__, __LINE__, 0)) SI2DspNode();

    if (type != SI2_DSP_TYPE_VOLUME && SE2::CreateDSPByType(TranslateDspType(type), &node->m_dsp)) {
        if (node) {
            delete node;
        }

        return;
    }

    DSP_CHAIN_HEAD_LOOKUP* pDSPChainHeadLookup = s_dspChains.Ptr(pszDspChainName);
    STORM_ASSERT(pDSPChainHeadLookup);
    pDSPChainHeadLookup->m_chain.m_nodes.LinkToTail(node);
    node->m_type = type;

    switch (type) {
        case SI2_DSP_TYPE_PITCHSHIFT:
            SE2::SetDspParam(node->m_dsp, 0, afParams[1]);
            SE2::SetDspParam(node->m_dsp, 1, 512.0f);
            break;

        case SI2_DSP_TYPE_HIGHPASS:
        case SI2_DSP_TYPE_LOWPASS:
            SE2::SetDspParam(node->m_dsp, 0, afParams[1]);
            SE2::SetDspParam(node->m_dsp, 1, afParams[2]);
            break;

        case SI2_DSP_TYPE_CHORUS:
            SE2::SetDspParam(node->m_dsp, 0, afParams[1]);
            SE2::SetDspParam(node->m_dsp, 1, afParams[2]);
            SE2::SetDspParam(node->m_dsp, 2, afParams[3]);
            SE2::SetDspParam(node->m_dsp, 3, afParams[4]);
            SE2::SetDspParam(node->m_dsp, 4, afParams[5]);
            SE2::SetDspParam(node->m_dsp, 5, afParams[6]);
            SE2::SetDspParam(node->m_dsp, 6, afParams[7]);
            SE2::SetDspParam(node->m_dsp, 7, afParams[8]);
            break;

        case SI2_DSP_TYPE_ECHO:
            SE2::SetDspParam(node->m_dsp, 0, afParams[1]);
            SE2::SetDspParam(node->m_dsp, 1, afParams[2]);
            SE2::SetDspParam(node->m_dsp, 3, afParams[3]);
            SE2::SetDspParam(node->m_dsp, 4, afParams[4]);
            break;

        case SI2_DSP_TYPE_VOLUME:
            if (afParams[0] > 0.0f) {
                SE2::SetFMODChannelGroupVolume(pDSPChainHeadLookup->m_chain.m_channelGroup, afParams[1]);
            }

            break;

        default:
            break;
    }

    if (type != SI2_DSP_TYPE_VOLUME) {
        SE2::BypassDsp(node->m_dsp, 0.0f == afParams[0]);
        SE2::ConnectDspToChannelGroup(node->m_dsp, pDSPChainHeadLookup->m_chain.m_channelGroup);
    }
}

// VERIFY: e4c6f172 manual
// OFFSET: 0x004CE250
void SI2::DSP_AttachCustomDspChain(FMOD::Channel* pFmodChannel, char* name) {
    STORM_ASSERT(pFmodChannel);

    DSP_CHAIN_HEAD_LOOKUP* head = s_dspChains.Ptr(name);

    if (!head) {
        return;
    }

    for (SI2DspNode* node = head->m_chain.m_nodes.Head(); node; node = head->m_chain.m_nodes.Next(node)) {
        SE2::EnableDSP(node->m_dsp, true);
    }

    head->m_chain.m_expireTime = 3.0f;
    SE2::AddChannelToChannelGroup(pFmodChannel, head->m_chain.m_channelGroup);
}

// VERIFY: 1f3d2404 manual
// OFFSET: 0x004CE7C0
void SI2::DSP_Init() {
    for (int32_t i = 0; i < g_soundFilterDB.GetNumRecords(); i++) {
        SoundFilterRec* pRec = (i < 0 || i >= g_soundFilterDB.GetNumRecords()) ? nullptr : &g_soundFilterDB.m_records[i];
        STORM_ASSERT(pRec);

        if (s_dspChains.Ptr(pRec->m_name)) {
            continue;
        }

        const char* sChannelGroupName = pRec->m_name;
        DSP_CHAIN_HEAD_LOOKUP* head = s_dspChains.New(sChannelGroupName, 0, 0);

        for (SI2DspNode* node = head->m_chain.m_nodes.Head(); node; node = head->m_chain.m_nodes.Next(node)) {
            SE2::DspDestroy(node->m_dsp);
            node->m_dsp = nullptr;
        }

        head->m_chain.m_nodes.Clear();

        if (!head->m_chain.m_channelGroup) {
            STORM_ASSERT(sChannelGroupName);
            STORM_ASSERT(sChannelGroupName[0]);
            head->m_chain.m_channelGroup = SE2::CreateChannelGroup(sChannelGroupName);
        }
    }

    for (int32_t i = 0; i < g_soundFilterElemDB.GetNumRecords(); i++) {
        SoundFilterElemRec* pFilterElemRec = (i < 0 || i >= g_soundFilterElemDB.GetNumRecords()) ? nullptr : &g_soundFilterElemDB.m_records[i];
        STORM_ASSERT(pFilterElemRec);
        SoundFilterRec* pFilterRec = g_soundFilterDB.GetRecord(pFilterElemRec->m_soundFilterID);
        STORM_ASSERT(pFilterRec);

        SI2::DSP_CreateDsp(pFilterRec->m_name, pFilterElemRec->m_filterType, pFilterElemRec->m_params);
    }

    for (DSP_CHAIN_HEAD_LOOKUP* head = s_dspChains.m_fulllist.Head(); head; head = s_dspChains.m_fulllist.Next(head)) {
        for (SI2DspNode* node = head->m_chain.m_nodes.Head(); node; node = head->m_chain.m_nodes.Next(node)) {
            SE2::EnableDSP(node->m_dsp, false);
        }
    }
}
