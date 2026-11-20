#include <cfloat>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include "Engine/Source/ComSat/SPIRIT/ipmr.h"

extern "C" {

// STATIC: 0x00B23390
const float s_ipmrTableB23390[40] = {
    0.5903669f, 0.406370461f, -0.0620280392f, -0.159388974f, 0.0814775005f, 0.00397018529f,
    0.0543894358f, -0.0406994298f, -0.112167686f, 0.118772492f, 0.132412344f, -0.319878697f,
    0.206545874f, -0.010494817f, -0.00251173857f, -0.0601136871f, -0.0154648796f, 0.151835918f,
    -0.134953573f, -0.03094282f, 0.160174966f, -0.147234395f, 0.032956019f, 0.0840905532f,
    -0.114270478f, 0.0557867363f, -0.0252340231f, 0.0962928757f, -0.159430519f, 0.0965888202f,
    0.0035386621f, -0.009377487f, -0.00222194614f, -0.0873734728f, 0.167061999f, -0.0965164229f,
    0.0196132697f, -0.112730898f, 0.179745913f, 0.0610430539f,
};

// STATIC: 0x00B23430
const float s_ipmrTableB23430[40] = {
    0.928170383f, 0.0419551693f, 0.00404424872f, -0.0569161251f, 0.100727193f, -0.119626194f,
    0.105895095f, -0.0647530556f, 0.0128855687f, 0.0289492682f, -0.0455433615f, 0.034036722f,
    -0.00430110609f, -0.0271745548f, 0.0464040935f, -0.0489278436f, 0.0409219339f, -0.0340180732f,
    0.0370852798f, -0.0499495864f, 0.0628596693f, -0.0621221587f, 0.0386857055f, 0.00534586515f,
    -0.0562007539f, 0.0944953039f, -0.104716904f, 0.0830429867f, -0.0393133238f, -0.00780669414f,
    0.0397487246f, -0.0462544002f, 0.0287352446f, 0.00245569577f, -0.0346562378f, 0.0592941307f,
    -0.0744847879f, 0.0828922316f, -0.0869466439f, 0.0850813091f,
};

// VERIFY: 6e74ab0c manual
// OFFSET: 0x0089E0B0
void SPIRIT_IP_MR_V3_NAME_109(int16_t mode, const SPIRIT_IPMR_TRACK* tracks, int16_t first, int16_t last, float* out) {
    if (mode == 2) {
        SPIRIT_IP_MR_V3_NAME_35(tracks, first, last, out);
        return;
    }

    SPIRIT_IP_MR_V3_NAME_333(out);
    const float* shape = mode == 1 ? s_ipmrTableB23430 : s_ipmrTableB23390;

    for (int16_t t = first; t <= last; t++) {
        const SPIRIT_IPMR_TRACK* track = &tracks[t];
        float pulse[40];
        SPIRIT_IP_MR_V3_NAME_191(pulse, shape);
        float gain = track->unk_0004[0];
        SPIRIT_IP_MR_V3_NAME_248(pulse, gain);

        for (int16_t i = 0; i < track->unk_0000; i++) {
            int32_t position = track->unk_0018[i];

            if (track->unk_0004[i] == gain) {
                SPIRIT_IP_MR_V3_NAME_179(&out[position], pulse, 40 - position);
                SPIRIT_IP_MR_V3_NAME_179(out, &pulse[40 - position], position);
            } else {
                SPIRIT_IP_MR_V3_NAME_321(&out[position], pulse, 40 - position);
                SPIRIT_IP_MR_V3_NAME_321(out, &pulse[40 - position], position);
            }
        }
    }
}

// VERIFY: 153e6aae manual
// OFFSET: 0x0089E260
void SPIRIT_IP_MR_V3_NAME_156(float* excitation, const float* target, const float* response, const float* a, const float* weighted, const SPIRIT_IPMR_TRACK* tracks, float* memory, float* out, int16_t count) {
    float synthesized[40];
    float pulses[40];

    for (int16_t i = 0; i < tracks->unk_0000; i++) {
        excitation[tracks->unk_0018[i]] = tracks->unk_0004[i] + excitation[tracks->unk_0018[i]];
    }

    SPIRIT_IP_MR_V3_NAME_148(excitation, synthesized, a, memory, 1);

    for (int16_t i = 0; i < 10; i++) {
        memory[10 + i] = target[30 + i] - synthesized[30 + i];
        memory[20 + i] = response[30 + i] - tracks->unk_020C[30 + i];
    }

    SPIRIT_IP_MR_V3_NAME_191(out, synthesized);

    for (int16_t t = 1; t < count; t++) {
        float* block = &memory[30 * t];
        SPIRIT_IP_MR_V3_NAME_35(tracks, t, t, pulses);
        SPIRIT_IP_MR_V3_NAME_148(pulses, synthesized, a, block, 1);
        SPIRIT_IP_MR_V3_AddSubframe(out, synthesized);
        SPIRIT_IP_MR_V3_NAME_190(block + 10, &synthesized[30]);
        SPIRIT_IP_MR_V3_NAME_190(block + 20, &tracks[t].unk_020C[30]);
    }
}

// VERIFY: b32a662b manual
// OFFSET: 0x0089E3A0
void SPIRIT_IP_MR_V3_SearchTrackPulses(SPIRIT_IPMR_TRACK* track, float* target, const float* h, float* correlation, const float* phi, int16_t* used, int16_t skip) {
    int32_t count = track->unk_0000;
    float d[40];
    SPIRIT_IP_MR_V3_NAME_191(d, correlation);
    float best = 0.0f;

    for (int32_t i = 0; i < 40; i++) {
        if (!used[i]) {
            float value = fabsf(d[i]);

            if (best <= value) {
                best = value;
                track->unk_0018[0] = static_cast<int16_t>(i);
            }
        }
    }

    best = fabsf(best / phi[0]);
    track->unk_0004[0] = d[track->unk_0018[0]] < 0.0f ? -best : best;
    used[track->unk_0018[0]] = 1;

    for (int32_t k = 1; k < count; k++) {
        best = -FLT_MAX;
        float gain = track->unk_0004[k - 1];
        int32_t previous = track->unk_0018[k - 1];

        for (int32_t i = 0; i < 40; i++) {
            if (!used[i]) {
                d[i] = d[i] - phi[i - previous] * gain;
                float value = fabsf(d[i]);

                if (best < value) {
                    best = value;
                    track->unk_0018[k] = static_cast<int16_t>(i);
                }
            }
        }

        best = fabsf(best / phi[0]);
        track->unk_0004[k] = 0.0f < d[track->unk_0018[k]] ? best : -best;
        used[track->unk_0018[k]] = 1;
    }

    float pulses[40];
    SPIRIT_IP_MR_V3_NAME_333(pulses);

    for (int32_t k = 0; k < count; k++) {
        int32_t position = track->unk_0018[k];

        if (0.0f < track->unk_0004[k]) {
            for (int32_t q = position; q < 40; q++) {
                pulses[q] = h[q - position] + pulses[q];
            }
        } else {
            for (int32_t q = position; q < 40; q++) {
                pulses[q] = pulses[q] - h[q - position];
            }
        }
    }

    int16_t index = 0;
    double numerator = SPIRIT_IP_MR_V3_NAME_42(target, pulses, 40);
    float ratio = static_cast<float>(fabs(numerator / SPIRIT_IP_MR_V3_NAME_42(pulses, pulses, 40)));
    ratio = static_cast<float>(log(ratio));
    SPIRIT_IP_MR_V3_QuantizeUniform(32, &ratio, &index, 0.7f, 10.18f);
    track->unk_0028 = index;
    float exponent = static_cast<float>(index * static_cast<double>(0.30580646f) + static_cast<double>(0.7f));
    float gain = static_cast<float>(exp(exponent));

    for (int32_t k = 0; k < count; k++) {
        track->unk_0004[k] = 0.0f < track->unk_0004[k] ? gain : -gain;
    }

    SPIRIT_IP_MR_V3_NAME_248(pulses, gain);
    SPIRIT_IP_MR_V3_NAME_322(target, pulses);

    if (!skip) {
        for (int32_t m = 0; m < 40; m++) {
            for (int32_t k = 0; k < count; k++) {
                correlation[m] = correlation[m] - phi[abs(m - track->unk_0018[k])] * track->unk_0004[k];
            }
        }
    }

    SPIRIT_IP_MR_V3_NAME_191(track->unk_020C, pulses);
}

// VERIFY: 05c0b3ee manual
// OFFSET: 0x0089E800
void SPIRIT_IP_MR_V3_NAME_59(float* target, SPIRIT_IPMR_TRACK* tracks, int16_t count, const float* h, int16_t skip) {
    int16_t used[40] = {};
    float correlation[40];
    float phiStorage[79];
    float* phi = &phiStorage[39];

    for (int16_t k = 0; k < 40; k++) {
        correlation[k] = SPIRIT_IP_MR_V3_NAME_42(&target[k], h, 40 - k);
    }

    for (int16_t k = 0; k < 40; k++) {
        float value = SPIRIT_IP_MR_V3_NAME_42(h, &h[k], 40 - k);
        phi[k] = value;
        phi[-k] = value;
    }

    SPIRIT_IP_MR_V3_SearchTrackPulses(tracks, target, h, correlation, phi, used, skip);

    for (int16_t t = 1; t < count; t++) {
        if (t >= 6) {
            for (int32_t i = 0; i < 40; i++) {
                used[i] = 0;
            }
        }

        if (skip) {
            SPIRIT_IP_MR_V3_NAME_322(target, tracks[t].unk_00CC);

            for (int16_t k = 0; k < 40; k++) {
                correlation[k] = SPIRIT_IP_MR_V3_NAME_42(h, &target[k], 40 - k);
            }
        }

        SPIRIT_IP_MR_V3_SearchTrackPulses(&tracks[t], target, h, correlation, phi, used, skip);
    }
}

// VERIFY: c733f5aa auto
// OFFSET: 0x0089EAE0
void SPIRIT_IP_MR_V3_SearchFixedCodebook(float* target, const float* h, const float* unused, SPIRIT_IPMR_TRACK* tracks, int16_t count, int16_t skip, const SPIRIT_IPMR_TABLES* tables) {
    SPIRIT_IP_MR_V3_NAME_59(target, tracks, count, h, skip);
    SPIRIT_IP_MR_V3_SortPulses(count, tracks);
    SPIRIT_IP_MR_V3_EncodePulsePositions(tracks, count, tables);
}

// VERIFY: bc864f9d manual
// OFFSET: 0x0089EB20
void SPIRIT_IP_MR_V3_NAME_149(SPIRIT_IPMR_TRACK* tracks, int16_t count, const SPIRIT_IPMR_TABLES* tables) {
    int16_t used[40] = {};

    for (int16_t t = 0; t < count; t++) {
        SPIRIT_IPMR_TRACK* track = &tracks[t];
        int32_t code = track->unk_0024;
        int16_t pulses = track->unk_0000;
        int16_t k = pulses - 1;

        if (code < 0) {
            for (int16_t i = t; i < count; i++) {
                tracks[i].unk_0000 = 0;
            }

            return;
        }

        if (t >= 6) {
            for (int32_t i = 0; i < 40; i++) {
                used[i] = 0;
            }
        }

        float exponent = static_cast<float>(track->unk_0028 * static_cast<double>(0.30580646f) + static_cast<double>(0.7f));
        float gain = static_cast<float>(exp(exponent));

        for (int16_t i = 39; i >= 0; i--) {
            int32_t combinations = tables->unk_B8AC[i][k];

            if (code >= combinations) {
                code -= combinations;
                track->unk_0004[k] = track->unk_002A & (1 << k) ? -gain : gain;
                track->unk_0018[k] = i;
                k--;
            }

            if (k < 0) {
                break;
            }
        }

        int16_t found = 0;
        int32_t slot = 0;

        for (int16_t j = 0; j < 40; j++) {
            if (!used[j]) {
                if (track->unk_0018[found] == slot) {
                    track->unk_0018[found] = j;

                    if (j >= 40 || j < 0) {
                        for (int16_t i = 0; i < count; i++) {
                            tracks[i].unk_0000 = 0;
                        }

                        return;
                    }

                    used[j] = 1;
                    found++;
                }

                slot++;
            }

            if (found == pulses) {
                break;
            }
        }
    }
}

}
