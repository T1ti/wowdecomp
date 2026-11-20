#include <cfloat>
#include <cstdint>
#include <cstring>
#include "Engine/Source/ComSat/SPIRIT/ipmr.h"

extern "C" {

// VERIFY: 9f95e8b0 manual
// OFFSET: 0x0089FD90
void SPIRIT_IP_MR_V3_AddVector(int32_t count, float* dst, const float* src) {
    for (int32_t i = 0; i < count; i++) {
        dst[i] = src[i] + dst[i];
    }
}

// VERIFY: 259fd37d manual
// OFFSET: 0x0089FE20
float* SPIRIT_IP_MR_V3_CopyVector(int32_t count, float* dst, const float* src) {
    for (int32_t i = 0; i < count; i++) {
        dst[i] = src[i];
    }

    return dst;
}

// VERIFY: 6985ec60 manual
// OFFSET: 0x0089FEA0
void SPIRIT_IP_MR_V3_SubVector(int32_t count, float* dst, const float* src) {
    for (int32_t i = 0; i < count; i++) {
        dst[i] = dst[i] - src[i];
    }
}

// VERIFY: b5bd5cc8 manual
// OFFSET: 0x0089FF30
float SPIRIT_IP_MR_V3_NAME_331(const float* codebook, const float* target, const float* mean, const float* predictor, const float* previous, const int32_t* sizes, const float* weights, float* quantized, int32_t* indices, int32_t stages) {
    int32_t paths[64];
    int32_t parents[2][8];
    float distances[2][8];
    float residuals[2][8][10];
    float t[10];

    if (16 * stages > 0) {
        memset(paths, 0, stages * 64);
    }

    memset(parents, 0, sizeof(parents));
    int32_t count = 0;
    int32_t* prevPaths = paths;
    int32_t* lastPaths = &paths[8 * stages];
    int32_t* newParents = parents[0];
    int32_t* spareParents = parents[1];
    float* prevDist = distances[1];
    float* spareDist = distances[0];
    float (*prevRes)[10] = residuals[0];
    float (*spareRes)[10] = residuals[1];

    float* x = SPIRIT_IP_MR_V3_CopyVector(10, t, target);

    if (mean) {
        SPIRIT_IP_MR_V3_SubVector(10, x, mean);

        if (predictor && previous) {
            for (int32_t i = 0; i < 10; i++) {
                t[i] = t[i] - (previous[i] - mean[i]) * predictor[i];
            }
        }
    }

    float initial = 0.0f;

    for (int32_t i = 0; i < 10; i++) {
        initial += t[i] * weights[i] * t[i];
    }

    for (int32_t i = 0; i < 8; i++) {
        prevDist[i] = initial;
    }

    for (int32_t c = 0; c < 8; c++) {
        for (int32_t i = 0; i < 10; i++) {
            prevRes[c][i] = t[i];
        }
    }

    const float* entry = codebook;
    int32_t parentCount = 1;

    for (int32_t s = 0; s < stages; s++) {
        const float* stageBase = entry;
        int32_t* newPaths = prevPaths;
        prevPaths = lastPaths;
        lastPaths = newPaths;
        int32_t* stageParents = newParents;
        newParents = spareParents;
        spareParents = stageParents;
        float* newDist = spareDist;
        spareDist = prevDist;
        float (*newRes)[10] = spareRes;
        spareRes = prevRes;
        int32_t size = sizes[s];
        int32_t slot = 0;

        for (int32_t i = 0; i < 8; i++) {
            newDist[i] = FLT_MAX;
        }

        for (int32_t e = 0; e < size; e++) {
            float weighted[10];
            float energy = 0.0f;

            for (int32_t i = 0; i < 10; i++) {
                weighted[i] = weights[i] * entry[i];
                energy += weighted[i] * entry[i];
            }

            for (int32_t i = 0; i < 10; i++) {
                weighted[i] = weighted[i] * -2.0f;
            }

            entry += 10;

            for (int32_t j = 0; j < parentCount; j++) {
                float cross = 0.0f;

                for (int32_t i = 0; i < 10; i++) {
                    cross += prevRes[j][i] * weighted[i];
                }

                float dist = prevDist[j] + energy + cross;

                if (newDist[slot] < dist) {
                    continue;
                }

                newDist[slot] = dist;
                newPaths[slot * stages + s] = e;
                stageParents[slot] = j;

                if (count >= 256) {
                    continue;
                }

                if (++count >= 256) {
                    if (newDist[1] < dist) {
                        slot = 1;
                    }

                    for (int32_t i = 2; i < 8; i++) {
                        if (newDist[slot] > newDist[i]) {
                            slot = i;
                        }
                    }
                } else {
                    float worst = dist;
                    slot = 0;

                    if (newDist[1] > dist) {
                        worst = newDist[1];
                        slot = 1;
                    }

                    for (int32_t i = 2; i < 8; i++) {
                        if (worst < newDist[i]) {
                            worst = newDist[i];
                            slot = i;
                        }
                    }
                }
            }
        }

        for (int32_t c = 0; c < 8; c++) {
            int32_t p = stageParents[c];
            const float* vector = &stageBase[10 * newPaths[c * stages + s]];

            for (int32_t i = 0; i < 10; i++) {
                newRes[c][i] = prevRes[p][i] - vector[i];
            }

            for (int32_t i = 0; i < s; i++) {
                newPaths[c * stages + i] = prevPaths[p * stages + i];
            }
        }

        int32_t product = size * parentCount;
        parentCount = product <= 8 ? product : 8;
        prevDist = newDist;
        prevRes = newRes;
    }

    int32_t best = prevDist[0] > prevDist[1] ? 1 : 0;

    for (int32_t i = 2; i < 8; i++) {
        if (prevDist[best] > prevDist[i]) {
            best = i;
        }
    }

    float result = prevDist[best];

    if (indices && stages > 0) {
        memcpy(indices, &lastPaths[best * stages], stages * sizeof(int32_t));
    }

    if (quantized) {
        if (mean) {
            for (int32_t i = 0; i < 10; i++) {
                quantized[i] = mean[i];
            }
        } else {
            for (int32_t i = 0; i < 10; i++) {
                quantized[i] = 0.0f;
            }
        }

        if (predictor && previous) {
            for (int32_t i = 0; i < 10; i++) {
                quantized[i] = (previous[i] - mean[i]) * predictor[i] + quantized[i];
            }
        }

        const float* stage = codebook;

        for (int32_t i = 0; i < stages; i++) {
            SPIRIT_IP_MR_V3_AddVector(10, quantized, &stage[10 * lastPaths[best * stages + i]]);
            stage += 10 * sizes[i];
        }
    }

    return result;
}

}
