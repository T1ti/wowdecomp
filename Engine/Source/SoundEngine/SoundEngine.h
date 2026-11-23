#ifndef ENGINE_SOURCE_SOUNDENGINE_SOUNDENGINE_H
#define ENGINE_SOURCE_SOUNDENGINE_SOUNDENGINE_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/SoundEngine/fmodex/src/fmod.hpp"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CRange.h"

class SESound;
class SESoundInternal;
class SESoundUserData;
class SFileImpl;

#define SE_ERRCHECK(result) SE2::ErrCheck(__LINE__, __FILE__, (result))
#define SE_LOG_INIT_FAILED(result) SE2::Log_Write(__LINE__, __FILE__, FMOD_OK, " -###########################################################################################"); SE2::Log_Write(__LINE__, __FILE__, (result), " -######## ERROR INITIALIZING. ALL GAME SOUND DISABLED."); SE2::Log_Write(__LINE__, __FILE__, FMOD_OK, " -###########################################################################################")
#define SE_LOG_CHAT_INIT_FAILED() SE2::Log_Write(__LINE__, __FILE__, FMOD_OK, " -###########################################################################################"); SE2::Log_Write(__LINE__, __FILE__, FMOD_OK, " -######## ERROR INITIALIZING. VOICE CHAT DISABLED."); SE2::Log_Write(__LINE__, __FILE__, FMOD_OK, " -###########################################################################################")

enum SESOUNDTYPE {
    SESOUNDTYPE_NONE = 0,
    SESOUNDTYPE_FILE = 1,
    SESOUNDTYPE_MEMORY = 2,
    SESOUNDTYPE_CHAT = 3,
};

enum SECHANNELSTATE {
    SECHANNELSTATE_NONE = 0,
    SECHANNELSTATE_LOADING = 1,
    SECHANNELSTATE_PLAYING = 2,
    SECHANNELSTATE_VIRTUAL = 3,
    SECHANNELSTATE_OTHER = 4,
};

// Game policy values translated by SetSoundGroupMaxAudibleBehavior.
enum SE2_MAX_AUDIBLE_BEHAVIOR : int32_t {
    SE2_MAX_AUDIBLE_MUTE = 0,
    SE2_MAX_AUDIBLE_STEAL_LOWEST = 1,
    SE2_MAX_AUDIBLE_FAIL = 2,
};

// SEFileSound::m_priority values. FMOD channel priorities run 0 (most important) to 256 with
// 128 as the channel default; UNSET (-1) leaves the channel default in place. PLAYING is the
// step above the default given to a non-looping sound once it plays (name inferred).
enum SE2_PRIORITY : int32_t {
    SE2_PRIORITY_UNSET = -1,
    SE2_PRIORITY_PLAYING = 127,
    SE2_PRIORITY_DEFAULT = 128,
    SE2_PRIORITY_MAX = 256,
};

// Sound_OutputQuality values ("sound quality, default 1 (medium)"); LOW/HIGH inferred from the
// sample rate (22050 for 0, 44100 for 1) and the linear resampler picked only for 2.
enum SE2_OUTPUT_QUALITY : int32_t {
    SE2_OUTPUT_QUALITY_LOW = 0,
    SE2_OUTPUT_QUALITY_MEDIUM = 1,
    SE2_OUTPUT_QUALITY_HIGH = 2,
};

// Sound_EnableMixMode2 values, from the " -# n <format>" log lines; 0 leaves mix mode 2 off.
enum SE2_MIX_MODE : int32_t {
    SE2_MIX_MODE_OFF = 0,
    SE2_MIX_MODE_PCMFLOAT = 1,
    SE2_MIX_MODE_PCM24 = 2,
    SE2_MIX_MODE_PCM32 = 3,
};

struct SEDriverInfo {
    /* 0000 */ char m_name[256];
};

struct SEChannelGroup {
    /* 0000 */ uint32_t m_hash;
    /* 0004 */ int32_t m_parent;
    /* 0008 */ float m_volume;
    /* 000C */ float m_muteVolume;
    /* 0010 */ uint8_t m_dirty;
};

struct SEChannelInfo {
    /* 0000 */ SECHANNELSTATE m_state;
    /* 0004 */ C3Vector m_position;
    /* 0010 */ CRange m_distanceRange;
    /* 0018 */ uint8_t m_is3D;
    /* 0019 */ uint8_t m_looping;
    /* 001A */ uint8_t m_fadingIn;
    /* 001B */ uint8_t m_fadingOut;
    /* 001C */ SESoundUserData* m_userData;
    /* 0020 */ float m_fadeVolume;
    /* 0024 */ float m_distanceSquared;
    /* 0028 */ float m_volume;
    /* 002C */ int32_t m_priority;
    /* 0030 */ float m_audibility;
    /* 0034 */ float m_channelFadeVolume;
    /* 0038 */ uint8_t m_hardware;
    /* 003C */ int32_t m_cached;
    /* 0040 */ float m_panLevel;
};

class SESoundUserData {
    public:
    virtual ~SESoundUserData();
    virtual void Release();
};

class SECaptureCallbacks {
    public:
    virtual ~SECaptureCallbacks();
    virtual void ProcessData(int16_t* data, uint32_t samples) = 0;
};

class SEStreamCallbacks {
    public:
    virtual ~SEStreamCallbacks();
    virtual bool GetSamples(uint32_t position, uint32_t count, float* buffer) = 0;
    virtual void DiscardSamples(uint32_t position, uint32_t count);
    virtual void Delete();
};

class SEWaveDataNode : public TSLinkedNode<SEWaveDataNode> {
    public:
    /* 0008 */ FMOD::Sound* pSound;
    /* 000C */ BOOL m_loaded;
    /* 0010 */ char m_name[128];
    /* 0090 */ uint32_t m_hash;
    /* 0094 */ int32_t nRefCount;
    /* 0098 */ uint32_t m_size;

    SEWaveDataNode();
    ~SEWaveDataNode();
};

class SESoundInternal : public TSLinkedNode<SESoundInternal> {
    public:
    void AttachHandle(SESound& handle); // (inferred)

    /* 0010 */ FMOD::System* m_system;
    /* 0014 */ FMOD::Channel* m_pFMODChannel;
    /* 0018 */ SESound* m_handle;
    /* 001C */ SESoundUserData* m_userData;
    /* 0020 */ float m_volume;
    /* 0024 */ float m_fadeVolume;
    /* 0028 */ float m_fadeInTime;
    /* 002C */ float m_fadeOutTime;
    /* 0030 */ uint8_t m_bFadingIn;
    /* 0031 */ uint8_t m_bFadingOut;
    /* 0034 */ uint32_t m_playTime;
    /* 0038 */ int32_t m_cached;
    /* 003C */ SESOUNDTYPE m_type;
    /* 0040 */ TSLink<SESoundInternal> m_deleteLink;
    /* 0048 */ TSLink<SESoundInternal> m_objectLink;
    /* 0050 */ C3Vector m_position;
    /* 005C */ CRange m_distanceRange;
    /* 0064 */ int32_t m_channelGroup;
    /* 0068 */ FMOD_MODE m_mode;
    /* 006C */ uint8_t m_play;
    /* 006D */ uint8_t m_stop;
    /* 0070 */ float m_frequency;
    /* 0074 */ float m_pitch;
    /* 0078 */ int32_t m_priority;
    /* 007C */ int32_t m_deleteDelay;
    /* 0080 */ int32_t m_reverb;
    /* 0084 */ int32_t m_streaming;
    /* 0088 */ int32_t m_randomStart;
    /* 008C */ int32_t m_software;
    /* 0090 */ float m_coneInsideAngle;
    /* 0094 */ float m_coneOutsideAngle;
    /* 0098 */ float m_coneOutsideVolume;
    /* 009C */ C3Vector m_coneOrientation;
    /* 00A8 */ BOOL m_loaded;
    /* 00AC */ void (*m_finishedCallback)(SESound*, void*);
    /* 00B0 */ void* m_finishedParam;
    /* 00B4 */ void (*m_finishedCallback2)(SESound*, void*);
    /* 00B8 */ void* m_finishedParam2;
    /* 00BC */ void (*m_fadeOutCallback)(SESound*);
    /* 00C0 */ void (*m_dspChainCallback)(FMOD::Channel*, char*);
    /* 00C4 */ char* m_pszCustomDSPChainName;
    /* 00C8 */ uint64_t m_objectGUID;
    /* 00D0 */ uint32_t m_id;

    virtual ~SESoundInternal() = 0;
    virtual BOOL IsReadyForRelease();

    SESoundInternal();
    float GetScaledVolume();
    void Play();
    int32_t Set3DMinMaxDistance(float minDistance, float maxDistance);
    void UpdateVolume();
};

class SEFileSound : public SESoundInternal {
    public:
    /* 00D8 */ TSLink<SESoundInternal> m_goLink;
    /* 00E0 */ SFileImpl* m_file;
    /* 00E4 */ uint32_t unused_00E4;
    /* 00E8 */ uint32_t unused_00E8;
    /* 00EC */ FMOD::Sound* m_pFMODSound;
    /* 00F0 */ FMOD::SoundGroup* m_loadSoundGroup;
    /* 00F4 */ FMOD::SoundGroup* m_soundGroup;
    /* 00F8 */ uint8_t m_loadCompleted;
    /* 00FC */ SEWaveDataNode* m_pSoundCacheNode;

    virtual ~SEFileSound();
    virtual BOOL IsReadyForRelease();

    SEFileSound();
    void AbortNonBlockingLoad(FMOD_RESULT result);
    void CompleteNonBlockingLoad();
};

class SEMemorySound : public SESoundInternal {
    public:
    /* 00D8 */ FMOD::Sound* m_sound;

    virtual ~SEMemorySound();

    SEMemorySound();
};

class SEChatSound : public SESoundInternal {
    public:
    static FMOD_RESULT F_CALLBACK DSPReadCallback(FMOD_DSP_STATE* dspState, float* inBuffer, float* outBuffer, unsigned int length, int inChannels, int outChannels);
    static FMOD_RESULT F_CALLBACK DSPSeekCallback(FMOD_DSP_STATE* dspState, unsigned int position);

    /* 00D8 */ SEStreamCallbacks* m_callbacks;
    /* 00DC */ uint32_t m_position;
    /* 00E0 */ FMOD::DSP* m_dsp;

    virtual ~SEChatSound();

    SEChatSound();
};

struct SOUND_INTERNAL_LOOKUP : public TSHashObject<SOUND_INTERNAL_LOOKUP, HASHKEY_NONE> {
    /* 0018 */ SESoundInternal* m_sound;
};

class SESound {
    private:
    friend class SE2;
    friend class SESoundInternal;

    /* 0000 */ SESoundInternal* m_pMySESound;

    public:
    bool HasSound() const { return this->m_pMySESound != nullptr; } // (inferred)
    SESound();
    ~SESound();
    void AttachToObject(uint64_t guid);
    void CompleteNonBlockingLoad();
    int32_t Create2D(const char* pszFilename, int32_t looping, FMOD::SoundGroup* loadSoundGroup, FMOD::SoundGroup* soundGroup, bool randomStart, bool streaming, int32_t reverb, int32_t priority, uint32_t decodeBufferSize);
    int32_t Create3D(const char* pszFilename, int32_t looping, FMOD::SoundGroup* loadSoundGroup, FMOD::SoundGroup* soundGroup, bool randomStart, bool streaming, int32_t priority, int32_t software, float coneInsideAngle, float coneOutsideAngle, float coneOutsideVolume, const C3Vector* coneOrientation);
    void CreateChatSound(SEStreamCallbacks* callbacks, int32_t chatSystem);
    void Detach();
    void FadeInNow();
    void FadeOutNow();
    uint32_t GetID();
    uint32_t GetPosition();
    SESoundUserData* GetUserData();
    bool Is3D();
    bool IsLooping();
    bool IsPlaying();
    void Play();
    void PlaySoundFromMemory(const char* data, uint32_t length, int32_t chatSystem);
    void Set3DMinMaxDistance(float minDistance, float maxDistance);
    void Set3DPanLevel(float level);
    void Set3DPosition(const C3Vector& position);
    void SetAttachCustomDSPChainCallback(void (*fpCallback)(FMOD::Channel*, char*), const char* pszCustomDSPChainName);
    void SetChannelGroup(const char* name, bool childOfMaster);
    void SetFadeInTime(float seconds);
    void SetFadeOutTime(float seconds);
    void SetFinishedCallback(void (*callback)(SESound*, void*), void* param);
    void SetFinishedCallback2(void (*callback)(SESound*, void*), void* param);
    void SetPitch(float pitch);
    void SetPosition(uint32_t position);
    void SetUserData(SESoundUserData* pUserData);
    void SetVolume(float volume);
    void Stop(BOOL immediate, float fadeOutTime);
};

inline void SESoundInternal::AttachHandle(SESound& handle) {
    this->m_handle = &handle;
    handle.m_pMySESound = this;
}

class SE2 {
    private:
    static int32_t sm_bCaptureInitialized;

    public:
    static void ActivateAlternateCaptureCallback();
    static void ActivatePrimaryCaptureCallback();
    static FMOD_RESULT AddChannelToChannelGroup(FMOD::Channel* channel, FMOD::ChannelGroup* channelGroup);
    static bool AlternateCaptureCallbackIsActive();
    static FMOD_RESULT AttachMonitorDSP(SESound* sound);
    static FMOD_RESULT BypassDsp(FMOD::DSP* dsp, bool bypass);
    static void CaptureWork();
    static FMOD_RESULT ConnectDspToChannelGroup(FMOD::DSP* dsp, FMOD::ChannelGroup* channelGroup);
    static FMOD::ChannelGroup* CreateChannelGroup(const char* name);
    static FMOD_RESULT CreateDSPByType(FMOD_DSP_TYPE type, FMOD::DSP** dsp);
    static FMOD::SoundGroup* CreateSoundGroup(const char* name, int32_t maxAudible);
    static int32_t CreateWaveData(FMOD::System* pSystem, const char* filename, FMOD_MODE mode, SESound* pSoundHandle, FMOD::SoundGroup* loadSoundGroup, FMOD::SoundGroup* soundGroup, bool randomStart, int32_t priority, int32_t reverb, bool streaming, uint32_t decodeBufferSize, int32_t software, float coneInsideAngle, float coneOutsideAngle, float coneOutsideVolume, const C3Vector* coneOrientation);
    static FMOD_RESULT DestroyChannelGroup(FMOD::ChannelGroup* channelGroup);
    static void DestroyChatSystem();
    static FMOD_RESULT DspDestroy(FMOD::DSP* dsp);
    static FMOD_RESULT EnableDSP(FMOD::DSP* dsp, bool active);
    static void EnableReverb(BOOL enable);
    static void ErrCheck(uint32_t line, const char* file, FMOD_RESULT result);
    static int32_t GetChannelGroupNumChannels(FMOD::ChannelGroup* pChannelGroup);
    static float GetChannelGroupVolume(const char* name);
    static int32_t GetChannelInfo(bool fadingOnly, SEChannelInfo* info, int32_t maxCount, int32_t playing, BOOL isVirtual, int32_t loading, int32_t other, uint32_t* hardwareCount, uint32_t* softwareCount, uint32_t* virtualCount, uint32_t* loadingCount, uint32_t* otherCount);
    static int32_t GetChannelInfo(SEChannelInfo* info, int32_t maxCount, int32_t playing, BOOL isVirtual, int32_t loading, int32_t other, uint32_t* hardwareCount, uint32_t* softwareCount, uint32_t* virtualCount, uint32_t* loadingCount, uint32_t* otherCount);
    static char* GetCurrentChatInputDeviceName();
    static char* GetCurrentChatOutputDeviceName();
    static char* GetCurrentGameOutputDeviceName();
    static bool GetInputDriverName(int32_t index, char* name, int32_t nameLength, int32_t chatSystem);
    static bool GetInputDriverName_Cached(int32_t index, char* name, int32_t nameLength, int32_t chatSystem);
    static bool GetListenerPosition(C3Vector* position);
    static int32_t GetNumInputDrivers(int32_t chatSystem);
    static int32_t GetNumInputDrivers_Cached(int32_t chatSystem);
    static int32_t GetNumOutputDrivers(int32_t chatSystem);
    static int32_t GetNumOutputDrivers_Cached(int32_t chatSystem);
    static bool GetOutputDriverName(int32_t index, char* name, int32_t nameLength, int32_t chatSystem);
    static bool GetOutputDriverName_Cached(int32_t index, char* name, int32_t nameLength, int32_t chatSystem);
    static SESound* GetSoundFromID(uint32_t id, int32_t* created);
    static int32_t HeartBeat(const void* data, void* param);
    static int32_t Init(int32_t maxChannels, bool (*positionCallback)(uint64_t, C3Vector*), BOOL enableReverb, BOOL enableSoftwareHRTF, int32_t* numChannels, int32_t* outputDriverIndex, const char* outputDriverName, float (*volumeCallback)(SESoundUserData*), int32_t restart);
    static void InitCapture(int32_t* outputDriverIndex, const char* outputDriverName, int32_t* inputDriverIndex, const char* inputDriverName, float sampleRate);
    static bool IsCapturing();
    static BOOL IsChatSoundInitialized();
    static BOOL IsInitialized();
    static BOOL IsReverbEnabled();
    static void LockSoundList();
    static void Log_Write(const char* format, ...);
    static void Log_Write(uint32_t line, const char* file, FMOD_RESULT result, const char* format, ...);
    static void MuteAllSound(bool mute);
    static void MuteChannelGroup(const char* name, bool mute);
    static void PlaySoundFromMemory(FMOD::System* pSystem, const char* data, uint32_t length, FMOD_MODE mode, SESound* pSoundHandle);
    static void CreateChatSound(FMOD::System* pSystem, SEStreamCallbacks* callbacks, FMOD_MODE mode, SESound* pSoundHandle);
    static FMOD_RESULT ReleaseSoundGroup(FMOD::SoundGroup* soundGroup);
    static void SetAlternateCaptureCallback(SECaptureCallbacks* callbacks);
    static void SetChannelGroupVolume(const char* name, float volume);
    static FMOD_RESULT SetDeviceChangeCallback(void (*callback)());
    static FMOD_RESULT SetDspParam(FMOD::DSP* dsp, int32_t index, float value);
    static FMOD_RESULT SetFMODChannelGroupVolume(FMOD::ChannelGroup* channelGroup, float volume);
    static void SetListenerAttributes(const C3Vector* position, const C3Vector* forward, const C3Vector* up, const C3Vector* velocity);
    static void SetMasterVolume(float volume);
    static void SetPrimaryCaptureCallback(SECaptureCallbacks* callbacks);
    static void SetReverb(const FMOD_REVERB_PROPERTIES* properties);
    static void SetSoundGroupMaxAudibleBehavior(FMOD::SoundGroup* soundGroup, SE2_MAX_AUDIBLE_BEHAVIOR behavior, float muteFadeSpeed);
    static void SetUserMonitorDSPCallback(void (*callback)(float*, uint32_t));
    static void ShutDown(int32_t restart);
    static void ShutDownCapture();
    static void StartCapture();
    static void StopAllSounds(int32_t restart);
    static void StopAllSounds(const char* channelGroup, float fadeOutTime);
    static void StopCapture();
    static void UnlockSoundList();
    static void UnregisterHeartBeat();
};

#endif
