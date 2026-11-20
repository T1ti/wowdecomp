#include <cfloat>
#include <cmath>
#include <cstdint>
#include "Engine/Source/ComSat/SPIRIT/ipmr.h"

extern "C" {

// STATIC: 0x00B23744
const int32_t s_ipmrTableB23744[4] = {
    128, 128, 128, 64,
};

// STATIC: 0x00B23758
const int32_t s_ipmrTableB23758[3] = {
    128, 128, 64,
};

// VERIFY: 0a95eb16 manual
// OFFSET: 0x0089C6F0
int32_t SPIRIT_IP_MR_V3_NAME_121(float* lsp, const float* lpc, int32_t* indices, const float* previous, const SPIRIT_IPMR_TABLES* tables) {
    float lsf[10];
    float weights[10];
    float quantized[10];
    float previousLsf[10];
    SPIRIT_IP_MR_V3_NAME_216(lsp, lsf);

    if (previous) {
        SPIRIT_IP_MR_V3_NAME_216(previous, previousLsf);
    }

    SPIRIT_IP_MR_V3_NAME_330(weights, lsf, lpc);

    if (previous) {
        float first[10];
        float second[10];
        int32_t firstIndices[3];
        int32_t secondIndices[3];
        SPIRIT_IP_MR_V3_NAME_331(tables->unk_4600, lsf, tables->unk_B850, tables->unk_B7F0, previousLsf, s_ipmrTableB23758, weights, first, firstIndices, 3);
        SPIRIT_IP_MR_V3_NAME_331(tables->unk_7800, lsf, tables->unk_B850, tables->unk_B820, previousLsf, s_ipmrTableB23758, weights, second, secondIndices, 3);

        float secondError = 0.0f;
        float firstError = 0.0f;

        for (int32_t i = 0; i < 10; i++) {
            float diff = lsf[i] - second[i];
            secondError = diff * diff * weights[i] + secondError;
        }

        for (int32_t i = 0; i < 10; i++) {
            float diff = lsf[i] - first[i];
            firstError = diff * diff * weights[i] + firstError;
        }

        if (firstError <= secondError) {
            SPIRIT_IP_MR_V3_NAME_190(quantized, first);
            indices[0] = firstIndices[0];
            indices[1] = firstIndices[1];
            indices[2] = firstIndices[2];
            indices[3] = 1;
        } else {
            SPIRIT_IP_MR_V3_NAME_190(quantized, second);
            indices[0] = secondIndices[0];
            indices[1] = secondIndices[1];
            indices[2] = secondIndices[2];
            indices[3] = 0;
        }
    } else {
        SPIRIT_IP_MR_V3_NAME_331(tables->unk_0000, lsf, tables->unk_B7C0, nullptr, nullptr, s_ipmrTableB23744, weights, quantized, indices, 4);
    }

    int32_t result = SPIRIT_IP_MR_V3_SortLsf(quantized);

    for (int32_t i = 0; i < 10; i++) {
        lsp[i] = static_cast<float>(cos(quantized[i] * static_cast<double>(3.1415927f)));
    }

    return result;
}

// VERIFY: 19f6be2b manual
// OFFSET: 0x0089CB70
void SPIRIT_IP_MR_V3_NAME_78(SPIRIT_IPMR_ENC_UNK0D20* state, const SPIRIT_IPMR_TABLES* tables, float* lpc, float* weighted, SPIRIT_IPMR_FRAME* frame) {
    float analysis[4][10];
    float lsp[4][10];
    float weights[4][10];
    float current[10];
    const float* signal = state->unk_0010;

    for (int32_t sf = 0; sf < 4; sf++) {
        SPIRIT_IP_MR_V3_ComputeLpc(analysis[sf], signal, tables);

        if (SPIRIT_IP_MR_V3_LpcToLsp(analysis[sf], lsp[sf], tables->unk_B660)) {
            SPIRIT_IP_MR_V3_NAME_190(state->unk_1988, lsp[sf]);
        } else {
            SPIRIT_IP_MR_V3_NAME_190(lsp[sf], state->unk_1988);
        }

        signal += 40;
    }

    SPIRIT_IP_MR_V3_NAME_190(current, lsp[3]);

    if (frame->unk_02B0 != 0 && frame->unk_02B0 != 1) {
        SPIRIT_IP_MR_V3_NAME_189(lpc, analysis[0], 40);
        SPIRIT_IP_MR_V3_NAME_190(state->unk_19B0, current);
        return;
    }

    if (frame->unk_0290) {
        SPIRIT_IP_MR_V3_NAME_391(weighted, analysis[0], tables->unk_B760, tables->unk_B790);
    } else {
        SPIRIT_IP_MR_V3_NAME_391(weighted, analysis[0], tables->unk_AE80[frame->unk_02AE - 1], tables->unk_B090[frame->unk_02AE - 1]);
    }

    SPIRIT_IP_MR_V3_FilterSubframes(state->unk_1FA0, weighted, state->unk_1F9C, state->unk_1A00);

    for (int32_t sf = 0; sf < 4; sf++) {
        SPIRIT_IP_MR_V3_NAME_330(weights[sf], lsp[sf], analysis[sf]);
    }

    float change = SPIRIT_IP_MR_V3_SquaredDistance10(state->unk_1938, lsp[3]);

    for (int32_t sf = 0; sf < 3; sf++) {
        float distance = SPIRIT_IP_MR_V3_SquaredDistance10(state->unk_1938, lsp[sf]);

        if (distance > change) {
            change = distance;
        }
    }

    if (state->unk_1F78 > change && state->unk_1F94 < 30 && state->unk_1FB8 != 3 && state->unk_1FB8 != 4) {
        state->unk_1F94++;

        if (state->unk_1F94 > 100) {
            state->unk_1F94 = 100;
        }

        frame->unk_02B4 = -1;
        SPIRIT_IP_MR_V3_NAME_121(current, analysis[3], frame->unk_02C8, state->unk_1960, tables);
        SPIRIT_IP_MR_V3_NAME_205(state->unk_19B0, current, 3, lpc);
        SPIRIT_IP_MR_V3_NAME_190(state->unk_19B0, current);
        return;
    }

    SPIRIT_IP_MR_V3_NAME_190(state->unk_1938, current);
    int32_t stable = state->unk_1F94;

    if (stable >= 15) {
        state->unk_1F78 = state->unk_1F78 - 0.006f;

        if (0.04f > state->unk_1F78) {
            state->unk_1F78 = 0.04f;
        }
    }

    if (stable <= 3) {
        state->unk_1F78 = 0.006f + state->unk_1F78;

        if (0.08f < state->unk_1F78) {
            state->unk_1F78 = 0.08f;
        }
    }

    state->unk_1F94 = 0;
    float lsf[10];
    float lsfWeights[10];
    float quantized[10];
    SPIRIT_IP_MR_V3_NAME_216(current, lsf);
    SPIRIT_IP_MR_V3_NAME_330(lsfWeights, lsf, analysis[3]);
    SPIRIT_IP_MR_V3_NAME_331(tables->unk_0000, lsf, tables->unk_B7C0, nullptr, nullptr, s_ipmrTableB23744, lsfWeights, quantized, frame->unk_02C8, 4);
    SPIRIT_IP_MR_V3_SortLsf(quantized);

    for (int32_t i = 0; i < 10; i++) {
        current[i] = static_cast<float>(cos(quantized[i] * static_cast<double>(3.1415927f)));
    }

    SPIRIT_IP_MR_V3_NAME_190(state->unk_1960, current);
    float best = FLT_MAX;
    frame->unk_02B4 = 0;
    float diff[10];

    for (int32_t i = 0; i < 10; i++) {
        diff[i] = current[i] - lsp[3][i];
    }

    for (int16_t row = 0; row < 3; row++) {
        float error = 0.0f;

        for (int32_t sf = 0; sf < 3; sf++) {
            float weight = s_ipmrTableA58F70[row][sf];

            for (int32_t i = 0; i < 10; i++) {
                float e = current[i] * weight + state->unk_19B0[i] * (1.0f - weight) - lsp[sf][i];
                error = e * e * weights[sf][i] + error;
            }
        }

        for (int32_t i = 0; i < 10; i++) {
            error = diff[i] * diff[i] * weights[3][i] + error;
        }

        if (error < best) {
            best = error;
            frame->unk_02B4 = row;
        }
    }

    SPIRIT_IP_MR_V3_NAME_205(state->unk_19B0, current, frame->unk_02B4, lpc);
    SPIRIT_IP_MR_V3_NAME_190(state->unk_19B0, current);
}

}
