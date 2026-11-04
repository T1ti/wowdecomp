#ifndef WOW_SOURCE_WORLDCLIENT_PARTICULATE_H
#define WOW_SOURCE_WORLDCLIENT_PARTICULATE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"

class CTexture;

struct LiquidParticle {
    /* 0000 */ C3Vector m_position;
    /* 000C */ float m_size;
};

class Particulate {
    public:
    struct Movement {
        /* 0000 */ C3Vector m_dir; // Normalized direction of the sinusoidal displacement.
        /* 000C */ float m_freq; // Frequency: phase in cycles is elapsed time multiplied by this value.
        /* 0010 */ float m_time; // Elapsed time since InitMovement last reset the motion.
        /* 0014 */ float m_amplitude; // Amplitude of the per-update displacement.
    };

    /* 0000 */ LiquidParticle m_particles [4000];
    /* FA00 */ uint32_t m_maxParticles;
    /* FA04 */ C3Vector m_prevCameraPos;
    /* FA10 */ CTexture* m_texture;
    /* FA14 */ uint8_t m_enabled;
    /* FA15 */ uint8_t m_padding [3];
    /* FA18 */ float m_particleSize;
    /* FA1C */ float m_spawnAreaSize;
    /* FA20 */ float m_unkFloat1;
    /* FA24 */ uint32_t m_liquidType;
    /* FA28 */ Movement m_movement;

    Particulate(float particleSize, float spawnAreaSize, const char* textureName);
    ~Particulate();
    C3Vector ComputeMovement(float elapsed);
    void InitMovement();
    void InitParticles(uint32_t count);
    void Render();
    void Update();
    void SetScale(float scale);
    void SetTexture(const char* textureName);
};

#endif
