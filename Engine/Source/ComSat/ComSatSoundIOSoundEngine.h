#ifndef ENGINE_SOURCE_COMSAT_COMSATSOUNDIOSOUNDENGINE_H
#define ENGINE_SOURCE_COMSAT_COMSATSOUNDIOSOUNDENGINE_H

#include <cstddef>
#include <cstdint>
#include <cstring>
#include "Engine/Source/ComSat/ComSatInterface.h"
#include "Engine/Source/SoundEngine/SoundEngine.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Storm/Source/SLock.h"
#include "WoW/Source/SoundInterface/SoundInterface2.h"

namespace ComSat {
namespace Sound {

class IOCallback {
    public:
    virtual ~IOCallback();
    virtual void DeviceError(uint8_t player) = 0;
};

class IO {
    public:
    static IO* Create(const Initialization& init, IOCallback& callback, Allocator& allocator);

    virtual ~IO();
    virtual bool Init() = 0;
    virtual void Work(uint32_t time) = 0;
    virtual void Shutdown() = 0;
    virtual bool InitLocalPlayer(uint8_t player, int32_t outputDevice, int32_t captureDevice, int32_t sampleRate, int32_t bitsPerSample, int32_t samplesPerFrame) = 0;
    virtual bool DestroyLocalPlayer(uint8_t player) = 0;
    virtual bool LocalPlayerChangeOutputDevice(uint8_t player, int32_t device) = 0;
    virtual bool LocalPlayerChangeCaptureDevice(uint8_t player, int32_t device) = 0;
    virtual int32_t LocalPlayerGetCurrentOutputDeviceIndex(uint8_t player) = 0;
    virtual int32_t LocalPlayerGetCurrentCaptureDeviceIndex(uint8_t player) = 0;
    virtual void SetPlayerPlaybackVolume(uint8_t player, int32_t channel, float volume) = 0;
    virtual bool PlayDataForPlayer(uint8_t player, int32_t channel, uint8_t* data, uint32_t size, double rate) = 0;
    virtual int32_t GetRemainingPlaybackBytes(uint8_t player, int32_t channel) = 0;
    virtual int32_t GetInputBlockSize(uint8_t player) = 0;
    virtual int32_t GetPlaybackBlockSize(uint8_t player) = 0;
    virtual int32_t ReadDataForPlayer(uint8_t player, uint8_t* data, int32_t bytesToRead) = 0;
    virtual int32_t GetAvailableDataBytes(uint8_t player) = 0;
    virtual int32_t EnumerateOutputDevices(DeviceInfo* devices, int32_t maxDevices) = 0;
    virtual int32_t EnumerateCaptureDevices(DeviceInfo* devices, int32_t maxDevices) = 0;
    virtual void DebugSetSoundOutputDelay(uint8_t player, int32_t delay) = 0;
};

template <class T>
class RingBuffer {
    public:
    /* 0000 */ int32_t m_position;
    /* 0004 */ uint32_t m_count;
    /* 0008 */ SCritSect m_lock;
    /* 0030 */ T m_samples[0x4000];

    RingBuffer();
    uint32_t Count();
    uint32_t Read(T* samples, uint32_t count);
    bool Write(const T* samples, uint32_t count);
};

class CaptureChannel : public SECaptureCallbacks {
    public:
    /* 0004 */ Allocator* m_allocator;
    /* 0008 */ RingBuffer<int16_t> m_buffer;

    virtual ~CaptureChannel();
    virtual void ProcessData(int16_t* data, uint32_t samples);

    CaptureChannel(Allocator& allocator);
};

class OutputChannel : public SEStreamCallbacks {
    public:
    /* 0004 */ Allocator* m_allocator;
    /* 0008 */ SE2SoundKitObject m_object;
    /* 001C */ RingBuffer<float> m_buffer;

    virtual ~OutputChannel();
    virtual bool GetSamples(uint32_t position, uint32_t count, float* buffer);
    virtual void Delete();

    OutputChannel(Allocator& allocator);
};

class IOSoundEngine : public IO {
    public:
    /* 0004 */ Allocator* m_allocator;
    /* 0008 */ IOCallback* m_callback;
    /* 000C */ TSGrowableArray<OutputChannel*> m_channels;
    /* 001C */ CaptureChannel m_captureChannel;
    /* 8054 */ int32_t m_outputDevice;
    /* 8058 */ int32_t m_captureDevice;
    /* 805C */ float m_sampleRate;
    /* 8060 */ int32_t m_bitsPerSample;
    /* 8064 */ int32_t m_samplesPerFrame;

    virtual ~IOSoundEngine();
    virtual bool Init();
    virtual void Work(uint32_t time);
    virtual void Shutdown();
    virtual bool InitLocalPlayer(uint8_t player, int32_t outputDevice, int32_t captureDevice, int32_t sampleRate, int32_t bitsPerSample, int32_t samplesPerFrame);
    virtual bool DestroyLocalPlayer(uint8_t player);
    virtual bool LocalPlayerChangeOutputDevice(uint8_t player, int32_t device);
    virtual bool LocalPlayerChangeCaptureDevice(uint8_t player, int32_t device);
    virtual int32_t LocalPlayerGetCurrentOutputDeviceIndex(uint8_t player);
    virtual int32_t LocalPlayerGetCurrentCaptureDeviceIndex(uint8_t player);
    virtual void SetPlayerPlaybackVolume(uint8_t player, int32_t channel, float volume);
    virtual bool PlayDataForPlayer(uint8_t player, int32_t channel, uint8_t* data, uint32_t size, double rate);
    virtual int32_t GetRemainingPlaybackBytes(uint8_t player, int32_t channel);
    virtual int32_t GetInputBlockSize(uint8_t player);
    virtual int32_t GetPlaybackBlockSize(uint8_t player);
    virtual int32_t ReadDataForPlayer(uint8_t player, uint8_t* data, int32_t bytesToRead);
    virtual int32_t GetAvailableDataBytes(uint8_t player);
    virtual int32_t EnumerateOutputDevices(DeviceInfo* devices, int32_t maxDevices);
    virtual int32_t EnumerateCaptureDevices(DeviceInfo* devices, int32_t maxDevices);
    virtual void DebugSetSoundOutputDelay(uint8_t player, int32_t delay);

    IOSoundEngine(const Initialization& init, IOCallback& callback, Allocator& allocator);
    void InitCapture();
    void ShutDownCapture();
};

}
}

// INLINED
template <class T>
ComSat::Sound::RingBuffer<T>::RingBuffer() : m_position(0), m_count(0) {
}

// INLINED
template <class T>
uint32_t ComSat::Sound::RingBuffer<T>::Count() {
    this->m_lock.Enter();
    uint32_t count = this->m_count;
    this->m_lock.Leave();
    return count;
}

// VERIFY: 815eb058 manual
// OFFSET: [0x00887590<float>, 0x008876F0<short>]
template <class T>
uint32_t ComSat::Sound::RingBuffer<T>::Read(T* samples, uint32_t count) {
    uint32_t read = 0;
    this->m_lock.Enter();
    if (this->m_count) {
        int32_t position = this->m_position - this->m_count;
        if (position < 0) {
            position += 0x4000;
        }
        read = count;
        if (count >= this->m_count) {
            read = this->m_count;
        }
        this->m_count -= read;
        for (uint32_t i = 0; i < read; i++) {
            *samples = this->m_samples[position];
            position = (position + 1) & 0x3FFF;
            samples++;
        }
        if (read < count) {
            memset(samples, 0, sizeof(T) * (count - read));
        }
    }
    this->m_lock.Leave();
    return read;
}

// VERIFY: a7947d25 auto
// OFFSET: [0x00887520<float>, 0x00887680<short>]
template <class T>
bool ComSat::Sound::RingBuffer<T>::Write(const T* samples, uint32_t count) {
    this->m_lock.Enter();
    if (count > 0x4000 - this->m_count) {
        this->m_lock.Leave();
        return false;
    }
    this->m_count += count;
    int32_t position = this->m_position;
    for (; count; count--) {
        this->m_samples[position] = *samples;
        samples++;
        position = (position + 1) & 0x3FFF;
    }
    this->m_position = position;
    this->m_lock.Leave();
    return true;
}

#endif
